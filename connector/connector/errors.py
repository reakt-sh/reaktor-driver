"""
Error types.
"""

from datetime import datetime

class ConnectionProblem(Exception):
    """Represents a problem with a connection."""
    cause: Exception | None # Underlying cause of the problem, if any
    time: datetime # Timestamp when the problem occurred

    """Base class for connection problems."""
    def __init__(self, message, cause=None):
        super().__init__(message)
        self.message = message
        self.cause = cause
        self.time = datetime.now()

    def __str__(self):
        if self.cause:
            return f"{self.message} (Caused by: {self.cause})"
        return f"{self.message}"
