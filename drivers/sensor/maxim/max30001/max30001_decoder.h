/*
 * Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_MAX30001_DECODER_H_
#define ZEPHYR_DRIVERS_SENSOR_MAX30001_DECODER_H_

#include <stdint.h>
#include <zephyr/drivers/sensor.h>
#include "max30001.h"

int max30001_encode(const struct device *dev,
		    const struct sensor_chan_spec *const channels,
		    const size_t num_channels,
		    uint8_t *buf);

int max30001_get_decoder(const struct device *dev,
			 const struct sensor_decoder_api **decoder);

#endif /* ZEPHYR_DRIVERS_SENSOR_MAX30001_DECODER_H_ */