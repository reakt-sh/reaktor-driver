"""INTERNAL abstract based class for data providing connections."""

import asyncio
from typing import Callable, Generic, TypeVar

from ..errors import ConnectionProblem
from . import SHARED_ROOT_LOGGER

logger = SHARED_ROOT_LOGGER.getChild("data_provider")

T = TypeVar("T")

class DataProvider(Generic[T]):
    """Abstract base class for data providing connections."""
    _listeners: list[Callable[[T], None]] # Listeners for data updates
    _listener_futures: list[asyncio.Future] # Futures for one-time data listeners
    _listener_tasks: set # Background tasks for async listeners
    _problems: list[ConnectionProblem] # List of connection problems
    current_data: T # Latest received data

    # Internal API

    def __init__(self):
        self._listeners = []
        self._listener_futures = []
        self._listener_tasks = set()
        self._problems = []
        self.current_data = None

    def _handle_error(self, problem: ConnectionProblem):
        """Register a connection problem."""
        self._problems.append(problem)
        self._notify_listeners(problem)

    def _handle_data(self, data: T):
        """Store data and notify all registered listeners about new data."""
        self.current_data = data
        self._notify_listeners(data)

    def _notify_listeners(self, event: T | ConnectionProblem):
        """Notify all registered listeners about new data or a problem."""
        # One-time listeners
        for listener in self._listener_futures:
            if asyncio.isfuture(listener) and not listener.done():
                listener.set_result(event)
        self._listener_futures = []
        # Persistent listeners
        for listener in self._listeners:
            try:
                if asyncio.inspect.iscoroutinefunction(listener):
                    task = asyncio.create_task(listener(event))
                    self._listener_tasks.add(task)
                    task.add_done_callback(self._listener_tasks.discard)
                else:
                    listener(event)
            except Exception as e:
                logger.error("Failed to notify data listener: %s", e)

    # Public API

    async def get_next_data(self) -> T | ConnectionProblem:
        """Get the next data when received. ASYNC! If you do not want to miss data messages, add a listener."""
        collector = asyncio.get_running_loop().create_future()
        self._listener_futures.append(collector)
        return await collector

    def get_current_data(self) -> T:
        """Get the latest received data. If you do not want to miss data messages, add a listener."""
        return self.current_data

    def add_data_listener(self, listener: Callable[[T | ConnectionProblem], None]):
        """Add a listener for data updates. Callback can be sync or async function."""
        self._listeners.append(listener)

    def get_problems(self) -> list[ConnectionProblem]:
        """Get the list of connection problems."""
        return self._problems[:]

    def clear_problems(self):
        """Clear the list of connection problems."""
        self._problems.clear()
