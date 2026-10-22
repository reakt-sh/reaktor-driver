"""
User-facing connection for managing communication with the REAKTOR motor controller.
"""

import asyncio
from datetime import datetime
from typing import Callable
from inspect import iscoroutinefunction
from serial_asyncio import create_serial_connection

from ..errors import ConnectionProblem
from .data import PowerConsumption
from .serial_connection import PowerSensorSerialConnection
from .protocol import PowerSensorProtocol
from .config import POWER_METER_BAUDRATE, POWER_METER_BYTESIZE, POWER_METER_PARITY, POWER_METER_STOPBITS, POWER_METER_TIMEOUT
from . import POWER_ROOT_LOGGER

logger = POWER_ROOT_LOGGER.getChild("connection")

class PowerSensorConnection:
    """Connection to the power sensor."""

    _serial: PowerSensorSerialConnection # Internal serial connection handler
    _decoder: PowerSensorProtocol # Message handler for en/decoding messages
    _polling_task: asyncio.Task # Background task for automatic data polling
    _listeners: list[Callable[[PowerConsumption], None]] # Listeners for data updates
    _listener_futures: list[asyncio.Future] # Futures for one-time data listeners
    _listener_tasks: set # Background tasks for async listeners
    _problems: list[ConnectionProblem] # List of connection problems
    _pending_request: bool # Flag to indicate if a data request is pending
    _poll: bool # Flag to indicate if automatic polling is enabled
    _poll_delay_ms: int # Delay between automatic polls in milliseconds
    _reconnect: bool # Flag to indicate if automatic reconnection is enabled
    current_data: PowerConsumption # Latest received data

    def __init__(self, automatic_polling: bool = True, poll_delay_ms: int = 0, automatic_reconnect: bool = True):
        self._serial = None
        self._decoder = None
        self._polling_task = None
        self._listeners = []
        self._listener_futures = []
        self._listener_tasks = set()
        self._problems = []
        self._pending_request = False
        self._poll = automatic_polling
        self._poll_delay_ms = poll_delay_ms
        self._reconnect = automatic_reconnect
        self.current_data = None

    # Internal API

    def _handle_error(self, problem: ConnectionProblem):
        """Register a connection problem."""
        self._problems.append(problem)
        self._notify_listeners(problem)

    def _handle_data(self, data: PowerConsumption):
        """Store data and notify all registered listeners about new data."""
        self.current_data = data
        self._pending_request = False
        self._notify_listeners(data)

    def _notify_listeners(self, event: PowerConsumption | ConnectionProblem):
        """Notify all registered listeners about new data or a problem."""
        # One-time listeners
        for listener in self._listener_futures:
            if asyncio.isfuture(listener) and not listener.done():
                listener.set_result(event)
        self._listener_futures = []
        # Persistent listeners
        for listener in self._listeners:
            try:
                if iscoroutinefunction(listener):
                    task = asyncio.create_task(listener(event))
                    self._listener_tasks.add(task)
                    task.add_done_callback(self._listener_tasks.discard)
                else:
                    listener(event)
            except Exception as e:
                logger.error("Failed to notify data listener: %s", e)

    async def _polling_loop(self):
        """Background task to poll data from the power sensor."""
        try:
            sleep_time = self._poll_delay_ms / 1000 # convert to seconds
            last_poll = datetime.now()
            last_start = datetime.now()
            while True:
                if self._serial and self._serial.is_ready():
                    logger.debug("Automatic data poll (%d ms delay, %d ms waiting).", (datetime.now() - last_poll).total_seconds() * 1000, (last_poll - last_start).total_seconds() * 1000)
                    last_start = datetime.now()
                    self.request_data()
                    try:
                        await asyncio.wait_for(self.get_next_data(), timeout=POWER_METER_TIMEOUT)
                    except TimeoutError:
                        logger.warning("Data polling timed out.")
                        if self._reconnect:
                            logger.info("Attempting to reconnect to the power sensor.")
                            current_port = self._serial.port
                            # Prevent closing this task
                            my_task = self._polling_task
                            self._polling_task = None
                            # Close all
                            self.close()
                            # Restore task
                            self._polling_task = my_task
                            # Attempt to reopen connection
                            await self.open(current_port)
                    last_poll = datetime.now()
                await asyncio.sleep(sleep_time)
        except asyncio.CancelledError:
            logger.info("Polling loop cancelled")

    # Public API

    async def open(self, port: str):
        """Open the connection. ASYNC!"""
        if self._serial:
            logger.warning("Connection already opened")
            return
        # Create serial connection
        _, connection = await create_serial_connection(
            asyncio.get_event_loop(),
            PowerSensorSerialConnection,
            port,
            baudrate=POWER_METER_BAUDRATE,
            bytesize=POWER_METER_BYTESIZE,
            parity=POWER_METER_PARITY,
            stopbits=POWER_METER_STOPBITS,
            timeout=POWER_METER_TIMEOUT,
        )
        self._serial = connection
        self._decoder = PowerSensorProtocol(connection, self._handle_data, self._handle_error)
        self._decoder.start_processing()
        logger.info("Communication established on port %s", port)
        if self._poll and not self._polling_task:
            self._polling_task = asyncio.create_task(self._polling_loop())

    async def get_next_data(self) -> PowerConsumption | ConnectionProblem:
        """Get the next data when received. ASYNC! If you do not want to miss data messages, add a listener."""
        collector = asyncio.get_running_loop().create_future()
        self._listener_futures.append(collector)
        return await collector

    def get_current_data(self) -> PowerConsumption:
        """Get the latest received data. If you do not want to miss data messages, add a listener."""
        return self.current_data

    def add_data_listener(self, listener: Callable[[PowerConsumption | ConnectionProblem], None]):
        """Add a listener for data updates. Callback can be sync or async function. The pace of data updates is determined by control command responses and heartbeats. If you want to increase data updates send additional controls/heartbeats or switch to manual heartbeat mode."""
        self._listeners.append(listener)

    def get_problems(self) -> list[ConnectionProblem]:
        """Get the list of connection problems."""
        return self._problems[:]

    def clear_problems(self):
        """Clear the list of connection problems."""
        self._problems.clear()

    def request_data(self):
        """Request data from the power sensor. This is a synchronous call that will return immediately. The data will be delivered to listeners or the next get_next_data() call."""
        if self.is_ready():
            if self._pending_request:
                logger.warning("Data request already pending, skipping new request")
            else:
                self._pending_request = True
                self._decoder.send_request()
        else:
            raise Exception("Cannot request data: serial connection not yet open")

    def is_ready(self) -> bool:
        """Check if the connection is ready."""
        if self._serial:
            return self._serial.is_ready()
        return False

    def close(self):
        """Close the connection."""
        if self._serial:
            self._serial.close()
            self._serial = None
        if self._decoder:
            self._decoder.shutdown()
            self._decoder = None
        if self._polling_task:
            self._polling_task.cancel()
            self._polling_task = None
        self._pending_request = False
