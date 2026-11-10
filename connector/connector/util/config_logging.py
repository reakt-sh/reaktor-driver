

import logging
from . import CONNECTOR_ROOT_LOGGER

def activate_file_logging(file_path: str, level: int = logging.WARNING):
    """Activate additional logging to a file. Note that control center will also save all logs."""
    file_handler = logging.FileHandler(file_path)
    file_handler.setFormatter(logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s"))
    CONNECTOR_ROOT_LOGGER.addHandler(file_handler)
    CONNECTOR_ROOT_LOGGER.setLevel(level)
