/*
 * Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_MAX30001_STREAM_H_
#define ZEPHYR_DRIVERS_SENSOR_MAX30001_STREAM_H_

int max30001_stream_init(const struct device *dev);

void max30001_stream_submit(const struct device *dev,
			    struct rtio_iodev_sqe *iodev_sqe);

#endif /* ZEPHYR_DRIVERS_SENSOR_MAX30001_STREAM_H_ */