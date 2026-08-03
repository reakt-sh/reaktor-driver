#include <Arduino.h>
// Generated code library
#include <config_hardware.h>
#include <config_vehicle.h>

#include "panel.h"

#include "debug.h"
#include "mode.h"
#include "switches.h"
#include "speed.h"
#include "error.h"

// Constants
const uint8_t DISPLAY_ROWS = 2;
const uint8_t DISPLAY_COLUMNS = 16;
const uint8_t DISPLAY_ROW_TEXT_OFFSET = 0x40;
const uint8_t DISPLAY_CMD_INIT_ROWS = 0x3B; // 2-row display type
const uint8_t DISPLAY_CMD_INIT_TEXT_MODE = 0x17; // character mode
const uint8_t DISPLAY_CMD_INIT_CONTROL_MODE = 0x0C; // Bit code (last 4 bits): display-control-cmd-bit (always 1) + display on + cursor off + blinking off
const uint8_t DISPLAY_CMD_INIT_ENTRY_MODE = 0x06; // Bit code (last 3 bits): entry-mode-cmd-bit (always 1) + entry-left on + unused (always 0)
const uint8_t DISPLAY_CMD_CLEAR = 0x01;
const uint8_t DISPLAY_CMD_RETURNHOME = 0x02;
const uint8_t DISPLAY_CMD_SET_PREFIX = 0x80;
const char* DISPLAY_INIT_ROW_0 = "NUT     0,0 km/h";
const char* DISPLAY_INIT_ROW_1 = "SYS ->  0,0 km/h";
const float RPM_TO_KMH = MOTOR_SPEED_TRANSMISSION_FACTOR * 3.6;

// Variables
enum {
    DISPLAY_STATE_CONTROL_ERROR,
    DISPLAY_STATE_CONTROL_MANUAL,
    DISPLAY_STATE_CONTROL_REMOTE,
    DISPLAY_STATE_CONTROL_SYSTEM
} displayStateControlSource = DISPLAY_STATE_CONTROL_SYSTEM;
tDriveMode displayStateDriveMode = NEUTRAL;
int displayStateTargetSpeed = 0;
int displayStateCurrentSpeed = 0;
int displayLastUpdate = 0;

// ====================
// API

bool setupPanel() {
    // Configure digital IO pins
    pinMode(BOARD_PIN_IN_PANEL_KNOB_PRESSED, INPUT_PULLUP);
    // TODO Also handle turn events on the knob

    pinMode(BOARD_PIN_OUT_PANEL_LED_RED, OUTPUT);
    pinMode(BOARD_PIN_OUT_PANEL_LED_GREEN, OUTPUT);
    pinMode(BOARD_PIN_OUT_PANEL_LED_BLUE, OUTPUT);

    pinMode(BOARD_PIN_OUT_PANEL_DISPLAY_SDO, OUTPUT);
    pinMode(BOARD_PIN_OUT_PANEL_DISPLAY_SDI, OUTPUT);
    pinMode(BOARD_PIN_OUT_PANEL_DISPLAY_SCL, OUTPUT);
    pinMode(BOARD_PIN_OUT_PANEL_DISPLAY_CS, OUTPUT);

    // Set initial LED output states
    digitalWrite(BOARD_PIN_OUT_PANEL_LED_RED, LOW);
    digitalWrite(BOARD_PIN_OUT_PANEL_LED_GREEN, LOW);
    digitalWrite(BOARD_PIN_OUT_PANEL_LED_BLUE, LOW);

    // Setup display connection
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SCL, HIGH); // Protocol requires clock to be high on start
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_CS, HIGH); // Protocol requires CS to be high on start
    sendByteToDisplay(DISPLAY_CMD_INIT_ROWS, true);
    delay(2);
    sendByteToDisplay(DISPLAY_CMD_INIT_TEXT_MODE, true);
    delay(2);
    sendByteToDisplay(DISPLAY_CMD_INIT_CONTROL_MODE, true);
    delay(2);
    sendByteToDisplay(DISPLAY_CMD_CLEAR, true);
    delay(2);
    sendByteToDisplay(DISPLAY_CMD_RETURNHOME, true);
    delay(2);
    sendByteToDisplay(DISPLAY_CMD_INIT_ENTRY_MODE, true);
    delay(2);
    setDisplayText(0, 0, DISPLAY_INIT_ROW_0, DISPLAY_COLUMNS);
    setDisplayText(1, 0, DISPLAY_INIT_ROW_1, DISPLAY_COLUMNS);

    return true;
}

