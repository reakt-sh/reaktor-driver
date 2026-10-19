"""
Error types.
"""

class ConnectionProblem(Exception):
    """Base class for connection problems."""
    def __init__(self, message, cause=None):
        super().__init__(message)
        self.message = message
        self.cause = cause

    def __str__(self):
        if self.cause:
            return f"{self.message} (Caused by: {self.cause})"
        else:
            return f"{self.message}"
