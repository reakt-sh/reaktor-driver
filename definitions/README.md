# REAKTOR Definitions and Configuration

Definitions and configuration for various aspects of the driver code. Used as cross-platform source.

- Binary protocol definitions for communication with the driver.
- Hardware capabilities and configuration options.
- Behavioral definitions for the operation.

Generated code will be placed in the `generated` directories of the adjacent component directories.

## Developer Setup

1. `python3 -m venv .venv`
1. `source .venv/bin/activate`
1. `python -m pip install --upgrade pip`
1. `python -m pip install -r requirements.txt`

## Code generation

1. `source .venv/bin/activate`
1. `python generate.py`

