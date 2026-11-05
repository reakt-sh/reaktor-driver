"""
User-facing connection for managing communication with the GNSS receiver.
"""

import asyncio
from datetime import datetime
from gps.aiogps import aiogps

from ..shared import DataProvider
from ..errors import ConnectionProblem
from .data import Position
from . import GNSS_ROOT_LOGGER

logger = GNSS_ROOT_LOGGER.getChild("connection")

class GNSSConnection(DataProvider[Position]):
    """Connection to the GNSS receiver."""

    _client: aiogps # Internal GNSS client
    _update_task: asyncio.Task # Background task for automatic data polling
    _problems: list[ConnectionProblem] # List of connection problems
    _pending_request: bool # Flag to indicate if a data request is pending
    _poll: bool # Flag to indicate if automatic polling is enabled
    _poll_delay_ms: int # Delay between automatic polls in milliseconds
    _reconnect: bool # Flag to indicate if automatic reconnection is enabled

    def __init__(self, automatic_reconnect: bool = True):
        super().__init__()
        self._client = aiogps(
            connection_args={
                "host": "127.0.0.1",
                "port": 2947,
            },
            connection_timeout=5,
            reconnect=2 if automatic_reconnect else 0,
            alive_opts={
                "rx_timeout": 5
            }
        )
        self._update_task = None

    # Internal API

    async def _update_loop(self):
        """Background task to update data from the GNSS receiver."""
        try:
            async with self._client as gpsd:
                while True:
                    try:
                        async for msg in gpsd:
                            logger.debug(f"Received: {msg}")
                            if msg.get("class") == "TPV":
                                position = Position(
                                    latitude=msg.get("lat"),
                                    longitude=msg.get("lon"),
                                    altitude=msg.get("alt"),
                                    speed=msg.get("speed"),
                                    # climb=msg.get("climb"),
                                    # track=msg.get("track"),
                                    # mode=msg.get("mode"),
                                    # time=datetime.fromisoformat(msg.get("time")) if msg.get("time") else None
                                )
                                self._handle_data(position)
                    except asyncio.IncompleteReadError:
                        logger.info("Connection closed by server")
                    except asyncio.TimeoutError:
                        logger.error("Timeout waiting for gpsd to respond")
                    except Exception as exc: # pylint: disable=W0703
                        logger.error(f"Error: {exc}")
                    # Try again in 1s
                    await asyncio.sleep(1)
        except asyncio.CancelledError:
            logger.info("Update loop cancelled")

    # Public API

    async def open(self):
        """Open the connection. ASYNC!"""
        if not self._update_task:
            self._update_task = asyncio.create_task(self._update_loop())

    def close(self):
        """Close the connection."""
        if self._update_task:
            self._update_task.cancel()
            self._update_task = None
