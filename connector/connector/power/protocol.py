"""
INTERNAL message handler for decoding and encoding messages to/from the power sensor.
"""

import asyncio
from datetime import datetime
from typing import Callable
from ..errors import ConnectionProblem
from .data import PowerConsumption
from ..shared import SerialConnection
from ..generated.config_power_meter import POWER_METER_PROTOCOL_MESSAGE_BYTES
from . import POWER_ROOT_LOGGER

logger = POWER_ROOT_LOGGER.getChild("internal.messages")

class PowerSensorProtocol:
    """Handler for encoding and decoding messages to/from the power sensor."""

    _serial: SerialConnection # Internal serial connection handler
    _data_callback: Callable[[PowerConsumption], None] # Callback for new status
    _error_callback: Callable[[ConnectionProblem], None] # Callback for errors
    _worker: asyncio.Task # Background worker task for processing messages
    _acknowledge_code: int # Acknowledgment code for the request (aka slave ID)

    def __init__(self, serial: SerialConnection, data_callback: Callable[[PowerConsumption], None], error_callback: Callable[[ConnectionProblem], None]):
        self._serial = serial
        self._data_callback = data_callback
        self._error_callback = error_callback
        self._worker = None
        self._acknowledge_code = 1

    async def _decoder_loop(self) -> None:
        """Background task to decode incoming data."""
        try:
            while True:
                # Read status message bytes form serial
                input_data = await self._serial.consume(POWER_METER_PROTOCOL_MESSAGE_BYTES)
                logger.debug("Received power consumption data: %s", input_data.hex(" "))
                # Validate
                if len(input_data) < POWER_METER_PROTOCOL_MESSAGE_BYTES:
                    logger.warning("Response too short: %d bytes", len(input_data))
                    continue
                # Decode data
                received_crc = input_data[-2:]
                calculated_crc = self._modbus_crc(input_data[:-2])
                # Check CRC
                if received_crc != calculated_crc:
                    logger.warning("CRC mismatch: got %s, expected %s", received_crc.hex(' '), calculated_crc.hex(' '))
                    continue
                # Validate response
                if input_data[0] != self._acknowledge_code:
                    logger.warning("Wrong acknowlage code: %s", input_data[0])
                    continue
                if input_data[1] & 0x80:
                    logger.warning("Modbus exception: %s", input_data.hex(' '))
                    continue
                if input_data[1] != 0x04:
                    logger.warning("Unexpected function code: %02x", input_data[1])
                    continue
                # Decode data
                byte_count = input_data[2]
                if byte_count != 16:
                    logger.warning("Unexpected byte count: %d", byte_count)
                    continue
                data = input_data[3:3 + byte_count]
                registers = []
                for i in range(0, len(data), 2):
                    registers.append(int.from_bytes(data[i:i + 2], byteorder="big"))
                # Convert data
                voltage_v = registers[0] / 100.0
                current_a = registers[1] / 100.0
                # PZEM-017: 32-bit values are transmitted as low word first, then high word.
                power_raw = (registers[3] << 16) | registers[2]
                energy_raw = (registers[5] << 16) | registers[4]
                power_w = power_raw / 10.0
                energy_wh = energy_raw
                high_voltage_alarm = registers[6] == 0xFFFF
                low_voltage_alarm = registers[7] == 0xFFFF

                # Create record and validate via pydantic
                new_data = PowerConsumption(
                    time=datetime.now(),
                    voltage=voltage_v,
                    current=current_a,
                    power=power_w,
                    energy=energy_wh,
                    high_voltage_alarm=high_voltage_alarm,
                    low_voltage_alarm=low_voltage_alarm,
                )
                logger.info("Power consumption data received: %s", new_data)
                # Notify listener about new data
                self._data_callback(new_data)
        except asyncio.CancelledError:
            logger.info("Decoder loop stopped")

    def _modbus_crc(self, data: bytes) -> bytes:
        """Calculate the Modbus CRC16 for the given data."""
        crc = 0xFFFF
        for byte in data:
            crc ^= byte
            for _ in range(8):
                if crc & 0x0001:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
        return crc.to_bytes(2, byteorder="little")

    def _encode_request(self, slave_id: int) -> bytes:
        """Encode a request to read data from the power sensor."""
        # Read 8 input registers from address 0x0000
        payload = bytes([
            slave_id,
            0x04,       # Read Input Registers
            0x00, 0x00, # Start register
            0x00, 0x08, # Number of registers
        ])
        return payload + self._modbus_crc(payload)

    def send_request(self) -> bool:
        """Send a data request to the sensor."""
        if not self._serial.is_ready():
            logger.error("Cannot send data request: serial connection not yet ready")
            return False
        # Encode and send request
        msg = self._encode_request(self._acknowledge_code)
        logger.info("Sending data request: %s", msg.hex(" "))
        self._serial.send(msg)
        return True

    def start_processing(self) -> bool:
        """Start decoding incoming data."""
        if not self._worker:
            self._worker = asyncio.create_task(self._decoder_loop())
            return True
        else:
            logger.warning("Decoder loop already running")
            return False

    def shutdown(self):
        """Stop decoding incoming messages."""
        if self._worker:
            self._worker.cancel()
            self._worker = None
