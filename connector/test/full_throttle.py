"""
A test to observe continuous full speed behavior.
"""

import asyncio
import logging
from test_context import ControlCenter, Control, Mode

async def main():
    cc = ControlCenter(storage_dir="./local", log_level=logging.DEBUG)
    connection = cc.get_reaktor_connection()
    await cc.start()

    # Testing
    try:
        while not connection.is_ready():
            await asyncio.sleep(.1)
        # Set full throttle
        control = Control(
            mode = Mode.FORWARD,
            target_speed = 20
        )
        connection.send_control(control)
        while True:
            # Forever
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

        connection.close()
        print("Connection closed.")

if __name__ == "__main__":
    asyncio.run(main())
