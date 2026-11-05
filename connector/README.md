# REAKTOR Driver Connector

The Python module that handles the serial connections to the Arduino driver board and the power meter.
Public entry point is the `ControlCenter` class, which handles all connections and provides data logging.

## Developer Setup

1. `python3.13 -m venv .venv`
1. `source .venv/bin/activate`
1. `python -m pip install --upgrade pip`
1. `python -m pip install -r requirements.txt`
