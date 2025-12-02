#pragma once

#include <errors.h>


/**
 * Register the occurrence of an error.
 * The error will be recorded as transient an must be re-registered if it continues to persist.
 *
 * @param error_code The error code to register (see errors.h for codes)
 */
void registerError(int error_code);

/**
 * Register the occurrence of an error.
 *
 * @param error_code The error code to register (see errors.h for codes)
 * @param fatal Whether the error is fatal and the system cannot recover (requires restart)
 * @param transient Whether the error is transient and can be discarded (has to be re-registered if continues to persist)
 */
void registerError(int error_code, bool transient);

/**
 * Register the occurrence of a permanent fatal error.
 * The system cannot recover from a fatal error and will require a restart.
 *
 * @param error_code The error code to register (see errors.h for codes)
 */
void registerFatalError(int error_code);

/**
 * Removes registration of an error.
 * Works for both transient and permanent errors but has no effect after a fatal error occurred.
 *
 * @param error_code The error code to resolve (see errors.h for codes)
 */
void resolveError(int error_code);

/**
 * Check whether any error has been registered.
 *
 * @return true if any error has been registered, false otherwise
 */
bool hasError();

/**
 * Check whether a fatal error has been registered.
 *
 * @return true if a fatal error has been registered, false otherwise
 */
bool hasFatalError();

/**
 * Returns the bitvector of all registered errors.
 * Each bit corresponds to an error code (1 << error_code).
 * Transient errors will be removed afterwards.
 */
uint64_t consumeErrors();
