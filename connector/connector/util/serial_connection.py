""" INTERNAL asynchronous serial connection."""
import asyncio
from turtle import st
from typing import Callable
from serial_asyncio import SerialTransport

from ..errors import ConnectionProblem
from . import UTIL_ROOT_LOGGER

logger = UTIL_ROOT_LOGGER.getChild("internal.serial")

class SerialConnection(asyncio.Protocol):
    """Internal asynchronous serial connection."""
    _transport: SerialTransport # Low-level serial transport handler
    _buffer: bytearray # Buffer queue for incoming data
    _consumer: asyncio.Future # Future for waiting for data
    _shutdown: bool # Flag to indicate if the connection is shutting down
    _id: str # Identifier for the connection (for logging purposes)
    _error_callback: Callable[[ConnectionProblem], None] # Callback for errors

    # Internal API

    def __init__(self):
        super().__init__()
        self._transport = None
        self._buffer = bytearray()
        self._consumer = None
        self._shutdown = False
        self._id = "unknown"

    def connection_made(self, transport):
        self._transport = transport
        logger.info("[%s] Serial connection established", self._id)

    def connection_lost(self, exc):
        if self._shutdown:
            logger.info("[%s] Serial connection closed", self._id)
        else:
            logger.error("[%s] Serial connection lost (Exception: %s)", self._id, exc)
            if self._consumer and not self._consumer.done():
                self._consumer.set_exception(ConnectionError("Serial connection lost"))
            if self._error_callback:
                self._error_callback(ConnectionProblem("Serial connection lost", cause=exc))

    def data_received(self, data):
        logger.debug("[%s] Serial data received: %s", self._id, data.hex(" "))
        self._buffer.extend(data)
        if self._consumer and not self._consumer.done():
            self._consumer.set_result(True)

    # Pubic API

    def complete_init(self, identifier: str, error_callback: Callable[[ConnectionProblem], None]):
        """Complete the initialization."""
        self._id = identifier
        self._error_callback = error_callback

    async def consume(self, num_bytes: int) -> bytes:
        """Consume a number of bytes from the buffer."""
        # Wait for data if needed
        while len(self._buffer) < num_bytes:
            if self._consumer and not self._consumer.done():
                logger.error("[%s] Unsupported Operation: Cannot have multiple consumers waiting for data on the same serial connection.", self._id)
            self._consumer = asyncio.get_event_loop().create_future()
            await self._consumer
        # Retrieve data from buffer
        data = self._buffer[:num_bytes]
        self._buffer = self._buffer[num_bytes:]
        return bytes(data)

    def clear_input_buffer(self):
        """Clear the input buffer."""
        self._buffer.clear()
        self._transport.serial.reset_input_buffer()
        logger.debug("Serial input buffer cleared")

    def send(self, data: bytes):
        """Send a data."""
        if not self._transport:
            logger.error("[%s] Cannot send data: serial connection not yet established", self._id)
            return
        logger.debug("[%s] Sending data: %s", self._id, data.hex(" "))
        self._transport.write(data)

    def is_ready(self) -> bool:
        """Check if the serial connection is ready."""
        return self._transport is not None

    def close(self):
        """Close the serial connection."""
        if self._transport:
            logger.debug("[%s] Closing serial connection", self._id)
            self._shutdown = True
            self._transport.close()