void updatePanel() {
    tDriveMode currentMode = getCurrentMode();

    // Update the LED on the panel based on the current mode
    switch (currentMode) {
    case EMERGENCY_STOP:
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_RED, HIGH);
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_GREEN, LOW);
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_BLUE, LOW);
        break;
    case NEUTRAL:
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_RED, LOW);
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_GREEN, LOW);
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_BLUE, HIGH);
        break;
    default: // all other modes
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_RED, LOW);
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_GREEN, HIGH);
        digitalWrite(BOARD_PIN_OUT_PANEL_LED_BLUE, LOW);
        break;
    }

    // Update Display
    bool forceSpeedUpdate = false;
    // Mode
    if (currentMode != displayStateDriveMode) {
        displayStateDriveMode = currentMode;
        forceSpeedUpdate = true;
        switch (currentMode) {
        case EMERGENCY_STOP:
            setDisplayText(0, 0, "ERR", 3);
            if (hasError()) {
                char errorTextBuffer[3];
                uint64_t errors = getErrors();
                int code = 0;
                for (code = 0; code < 64; code++) {
                    if (errors & (1ULL << code)) {
                        break;
                    }
                }
                snprintf(errorTextBuffer, 3, "%02d", code);

                if (errors > (1ULL << code)) {
                    // More than one error, indicate with a plus sign
                    errorTextBuffer[2] = '+';
                } else {
                    // Override null terminator by empty space
                    errorTextBuffer[2] = ' ';
                }
                setDisplayText(1, 0, errorTextBuffer, 3);
            }
            break;
        case DRIVE_FORWARD:
            setDisplayText(0, 0, "FWD", 3);
            break;
        case DRIVE_REVERSE:
            setDisplayText(0, 0, "REV", 3);
            break;
        case PARKING:
            setDisplayText(0, 0, "PRK", 3);
            break;
        case NEUTRAL:
            setDisplayText(0, 0, "NEU", 3);
            break;
        }
    }
    // Control source
    bool remoteControl = isRemoteControlSwitchEngaged(); // FIXME This should come from modes not hardware
    if (currentMode != EMERGENCY_STOP && ((remoteControl && displayStateControlSource != DISPLAY_STATE_CONTROL_REMOTE) || (!remoteControl && displayStateControlSource != DISPLAY_STATE_CONTROL_MANUAL))) {
        if (remoteControl) {
            setDisplayText(1, 0, "RC ", 3);
            displayStateControlSource = DISPLAY_STATE_CONTROL_REMOTE;
        } else {
            setDisplayText(1, 0, "MAN", 3);
            displayStateControlSource = DISPLAY_STATE_CONTROL_MANUAL;
        }
    }

    // Current speed
    char speedTextBuffer[4];
    bool speedRefresh = false;
    if (forceSpeedUpdate || millis() - displayLastUpdate >= DRIVER_PANEL_DISPLAY_SPEED_REFRESH_DELAY_MS) {
        speedRefresh = true;
        displayLastUpdate = millis();
        int actualCurrentRPM = getCurrentRPM();
        if (actualCurrentRPM != displayStateCurrentSpeed) {
            displayStateCurrentSpeed = actualCurrentRPM;
            formatDisplaySpeed(speedTextBuffer, actualCurrentRPM);
            setDisplayText(0, 7, speedTextBuffer, 4);
        }
    }
    // Target speed
    int actualTargetRPM = getTargetRPM();
    if (actualTargetRPM != displayStateTargetSpeed && (remoteControl || (!remoteControl && speedRefresh))) {
        displayStateTargetSpeed = actualTargetRPM;
        formatDisplaySpeed(speedTextBuffer, actualTargetRPM);
        setDisplayText(1, 7, speedTextBuffer, 4);
    }
}

// ====================
// INTERNAL

/**
 * Sets the text for the display.
 *
 * @param row The row number.
 * @param offset The starting column offset.
 * @param text The text to display.
 * @param length The length of the text.
 */
void setDisplayText(uint8_t row, uint8_t offset, const char* text, size_t length) {
    // Check if the row and offset are within valid bounds
    if (row >= 0 && row < DISPLAY_ROWS && offset >= 0 && offset < DISPLAY_COLUMNS) {
        uint8_t rowOffset = row * DISPLAY_ROW_TEXT_OFFSET;
        for (uint8_t idx = 0; idx < length && (offset + idx) < DISPLAY_COLUMNS; idx++) {
            // Send each character to the display
            uint8_t addr = rowOffset + offset + idx;
            sendByteToDisplay(DISPLAY_CMD_SET_PREFIX | addr, true);
            sendByteToDisplay((uint8_t)text[idx], false);
        }
    }
}

/**
 * Sends a byte to the display.
 *
 * @param value The byte value to send.
 * @param cmd If true, the byte is treated as a command; if false, it is treated as data.
 */
void sendByteToDisplay(uint8_t value, bool cmd) {
    // Start transmission
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_CS, LOW);
    // Type
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SDI, cmd ? LOW : HIGH);
    // Clock tick
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SCL, LOW);
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SCL, HIGH);

    // Indicate payload
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SDI, LOW);
    // Clock tick
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SCL, LOW);
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SCL, HIGH);

    // Send bits MSB first
    for (uint8_t i = 0x80; i; i >>= 1) {
        if (value & i) {
            digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SDI, HIGH);
        } else {
            digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SDI, LOW);
        }
        // Clock tick
        digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SCL, LOW);
        digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_SCL, HIGH);
    }

    // End transmission
    digitalWrite(BOARD_PIN_OUT_PANEL_DISPLAY_CS, HIGH);
}

/**
 * Formats the speed in RPM to a string representation in km/h.
 * Buffer must have 4 characters available (3 digits + comma + 1 digit after comma).
 *
 * @param buffer The character buffer to store the formatted speed (size 4).
 * @param rpm The speed in RPM.
 */
void formatDisplaySpeed(char* buffer, int rpm) {
    float speedKmh = (float) rpm * RPM_TO_KMH; // Convert RPM to km/h

    int first = (int) speedKmh / 10;
    if (speedKmh > 10) {
        buffer[0] = '0' + first;
    } else {
        buffer[0] = ' '; // This is the reason why I don't use sprintf, as it would print a leading zero instead of a space
    }

    int second = ((int) speedKmh) - (first * 10);
    buffer[1] = '0' + second;

    buffer[2] = ',';

    int last = round(speedKmh * 10) % 10;
    buffer[3] = '0' + last;
}
