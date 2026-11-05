"""
User-facing connection for managing communication with the motor controller.
"""

import asyncio
from warnings import deprecated
from datetime import datetime
from typing import Callable
from serial_asyncio import create_serial_connection

from ..generated.config_communication import COMM_SERIAL_BAUDRATE, COMM_CONTROL_MESSAGE_HEARTBEAT_TIME
from ..errors import ConnectionProblem
from ..shared import SerialConnection, DataProvider
from .protocol import MotorProtocol
from .data import Control, Status
from . import MOTOR_ROOT_LOGGER

logger = MOTOR_ROOT_LOGGER.getChild("connection")

class MotorConnection(DataProvider[Status]):
    """Connection to the motor controller."""

    _serial: SerialConnection # Internal serial connection handler
    _decoder: MotorProtocol # Message handler for en/decoding messages
    _heartbeat_task: asyncio.Task # Background task for automatic heartbeat
    _opened: bool # Connection established
    _problems: list[ConnectionProblem] # List of connection problems

    def __init__(self):
        super().__init__()
        self._serial = None
        self._decoder = None
        self._heartbeat_task = None
        self._opened = False
        self._problems = []

    # Internal API

    async def _heartbeat_loop(self):
        """Background task to send periodic heartbeat messages."""
        try:
            sleep_time = (COMM_CONTROL_MESSAGE_HEARTBEAT_TIME / 1000) / 2 # Half heartbeat interval
            last_msg_time = datetime.now()
            while True:
                if self._serial and self._serial.is_ready():
                    logger.debug("Automatic heartbeat (%d ms delay).", (datetime.now() - last_msg_time).total_seconds() * 1000)
                    self._decoder.send_heartbeat()
                    last_msg_time = datetime.now()
                await asyncio.sleep(sleep_time)
        except asyncio.CancelledError:
            logger.info("Heartbeat loop cancelled")

    # Public API

    async def open(self, port: str, manual_heartbeat: bool = False):
        """Open the connection. ASYNC!"""
        if self._serial:
            logger.warning("Connection already opened")
            return
        # Create serial connection
        _, connection = await create_serial_connection(
            asyncio.get_event_loop(),
            SerialConnection,
            port,
            baudrate=COMM_SERIAL_BAUDRATE
        )
        connection.complete_init(identifier=port, error_callback=self._handle_error)
        self._serial = connection
        self._decoder = MotorProtocol(connection, self._handle_status, self._handle_error)
        # Wait until the connection is fully established (driver indicates connected)
        if await self._decoder.start_processing():
            self._opened = True
            logger.info("Communication established on port %s", port)
            # Start automatic heartbeat to detect connection loss
            if not manual_heartbeat:
                self._heartbeat_task = asyncio.create_task(self._heartbeat_loop())
        else:
            logger.error("Failed to establish communication on port %s", port)
            raise ConnectionProblem("Failed to establish communication")

    @deprecated("Use add_data_listener instead")
    def add_status_listener(self, listener: Callable[[Status | ConnectionProblem], None]):
        """Add a listener for data updates. Callback can be sync or async function. The pace of data updates is determined by control command responses and heartbeats. If you want to increase data updates send additional controls/heartbeats or switch to manual heartbeat mode."""
        super().add_data_listener(listener)

    def send_control(self, control: Control) -> bool:
        """Send a control command."""
        return self._decoder.send_control(control)

    def send_heartbeat(self) -> bool:
        """Send a manual heartbeat signal. Only required if automatic heartbeat is disabled."""
        return self._decoder.send_heartbeat()

    def is_ready(self) -> bool:
        """Check if the connection is ready."""
        if open and self._serial:
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
        if self._heartbeat_task:
            self._heartbeat_task.cancel()
            self._heartbeat_task = None

