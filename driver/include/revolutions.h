#pragma once

/**
 * Setup the revolutions detector.
 *
 * @return true if the setup was successful
 */
bool setupRevolutionsDetection();

/**
 * Return the rotational speed of the motor.
 *
 * @return speed in revolutions per minute
 */
int getRevolutionsPerMinute();
