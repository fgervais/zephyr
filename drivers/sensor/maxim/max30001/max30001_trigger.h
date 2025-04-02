/*
 * Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_MAX30001_TRIGGER_H_
#define ZEPHYR_DRIVERS_SENSOR_MAX30001_TRIGGER_H_

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

/* Configure triggers for the max30001 sensor */
int max30001_trigger_set(const struct device *dev,
			 const struct sensor_trigger *trig,
			 sensor_trigger_handler_t handler);

/* Initialization for max30001 Triggers module */
int max30001_trigger_init(const struct device *dev);

#endif /* ZEPHYR_DRIVERS_SENSOR_MAX30001_TRIGGER_H_ */