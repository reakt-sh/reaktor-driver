#include <Arduino.h>

#include "error.h"

// Variables
uint64_t bitvectorErrors = 0;
uint64_t bitvectorTransients = 0;
bool fatalError = false;

// ====================
// API

void registerFatalError(int error) {
    registerError(error, false);
    fatalError = true;
}

void registerError(int error) {
    registerError(error, true);
}

void registerError(int error, bool transient) {
    uint64_t bit = 1ULL << error;
    bitvectorErrors |= bit;
    if (transient) {
        bitvectorTransients |= bit;
    }
}

void resolveError(int error) {
    uint64_t mask = ~(1ULL << error);
    bitvectorErrors &= mask;
    bitvectorTransients &= mask;
}

bool hasError() {
    return bitvectorErrors != 0;
}

bool hasFatalError() {
    return fatalError;
}

uint64_t consumeErrors() {
    uint64_t errors = bitvectorErrors;
    bitvectorErrors ^= bitvectorTransients; // Keep only non-transient errors
    bitvectorTransients = 0; // Clear transient errors
    return errors;
}
