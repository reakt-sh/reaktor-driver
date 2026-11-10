"""
User-facing connection for managing communication with the power meter.
"""

import asyncio
from datetime import datetime
from serial_asyncio import create_serial_connection

from .data import PowerConsumption
from .protocol import PowerSensorProtocol
from ..shared import DataProvider, SerialConnection
from ..generated.config_power_meter import POWER_METER_SERIAL_BAUDRATE, POWER_METER_SERIAL_BYTESIZE, POWER_METER_SERIAL_PARITY, POWER_METER_SERIAL_STOPBITS, POWER_METER_SERIAL_TIMEOUT
from . import POWER_ROOT_LOGGER

logger = POWER_ROOT_LOGGER.getChild("connection")

class PowerSensorConnection(DataProvider[PowerConsumption]):
    """Connection to the power sensor."""

    _serial: SerialConnection # Internal serial connection handler
    _decoder: PowerSensorProtocol # Message handler for en/decoding messages
    _current_port: str # Current serial port in use
    _polling_task: asyncio.Task # Background task for automatic data polling
    _pending_request: bool # Flag to indicate if a data request is pending
    _poll: bool # Flag to indicate if automatic polling is enabled
    _poll_delay_ms: int # Delay between automatic polls in milliseconds
    _reconnect: bool # Flag to indicate if automatic reconnection is enabled

    def __init__(self, automatic_polling: bool = True, poll_delay_ms: int = 0, automatic_reconnect: bool = True):
        super().__init__()
        self._serial = None
        self._decoder = None
        self._current_port = None
        self._polling_task = None
        self._pending_request = False
        self._poll = automatic_polling
        self._poll_delay_ms = poll_delay_ms
        self._reconnect = automatic_reconnect

    def _handle_data(self, data: PowerConsumption):
        self._pending_request = False
        super()._handle_data(data)

    # Internal API

    async def _polling_loop(self):
        """Background task to poll data from the power sensor."""
        try:
            sleep_time = self._poll_delay_ms / 1000 # convert to seconds
            last_poll = datetime.now()
            last_start = datetime.now()
            while True:
                if self.is_ready():
                    logger.debug("Automatic data poll (%d ms delay, %d ms waiting).", (datetime.now() - last_poll).total_seconds() * 1000, (last_poll - last_start).total_seconds() * 1000)
                    last_start = datetime.now()
                    self.request_data() # Send request to the power sensor
                    try:
                        await asyncio.wait_for(self.get_next_data(), timeout=POWER_METER_SERIAL_TIMEOUT)
                    except TimeoutError:
                        logger.warning("Data polling timed out.")
                        self._pending_request = False
                        if self._reconnect:
                            logger.info("Attempting to reconnect to the power sensor.")
                            # Prevent closing this task
                            my_task = self._polling_task
                            self._polling_task = None
                            # Close all
                            self.close()
                            # Restore task
                            self._polling_task = my_task
                            # Reopen connection
                            await self.open(self._current_port)
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
            SerialConnection,
            port,
            baudrate=POWER_METER_SERIAL_BAUDRATE,
            bytesize=POWER_METER_SERIAL_BYTESIZE,
            parity=POWER_METER_SERIAL_PARITY,
            stopbits=POWER_METER_SERIAL_STOPBITS,
            timeout=POWER_METER_SERIAL_TIMEOUT,
        )
        connection.complete_init(identifier=port, error_callback=self._handle_error)
        self._serial = connection
        self._decoder = PowerSensorProtocol(connection, self._handle_data, self._handle_error)
        self._decoder.start_processing()
        self._current_port = port
        logger.info("Connection created for port %s", port)
        if self._poll and not self._polling_task:
            self._polling_task = asyncio.create_task(self._polling_loop())

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
        if self._decoder:
            self._decoder.shutdown()
            self._decoder = None
        if self._serial:
            self._serial.close()
            self._serial = None
        if self._polling_task:
            self._polling_task.cancel()
            self._polling_task = None
        self._pending_request = False
