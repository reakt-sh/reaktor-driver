"""
User-facing control center bundling all capabilities and sensors of the REAKTOR.
"""
import logging
import aiofiles
from json import dumps
from os import makedirs
from os.path import exists, isdir, isfile, join
from datetime import datetime
from aiofiles.threadpool.text import AsyncTextIOWrapper

from connector.power.connection import PowerSensorConnection
from .util.device_helper import get_serial_ports, SerialPorts
from .motor.connection import MotorConnection
from .errors import ConnectionProblem
from .motor.data import Status
from .util.config_logging import activate_file_logging
from . import CONNECTOR_ROOT_LOGGER

logger = CONNECTOR_ROOT_LOGGER.getChild("center")

class ControlCenter:
    """Central control bundling all capabilities and sensors of the REAKTOR."""
    _session: str # Unique session identifier
    _storage_folder: str # Storage folder for data
    _latest_data: dict # Latest data snapshot
    _data_storage: AsyncTextIOWrapper # File handle for data storage
    _motor_connection: MotorConnection # Connection to the REAKTOR
    _power_connection: PowerSensorConnection # Connection to the power sensor
    _ports: SerialPorts # Serial ports for motor and power sensor

    def __init__(self, storage_dir: str = ".", log_level: int = logging.WARNING, power_sensor_poll_delay_ms: int = 0):
        logger.debug("New ControlCenter instance created.")
        self._ports = None
        # Motor Connection
        self._motor_connection = MotorConnection()
        self._motor_connection.add_data_listener(self._handle_status)
        # Power Meter Connection
        self._power_connection = PowerSensorConnection(poll_delay_ms=power_sensor_poll_delay_ms)
        # Storage
        self._session = None
        self._setup_storage(storage_dir, log_level)
        self._latest_data = None

    def _setup_storage(self, storage_dir: str, log_level: int):
        """Setup storage directory and session identifier."""
        # ID
        self._session = datetime.now().strftime("%Y%m%d_%H%M%S")
        # Directory structure
        if isdir(storage_dir):
            self._storage_folder = storage_dir
        elif not exists(storage_dir):
            self._storage_folder = storage_dir
            makedirs(self._storage_folder)
        elif isfile(storage_dir):
            raise ValueError(f"Storage directory '{storage_dir}' is a file not a directory.")
        else:
            raise ValueError(f"Storage directory '{storage_dir}' is not a valid directory.")
        # Logging
        # TODO Also make async file access?
        activate_file_logging(join(self._storage_folder, f"{self._session}.log"), level=log_level)

    async def _handle_status(self, status: Status | ConnectionProblem):
        """Handle status."""
        if isinstance(status, Status):
            power_data = self._power_connection.get_current_data()
            if power_data and self._latest_data and power_data == self._latest_data.get("power"):
                power_data = None  # No change in power data, avoid redundant storage
            self._latest_data = {
                "timestamp": datetime.now().isoformat(),
                "status": status.__dict__,
                "power": power_data.__dict__ if power_data else None,
            }
            data_str = dumps(self._latest_data, indent=None, separators=(",",":"), default=str)
            await self._data_storage.write(data_str + ",\n")
            await self._data_storage.flush()
        # else: Errors are already in log

    async def start(self):
        """Start the observation system."""
        logger.debug("Starting.")
        # Init data storage
        self._data_storage = await aiofiles.open(join(self._storage_folder, f"{self._session}.json"), "w", encoding="utf-8")
        await self._data_storage.write("[\n")  # Start of JSON array
        # Devices
        self._ports = await get_serial_ports()
        # Open connection to driver
        logger.debug("Opening connection to REAKTOR motor driver on port %s", self._ports.motor)
        await self._motor_connection.open(self._ports.motor)
        # Open connection to power meter
        logger.debug("Opening connection to REAKTOR power meter on port %s", self._ports.power)
        await self._power_connection.open(self._ports.power)

    async def reconnect(self):
        """Issue a reconnecto on all devices."""
        if not self._ports:
            logger.error("Cannot reconnect: Not started yet.")
            return
        logger.debug("Reconnecting.")
        # Close connections
        self._motor_connection.close()
        self._power_connection.close()
        # Reopen connections
        logger.debug("Reopening connection to motor driver on port %s", self._ports.motor)
        await self._motor_connection.open(self._ports.motor)
        logger.debug("Reopening connection to power meter on port %s", self._ports.power)
        await self._power_connection.open(self._ports.power)

    async def stop(self):
        """Stop the observation system."""
        logger.debug("Stopping.")
        self._motor_connection.close()
        self._power_connection.close()
        if self._data_storage:
            await self._data_storage.write("\n]")  # End of JSON array
            await self._data_storage.flush()
            await self._data_storage.close()

    def get_reaktor_connection(self) -> MotorConnection:
        """Get the active control connection."""
        return self._motor_connection

    def get_power_connection(self) -> PowerSensorConnection:
        """Get the active power sensor connection."""
        return self._power_connection
