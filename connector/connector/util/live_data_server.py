"""A web server to publish live data via WebSocket protocol."""

import asyncio
from aiohttp import web
from . import UTIL_ROOT_LOGGER

logger = UTIL_ROOT_LOGGER.getChild("data-server")

class LiveDataServer():
    """A web server to publish live data via WebSocket protocol."""
    port: int # Port for the web server
    _app: web.Application # Web application
    _server_task: asyncio.Task # Background task for automatic data polling
    _connections: list[web.WebSocketResponse] # List of active WebSocket connections
    _running: bool # Flag to indicate if the server is running

    def __init__(self, port: int = 8080):
        """Initialize the live data server."""
        self.port = port
        self._app = web.Application()
        self._app.add_routes([web.get("/data", self._websocket_handler)])
        self._server_task = None
        self._connections = []
        self._running = False

    async def _websocket_handler(self, request):
        """Handle WebSocket connections."""
        ws = web.WebSocketResponse()
        await ws.prepare(request)
        self._connections.append(ws)

        async for msg in ws:
            if msg.type == web.WSMsgType.TEXT:
                if msg.data == "close":
                    logger.info("WebSocket connection closed by client.")
                    await ws.close()
                # Just ignoring any other text messages
            elif msg.type == web.WSMsgType.CLOSE:
                logger.info("WebSocket connection closed.")
            elif msg.type == web.WSMsgType.ERROR:
                logger.error("WebSocket connection closed with exception %s.", ws.exception())

        logger.info("WebSocket connection closed")
        self._connections.remove(ws)
        return ws

    def start(self):
        """Start the live data server."""
        logger.info("Starting live data server.")
        if self._running:
            logger.warning("Live data server is already running.")
            return
        self._server_task = asyncio.create_task(web._run_app( # pylint: disable=W0212
            self._app,
            host="0.0.0.0",
            port=self.port,
        ))
        self._running = True

    def is_running(self) -> bool:
        """Check if the server is running."""
        return self._running

    def stop(self):
        """Stop the live data server."""
        self._server_task.cancel()
        self._running = False
        logger.info("Stopping live data server.")

    async def broadcast(self, msg: str):
        """Broadcast data to all connected WebSocket clients."""
        logger.info("Broadcasting message to %d clients.", len(self._connections))
        for ws in self._connections:
            if not ws.closed:
                await ws.send_str(msg)
