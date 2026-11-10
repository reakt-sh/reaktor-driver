"""
A simple startup script for the observation system.
"""

import asyncio
import logging
from test_context import ControlCenter, Control, Mode

async def main():
    """Main function to run the test."""
    cc = ControlCenter(storage_dir="./local", log_level=logging.DEBUG)
    connection = cc.get_reaktor_connection()
    await cc.start(live_data=True)  # Start with live data server enabled

    # Testing
    try:
        flip = False
        while True:
            if connection.is_ready():
                # Set direction
                control = Control(
                    mode = Mode.REVERSE if flip else Mode.FORWARD,
                    target_speed = 0
                )
                flip = not flip
                connection.send_control(control)
            await asyncio.sleep(1)
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        print("Stopping motor...")
        if connection.is_ready():
            control = Control(
                mode = Mode.NEUTRAL,
                target_speed = 0
            )
            connection.send_control(control)
            await asyncio.sleep(1)
        else:
            print("Connection lost, cannot stop motor.")
        await cc.stop()

if __name__ == "__main__":
    asyncio.run(main())
