"""
User-facing control center bundling all capabilities and sensors of the REAKTOR.
"""
import logging
from json import dumps
from os import makedirs
from os.path import exists, isdir, isfile, join
from datetime import datetime
import aiofiles
from aiofiles.threadpool.text import AsyncTextIOWrapper

from .power.connection import PowerSensorConnection
from .power.data import PowerConsumption
from .util.device_helper import get_serial_ports, SerialPorts
from .util.live_data_server import LiveDataServer
from .motor.connection import MotorConnection
from .gnss import GNSSConnection, GNSSMode, Position
from .errors import ConnectionProblem
from .motor.data import InternalState, Status
from .util.config_logging import activate_file_logging
from . import CONNECTOR_ROOT_LOGGER

logger = CONNECTOR_ROOT_LOGGER.getChild("center")

class ControlCenter:
    """Central control bundling all capabilities and sensors of the REAKTOR."""
    _session: str # Unique session identifier
    _storage_folder: str # Storage folder for data
    _latest_power_data: dict # Latest power data snapshot
    _latest_gnss_data: dict # Latest GNSS data snapshot
    _data_storage: AsyncTextIOWrapper # File handle for data storage
    _motor_connection: MotorConnection # Connection to the REAKTOR
    _power_connection: PowerSensorConnection # Connection to the power sensor
    _gnss_connection: GNSSConnection # Connection to the GNSS
    _ports: SerialPorts # Serial ports for motor and power sensor
    _live_data: LiveDataServer # Web server for live data broadcasting

    def __init__(self, storage_dir: str = ".", log_level: int = logging.WARNING, power_sensor_poll_delay_ms: int = 0, live_data_port: int = 8080):
        logger.debug("New ControlCenter instance created.")
        self._ports = None
        # Motor Connection
        self._motor_connection = MotorConnection()
        self._motor_connection.add_data_listener(self._handle_status)
        # Power Meter Connection
        self._power_connection = PowerSensorConnection(poll_delay_ms=power_sensor_poll_delay_ms)
        # GNSS Connection
        self._gnss_connection = GNSSConnection()
        # Live Data Server
        self._live_data = LiveDataServer(port=live_data_port)
        # Storage
        self._session = None
        self._setup_storage(storage_dir, log_level)
        self._latest_power_data = None
        self._latest_gnss_data = None

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

    def _serialize_data(self, obj):
        """Custom serializer for non-serializable objects."""
        if isinstance(obj, datetime):
            return obj.isoformat()
        if isinstance(obj, GNSSMode):
            return obj.value
        if isinstance(obj, (Status, InternalState, PowerConsumption, Position)):
            return obj.__dict__
        return str(obj)

    async def _handle_status(self, status: Status | ConnectionProblem):
        """Handle status."""
        if isinstance(status, Status):
            # Power
            power_data = self._power_connection.get_current_data()
            if power_data and self._latest_power_data and power_data == self._latest_power_data:
                power_data = None  # No change in power data, avoid redundant storage
            else:
                self._latest_power_data = power_data
            # GNSS
            gnss_data = self._gnss_connection.get_current_data()
            if gnss_data and self._latest_gnss_data and gnss_data == self._latest_gnss_data:
                gnss_data = None  # No change in GNSS data, avoid redundant storage
            else:
                self._latest_gnss_data = gnss_data
            # Combined data
            data = {
                "timestamp": datetime.now().isoformat(),
                "status": status.__dict__,
                "power": power_data.__dict__ if power_data else None,
                "gnss": gnss_data.__dict__ if gnss_data else None
            }
            data_str = dumps(data, indent=None, separators=(",",":"), default=self._serialize_data)
            await self._data_storage.write(data_str + "\n")
            await self._data_storage.flush()
            # Broadcast to live data server if running
            if self._live_data.is_running():
                await self._live_data.broadcast(data_str)
        # else: Errors are already in log

    async def start(self, live_data: bool = False):
        """Start the observation system."""
        logger.debug("Starting.")
        # Init data storage
        self._data_storage = await aiofiles.open(join(self._storage_folder, f"{self._session}.jsonl"), "w", encoding="utf-8")
        # Devices
        self._ports = await get_serial_ports()
        # Open connection to driver
        logger.debug("Opening connection to REAKTOR motor driver on port %s", self._ports.motor)
        await self._motor_connection.open(self._ports.motor)
        # Open connection to power meter
        logger.debug("Opening connection to REAKTOR power meter on port %s", self._ports.power)
        await self._power_connection.open(self._ports.power)
        # Open connection to GNSS
        logger.debug("Opening connection to GNSS receiver.")
        await self._gnss_connection.open()
        # Start live data server
        if live_data:
            logger.debug("Starting live data server on port %d.", self._live_data.port)
            self._live_data.start()

    async def reconnect(self):
        """Issue a reconnecto on all devices."""
        if not self._ports:
            logger.error("Cannot reconnect: Not started yet.")
            return
        logger.debug("Reconnecting.")
        # Close connections
        self._motor_connection.close()
        self._power_connection.close()
        self._gnss_connection.close()
        # Reopen connections
        logger.debug("Reopening connection to motor driver on port %s", self._ports.motor)
        await self._motor_connection.open(self._ports.motor)
        logger.debug("Reopening connection to power meter on port %s", self._ports.power)
        await self._power_connection.open(self._ports.power)
        logger.debug("Reopening connection to GNSS receiver.")
        await self._gnss_connection.open()

    async def stop(self):
        """Stop the observation system."""
        logger.debug("Stopping.")
        self._motor_connection.close()
        self._power_connection.close()
        self._gnss_connection.close()
        if self._live_data.is_running():
            self._live_data.stop()
        if self._data_storage:
            await self._data_storage.flush()
            await self._data_storage.close()

    def get_reaktor_connection(self) -> MotorConnection:
        """Get the active control connection."""
        return self._motor_connection

    def get_power_connection(self) -> PowerSensorConnection:
        """Get the active power sensor connection."""
        return self._power_connection

    def get_gnss_connection(self) -> GNSSConnection:
        """Get the active GNSS connection."""
        return self._gnss_connection
