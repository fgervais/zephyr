/*
 * Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/sensor_clock.h>

#include "max30001.h"
#include "max30001_reg.h"
#include "max30001_decoder.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MAX30001_DECODER, CONFIG_SENSOR_LOG_LEVEL);

#define DT_DRV_COMPAT maxim_max30001

// static int max30001_get_shift(enum sensor_channel channel,
// 			      int accel_fs,
// 			      int gyro_fs,
// 			      int8_t *shift)
// {
// 	switch (channel) {
// 	case SENSOR_CHAN_ACCEL_XYZ:
// 	case SENSOR_CHAN_ACCEL_X:
// 	case SENSOR_CHAN_ACCEL_Y:
// 	case SENSOR_CHAN_ACCEL_Z:
// 		switch (accel_fs) {
// 		case MAX30001_DT_ACCEL_FS_32:
// 			*shift = 9;
// 			return 0;
// 		case MAX30001_DT_ACCEL_FS_16:
// 			*shift = 8;
// 			return 0;
// 		case MAX30001_DT_ACCEL_FS_8:
// 			*shift = 7;
// 			return 0;
// 		case MAX30001_DT_ACCEL_FS_4:
// 			*shift = 6;
// 			return 0;
// 		case MAX30001_DT_ACCEL_FS_2:
// 			*shift = 5;
// 			return 0;
// 		default:
// 			return -EINVAL;
// 		}
// 	case SENSOR_CHAN_GYRO_XYZ:
// 	case SENSOR_CHAN_GYRO_X:
// 	case SENSOR_CHAN_GYRO_Y:
// 	case SENSOR_CHAN_GYRO_Z:
// 		switch (gyro_fs) {
// 		case MAX30001_DT_GYRO_FS_4000:
// 			*shift = 12;
// 			return 0;
// 		case MAX30001_DT_GYRO_FS_2000:
// 			*shift = 11;
// 			return 0;
// 		case MAX30001_DT_GYRO_FS_1000:
// 			*shift = 10;
// 			return 0;
// 		case MAX30001_DT_GYRO_FS_500:
// 			*shift = 9;
// 			return 0;
// 		case MAX30001_DT_GYRO_FS_250:
// 			*shift = 8;
// 			return 0;
// 		case MAX30001_DT_GYRO_FS_125:
// 			*shift = 7;
// 			return 0;
// 		case MAX30001_DT_GYRO_FS_62_5:
// 			*shift = 6;
// 			return 0;
// 		case MAX30001_DT_GYRO_FS_31_25:
// 			*shift = 5;
// 			return 0;
// 		default:
// 			return -EINVAL;
// 		}
// 	case SENSOR_CHAN_DIE_TEMP:
// 		*shift = 9;
// 		return 0;
// 	default:
// 		return -EINVAL;
// 	}
// }

int max30001_convert_raw_to_q31(struct max30001_encoded_data *edata,
				enum sensor_channel chan,
				int32_t reading,
				q31_t *out)
{
	int32_t whole;
	int32_t fraction = 0;
	int64_t intermediate;
	int8_t shift = 0;
	// int rc;

	// rc = max30001_get_shift(chan, edata->header.accel_fs, edata->header.gyro_fs, &shift);
	// if (rc != 0) {
	// 	return rc;
	// }

	switch (chan) {
	case SENSOR_CHAN_VOLTAGE:
		max30001_ecg_voltage(edata->header.ecg_gain, reading, &whole);
		// LOG_DBG("max30001_ecg_voltage: %d", whole);
		break;
	// case SENSOR_CHAN_ACCEL_XYZ:
	// case SENSOR_CHAN_ACCEL_X:
	// case SENSOR_CHAN_ACCEL_Y:
	// case SENSOR_CHAN_ACCEL_Z:
	// 	max30001_accel_ms(edata->header.accel_fs, reading, false, &whole, &fraction);
	// 	break;
	// case SENSOR_CHAN_GYRO_XYZ:
	// case SENSOR_CHAN_GYRO_X:
	// case SENSOR_CHAN_GYRO_Y:
	// case SENSOR_CHAN_GYRO_Z:
	// 	max30001_gyro_rads(edata->header.gyro_fs, reading, false, &whole, &fraction);
	// 	break;
	// case SENSOR_CHAN_DIE_TEMP:
	// 	max30001_temp_c(reading, &whole, &fraction);
	// 	break;
	default:
		return -ENOTSUP;
	}
	intermediate = ((int64_t)whole * INT64_C(1000000) + fraction);
	if (shift < 0) {
		intermediate =
			intermediate * ((int64_t)INT32_MAX + 1) * (1 << -shift) / INT64_C(1000000);
	} else if (shift > 0) {
		intermediate =
			intermediate * ((int64_t)INT32_MAX + 1) / ((1 << shift) * INT64_C(1000000));
	}
	*out = CLAMP(intermediate, INT32_MIN, INT32_MAX);

	return 0;
}

static int max30001_get_channel_position(enum sensor_channel chan)
{
	switch (chan) {
	case SENSOR_CHAN_VOLTAGE:
		return 0;
	// case SENSOR_CHAN_ACCEL_XYZ:
	// case SENSOR_CHAN_ACCEL_X:
	// 	return offsetof(struct max30001_encoded_payload, accel.x) / sizeof(int16_t);
	// case SENSOR_CHAN_ACCEL_Y:
	// 	return offsetof(struct max30001_encoded_payload, accel.y) / sizeof(int16_t);
	// case SENSOR_CHAN_ACCEL_Z:
	// 	return offsetof(struct max30001_encoded_payload, accel.z) / sizeof(int16_t);
	// case SENSOR_CHAN_GYRO_XYZ:
	// case SENSOR_CHAN_GYRO_X:
	// 	return offsetof(struct max30001_encoded_payload, gyro.x) / sizeof(int16_t);
	// case SENSOR_CHAN_GYRO_Y:
	// 	return offsetof(struct max30001_encoded_payload, gyro.y) / sizeof(int16_t);
	// case SENSOR_CHAN_GYRO_Z:
	// 	return offsetof(struct max30001_encoded_payload, gyro.z) / sizeof(int16_t);
	// case SENSOR_CHAN_DIE_TEMP:
	// 	return offsetof(struct max30001_encoded_payload, temp) / sizeof(int16_t);
	default:
		return 0;
	}
}

static uint8_t max30001_encode_channel(enum sensor_channel chan)
{
	uint8_t encode_bmask = 0;

	switch (chan) {
	case SENSOR_CHAN_VOLTAGE:
		encode_bmask = BIT(max30001_get_channel_position(chan));
	// case SENSOR_CHAN_ACCEL_X:
	// case SENSOR_CHAN_ACCEL_Y:
	// case SENSOR_CHAN_ACCEL_Z:
	// case SENSOR_CHAN_GYRO_X:
	// case SENSOR_CHAN_GYRO_Y:
	// case SENSOR_CHAN_GYRO_Z:
	// case SENSOR_CHAN_DIE_TEMP:
	// 	encode_bmask = BIT(max30001_get_channel_position(chan));
	// 	break;
	// case SENSOR_CHAN_ACCEL_XYZ:
	// 	encode_bmask = BIT(max30001_get_channel_position(SENSOR_CHAN_ACCEL_X)) |
	// 		       BIT(max30001_get_channel_position(SENSOR_CHAN_ACCEL_Y)) |
	// 		       BIT(max30001_get_channel_position(SENSOR_CHAN_ACCEL_Z));
	// 	break;
	// case SENSOR_CHAN_GYRO_XYZ:
	// 	encode_bmask = BIT(max30001_get_channel_position(SENSOR_CHAN_GYRO_X)) |
	// 		       BIT(max30001_get_channel_position(SENSOR_CHAN_GYRO_Y)) |
	// 		       BIT(max30001_get_channel_position(SENSOR_CHAN_GYRO_Z));
	// 	break;
	default:
		break;
	}

	return encode_bmask;
}

int max30001_encode(const struct device *dev,
		    const struct sensor_chan_spec *const channels,
		    const size_t num_channels,
		    uint8_t *buf)
{
	struct max30001_encoded_data *edata = (struct max30001_encoded_data *)buf;
	// const struct max30001_config *dev_config = dev->config;
	uint64_t cycles;
	int err;

	edata->header.channels = 0;

	for (size_t i = 0 ; i < num_channels ; i++) {
		edata->header.channels |= max30001_encode_channel(channels[i].chan_type);
	}

	err = sensor_clock_get_cycles(&cycles);
	if (err != 0) {
		return err;
	}

	edata->header.events = 0;
	// edata->header.accel_fs = dev_config->settings.accel.fs;
	// edata->header.gyro_fs = dev_config->settings.gyro.fs;
	edata->header.timestamp = sensor_clock_cycles_to_ns(cycles);

	return 0;
}

static int max30001_decoder_get_frame_count(const uint8_t *buffer,
					    struct sensor_chan_spec chan_spec,
					    uint16_t *frame_count)
{
	struct max30001_encoded_data *edata = (struct max30001_encoded_data *)buffer;

	if (chan_spec.chan_idx != 0) {
		return -ENOTSUP;
	}

	uint8_t channel_request = max30001_encode_channel(chan_spec.chan_type);

	if ((edata->header.channels & channel_request) != channel_request) {
		return -ENODATA;
	}

	if (!edata->header.events ||
	    (edata->header.events & REG_INT1_STATUS0_DRDY(true))) {
		switch (chan_spec.chan_type) {
		case SENSOR_CHAN_ACCEL_X:
		case SENSOR_CHAN_ACCEL_Y:
		case SENSOR_CHAN_ACCEL_Z:
		case SENSOR_CHAN_ACCEL_XYZ:
		case SENSOR_CHAN_GYRO_X:
		case SENSOR_CHAN_GYRO_Y:
		case SENSOR_CHAN_GYRO_Z:
		case SENSOR_CHAN_GYRO_XYZ:
		case SENSOR_CHAN_DIE_TEMP:
			*frame_count = 1;
			return 0;
		default:
			return -ENOTSUP;
		}
	}

	if (edata->header.events & REG_INT1_STATUS0_FIFO_THS(true) ||
	    edata->header.events & REG_INT1_STATUS0_FIFO_FULL(true)) {
		switch (chan_spec.chan_type) {
		case SENSOR_CHAN_ACCEL_XYZ:
		case SENSOR_CHAN_GYRO_XYZ:
		case SENSOR_CHAN_DIE_TEMP:
			*frame_count = edata->header.fifo_count;
			return 0;
		/** We're skipping individual axis for fifo packets */
		default:
			return -ENOTSUP;
		}
	}

	return -1;
}

static int max30001_decoder_get_size_info(struct sensor_chan_spec chan_spec,
					  size_t *base_size,
					  size_t *frame_size)
{
	switch (chan_spec.chan_type) {
	case SENSOR_CHAN_ACCEL_XYZ:
	case SENSOR_CHAN_GYRO_XYZ:
		*base_size = sizeof(struct sensor_three_axis_data);
		*frame_size = sizeof(struct sensor_three_axis_sample_data);
		return 0;
	case SENSOR_CHAN_ACCEL_X:
	case SENSOR_CHAN_ACCEL_Y:
	case SENSOR_CHAN_ACCEL_Z:
	case SENSOR_CHAN_GYRO_X:
	case SENSOR_CHAN_GYRO_Y:
	case SENSOR_CHAN_GYRO_Z:
	case SENSOR_CHAN_DIE_TEMP:
		*base_size = sizeof(struct sensor_q31_data);
		*frame_size = sizeof(struct sensor_q31_sample_data);
		return 0;
	default:
		return -ENOTSUP;
	}
}

static int max30001_one_shot_decode(const uint8_t *buffer,
				    struct sensor_chan_spec chan_spec,
				    uint32_t *fit,
				    uint16_t max_count,
				    void *data_out)
{
	struct max30001_encoded_data *edata = (struct max30001_encoded_data *)buffer;
	uint8_t channel_request;
	// int err;

	// LOG_DBG("max30001_one_shot_decode:");

	// LOG_DBG("fit: %d", *fit);
	// LOG_DBG("max_count: %d", max_count);
	// LOG_DBG("chan_spec.chan_idx: %d", chan_spec.chan_idx);

	if (*fit != 0) {
		return 0;
	}

	if (max_count == 0 || chan_spec.chan_idx != 0) {
		return -EINVAL;
	}

	switch (chan_spec.chan_type) {
	case SENSOR_CHAN_VOLTAGE:
		uint8_t etag;
		uint32_t cpu_ecg_fifo_data;
		int32_t adc_counts;

		channel_request = max30001_encode_channel(chan_spec.chan_type);
		if ((channel_request & edata->header.channels) != channel_request) {
			return -ENODATA;
		}

		cpu_ecg_fifo_data = sys_be24_to_cpu(
			*(uint32_t *)edata->payload.ecg_fifo_data);
		etag = REG_ECG_FIFO_ETAG(cpu_ecg_fifo_data);

		// LOG_DBG("ETAG: %d", etag);

		switch (etag) {
		case REG_ECG_FIFO_ETAG_VALID_SAMPLE:
		case REG_ECG_FIFO_ETAG_VALID_SAMPLE_EOF:
			// LOG_DBG("Valid sample");
			adc_counts = REG_ECG_FIFO_VOLTAGE_DATA(cpu_ecg_fifo_data);
			adc_counts = sign_extend(adc_counts,
					REG_ECG_FIFO_DATA_SIZE_BITS - 1);
			break;
		case REG_ECG_FIFO_ETAG_EMPTY:
			LOG_DBG("FIFO empty");
			return -ENODATA;
		case REG_ECG_FIFO_ETAG_OVERFLOW:
			LOG_DBG("FIFO overflow");
			return -EOVERFLOW;
		default:
			return -ENOTSUP;
		}

		struct sensor_q31_data *out = data_out;
		// struct max30001_encoded_payload *payload = &edata->payload;

		out->header.base_timestamp_ns = edata->header.timestamp;
		out->header.reading_count = 1;

		// adc_counts =
		// 	((uint32_t)edata->payload.ecg_fifo_data[0] << 16) |
		// 	((uint32_t)edata->payload.ecg_fifo_data[1] << 8) |
		// 	((uint32_t)edata->payload.ecg_fifo_data[2]);

		max30001_convert_raw_to_q31(
			edata,
			chan_spec.chan_type,
			adc_counts,
			&out->readings[0].voltage);

		*fit = 1;
		return 1;
	// case SENSOR_CHAN_ACCEL_X:
	// case SENSOR_CHAN_ACCEL_Y:
	// case SENSOR_CHAN_ACCEL_Z:
	// case SENSOR_CHAN_GYRO_X:
	// case SENSOR_CHAN_GYRO_Y:
	// case SENSOR_CHAN_GYRO_Z:
	// case SENSOR_CHAN_DIE_TEMP: {
	// 	channel_request = max30001_encode_channel(chan_spec.chan_type);
	// 	if ((channel_request & edata->header.channels) != channel_request) {
	// 		return -ENODATA;
	// 	}

	// 	struct sensor_q31_data *out = data_out;

	// 	out->header.base_timestamp_ns = edata->header.timestamp;
	// 	out->header.reading_count = 1;

	// 	err = max30001_get_shift(chan_spec.chan_type,
	// 				 edata->header.accel_fs,
	// 				 edata->header.gyro_fs,
	// 				 &out->shift);
	// 	if (err != 0) {
	// 		return -EINVAL;
	// 	}

	// 	max30001_convert_raw_to_q31(
	// 		edata,
	// 		chan_spec.chan_type,
	// 		edata->payload.readings[max30001_get_channel_position(chan_spec.chan_type)],
	// 		&out->readings[0].value);
	// 	*fit = 1;
	// 	return 1;
	// }
	// case SENSOR_CHAN_ACCEL_XYZ:
	// case SENSOR_CHAN_GYRO_XYZ: {
	// 	channel_request = max30001_encode_channel(chan_spec.chan_type);
	// 	if ((channel_request & edata->header.channels) != channel_request) {
	// 		return -ENODATA;
	// 	}

	// 	struct sensor_three_axis_data *out = data_out;
	// 	struct max30001_encoded_payload *payload = &edata->payload;

	// 	out->header.base_timestamp_ns = edata->header.timestamp;
	// 	out->header.reading_count = 1;

	// 	max30001_convert_raw_to_q31(
	// 		edata,
	// 		chan_spec.chan_type - 3,
	// 		payload->readings[max30001_get_channel_position(chan_spec.chan_type - 3)],
	// 		&out->readings[0].x);
	// 	max30001_convert_raw_to_q31(
	// 		edata,
	// 		chan_spec.chan_type - 2,
	// 		payload->readings[max30001_get_channel_position(chan_spec.chan_type - 2)],
	// 		&out->readings[0].y);
	// 	max30001_convert_raw_to_q31(
	// 		edata,
	// 		chan_spec.chan_type - 1,
	// 		payload->readings[max30001_get_channel_position(chan_spec.chan_type - 1)],
	// 		&out->readings[0].z);
	// 	*fit = 1;
	// 	return 1;
	// }
	default:
		return -EINVAL;
	}
}

// static q31_t max30001_fifo_read_temp_from_packet(const uint8_t *pkt)
// {
// 	struct max30001_encoded_fifo_payload *fdata = (struct max30001_encoded_fifo_payload *)pkt;

// 	int32_t whole;
// 	uint32_t fraction;
// 	int64_t intermediate;
// 	int8_t shift;
// 	int err;

// 	err = max30001_get_shift(SENSOR_CHAN_DIE_TEMP, 0, 0, &shift);
// 	if (err != 0) {
// 		return -1;
// 	}

// 	max30001_temp_c(fdata->temp, &whole, &fraction);

// 	intermediate = ((int64_t)whole * INT64_C(1000000) + fraction);
// 	if (shift < 0) {
// 		intermediate =
// 			intermediate * ((int64_t)INT32_MAX + 1) * (1 << -shift) / INT64_C(1000000);
// 	} else if (shift > 0) {
// 		intermediate =
// 			intermediate * ((int64_t)INT32_MAX + 1) / ((1 << shift) * INT64_C(1000000));
// 	}

// 	return CLAMP(intermediate, INT32_MIN, INT32_MAX);
// }

// static int max30001_fifo_read_imu_from_packet(const uint8_t *pkt,
// 					      bool is_accel,
// 					      uint8_t axis_offset,
// 					      q31_t *out)
// {
// 	uint32_t unsigned_value;
// 	int32_t signed_value;
// 	int offset = 1 + (axis_offset * 2)  + (is_accel ? 0 : 6);
// 	uint32_t mask = is_accel ? GENMASK(7, 4) : GENMASK(3, 0);
// 	uint8_t accel_fs = MAX30001_DT_ACCEL_FS_32;
// 	uint8_t gyro_fs = MAX30001_DT_GYRO_FS_4000;

// 	int32_t whole;
// 	int32_t fraction;
// 	int64_t intermediate;
// 	int8_t shift;

// 	unsigned_value = (pkt[offset] | (pkt[offset + 1] << 8));
// 	if (unsigned_value == FIFO_NO_DATA) {
// 		return -ENODATA;
// 	}

// 	unsigned_value = (unsigned_value << 4) |
// 			 ((pkt[17 + axis_offset] & mask) >> (is_accel ? 4 : 0));
// 	signed_value = sign_extend(unsigned_value, 19);

// 	if (!is_accel) {
// 		max30001_get_shift(SENSOR_CHAN_GYRO_XYZ, accel_fs, gyro_fs, &shift);
// 		max30001_gyro_rads(gyro_fs, signed_value, true, &whole, &fraction);
// 	} else {
// 		max30001_get_shift(SENSOR_CHAN_ACCEL_XYZ, accel_fs, gyro_fs, &shift);
// 		max30001_accel_ms(accel_fs, signed_value, true, &whole, &fraction);
// 	}

// 	intermediate = ((int64_t)whole * INT64_C(1000000) + fraction);
// 	if (shift < 0) {
// 		intermediate =
// 			intermediate * ((int64_t)INT32_MAX + 1) * (1 << -shift) / INT64_C(1000000);
// 	} else if (shift > 0) {
// 		intermediate =
// 			intermediate * ((int64_t)INT32_MAX + 1) / ((1 << shift) * INT64_C(1000000));
// 	}

// 	*out = CLAMP(intermediate, INT32_MIN, INT32_MAX);

// 	return 0;
// }

static int max30001_fifo_read_ecg_from_packet(const uint8_t *pkt,
					      bool is_accel,
					      uint8_t axis_offset,
					      q31_t *out)
{
	// uint32_t unsigned_value;
	// int32_t signed_value;
	// int offset = 1 + (axis_offset * 2)  + (is_accel ? 0 : 6);
	// uint32_t mask = is_accel ? GENMASK(7, 4) : GENMASK(3, 0);
	// uint8_t accel_fs = MAX30001_DT_ACCEL_FS_32;
	// uint8_t gyro_fs = MAX30001_DT_GYRO_FS_4000;

	// int32_t whole;
	// int32_t fraction;
	// int64_t intermediate;
	// int8_t shift;

	// unsigned_value = (pkt[offset] | (pkt[offset + 1] << 8));
	// if (unsigned_value == FIFO_NO_DATA) {
	// 	return -ENODATA;
	// }

	// unsigned_value = (unsigned_value << 4) |
	// 		 ((pkt[17 + axis_offset] & mask) >> (is_accel ? 4 : 0));
	// signed_value = sign_extend(unsigned_value, 19);

	// if (!is_accel) {
	// 	max30001_get_shift(SENSOR_CHAN_GYRO_XYZ, accel_fs, gyro_fs, &shift);
	// 	max30001_gyro_rads(gyro_fs, signed_value, true, &whole, &fraction);
	// } else {
	// 	max30001_get_shift(SENSOR_CHAN_ACCEL_XYZ, accel_fs, gyro_fs, &shift);
	// 	max30001_accel_ms(accel_fs, signed_value, true, &whole, &fraction);
	// }

	// intermediate = ((int64_t)whole * INT64_C(1000000) + fraction);
	// if (shift < 0) {
	// 	intermediate =
	// 		intermediate * ((int64_t)INT32_MAX + 1) * (1 << -shift) / INT64_C(1000000);
	// } else if (shift > 0) {
	// 	intermediate =
	// 		intermediate * ((int64_t)INT32_MAX + 1) / ((1 << shift) * INT64_C(1000000));
	// }

	// *out = CLAMP(intermediate, INT32_MIN, INT32_MAX);

	return 0;
}

static int max30001_fifo_decode(const uint8_t *buffer,
				struct sensor_chan_spec chan_spec,
				uint32_t *fit,
				uint16_t max_count,
				void *data_out)
{
	struct max30001_encoded_data *edata = (struct max30001_encoded_data *)buffer;
	struct max30001_encoded_payload *frame_begin = &edata->payload;
	int count = 0;
	int err;

	if (*fit >= edata->header.fifo_count || chan_spec.chan_idx != 0) {
		return 0;
	}

	while (count < max_count && (*fit < edata->header.fifo_count)) {
		struct max30001_encoded_payload *fdata = &frame_begin[*fit];

		/** This driver assumes 20-byte fifo packets, with both accel and gyro,
		 * and no auxiliary sensors.
		 */
		// __ASSERT(!(fdata->header & FIFO_HEADER_EXT_HEADER_EN(true)) &&
		// 	(fdata->header & FIFO_HEADER_ACCEL_EN(true)) &&
		// 	(fdata->header & FIFO_HEADER_GYRO_EN(true)) &&
		// 	(fdata->header & FIFO_HEADER_HIRES_EN(true)),
		// 	"Unsupported FIFO packet format");

		switch (chan_spec.chan_type) {
		case SENSOR_CHAN_VOLTAGE:
			struct sensor_three_axis_data *out = data_out;
			bool is_accel = chan_spec.chan_type == SENSOR_CHAN_VOLTAGE;

			// max30001_get_shift(chan_spec.chan_type,
			// 		   edata->header.accel_fs,
			// 		   edata->header.gyro_fs,
			// 		   &out->shift);

			out->header.base_timestamp_ns = edata->header.timestamp;

			err = max30001_fifo_read_ecg_from_packet((uint8_t *)fdata,
								 is_accel,
								 0,
								 &out->readings[count].x);
			if (err != 0) {
				count--;
			}
			break;


		// case SENSOR_CHAN_ACCEL_XYZ:
		// case SENSOR_CHAN_GYRO_XYZ: {
		// 	struct sensor_three_axis_data *out = data_out;
		// 	bool is_accel = chan_spec.chan_type == SENSOR_CHAN_ACCEL_XYZ;

		// 	max30001_get_shift(chan_spec.chan_type,
		// 			   edata->header.accel_fs,
		// 			   edata->header.gyro_fs,
		// 			   &out->shift);

		// 	out->header.base_timestamp_ns = edata->header.timestamp;

		// 	err = max30001_fifo_read_imu_from_packet((uint8_t *)fdata,
		// 						 is_accel,
		// 						 0,
		// 						 &out->readings[count].x);
		// 	err |= max30001_fifo_read_imu_from_packet((uint8_t *)fdata,
		// 						  is_accel,
		// 						  1,
		// 						  &out->readings[count].y);
		// 	err |= max30001_fifo_read_imu_from_packet((uint8_t *)fdata,
		// 						  is_accel,
		// 						  2,
		// 						  &out->readings[count].z);
		// 	if (err != 0) {
		// 		count--;
		// 	}
		// 	break;
		// }
		// case SENSOR_CHAN_DIE_TEMP: {
		// 	struct sensor_q31_data *out = data_out;

		// 	max30001_get_shift(chan_spec.chan_type,
		// 			edata->header.accel_fs,
		// 			edata->header.gyro_fs,
		// 			&out->shift);

		// 	out->header.base_timestamp_ns = edata->header.timestamp;
		// 	out->readings[count].temperature =
		// 		max30001_fifo_read_temp_from_packet((uint8_t *)fdata);
		// 	break;
		// }
		default:
			return 0;
		}
		*fit = *fit + 1;
		count++;
	}

	return count;
}

static int max30001_decoder_decode(const uint8_t *buffer,
				   struct sensor_chan_spec chan_spec,
				   uint32_t *fit,
				   uint16_t max_count,
				   void *data_out)
{
	struct max30001_encoded_data *edata = (struct max30001_encoded_data *)buffer;

	if (edata->header.events & REG_INT1_STATUS0_FIFO_THS(true) ||
	    edata->header.events & REG_INT1_STATUS0_FIFO_FULL(true)) {
		return max30001_fifo_decode(buffer, chan_spec, fit, max_count, data_out);
	} else {
		return max30001_one_shot_decode(buffer, chan_spec, fit, max_count, data_out);
	}
}

static bool max30001_decoder_has_trigger(const uint8_t *buffer, enum sensor_trigger_type trigger)
{
	struct max30001_encoded_data *edata = (struct max30001_encoded_data *)buffer;

	switch (trigger) {
	case SENSOR_TRIG_DATA_READY:
		return edata->header.events & REG_INT1_STATUS0_DRDY(true);
	case SENSOR_TRIG_FIFO_WATERMARK:
		return edata->header.events & REG_INT1_STATUS0_FIFO_THS(true);
	case SENSOR_TRIG_FIFO_FULL:
		return edata->header.events & REG_INT1_STATUS0_FIFO_FULL(true);
	default:
		break;
	}

	return false;
}

SENSOR_DECODER_API_DT_DEFINE() = {
	.get_frame_count = max30001_decoder_get_frame_count,
	.get_size_info = max30001_decoder_get_size_info,
	.decode = max30001_decoder_decode,
	.has_trigger = max30001_decoder_has_trigger,
};

int max30001_get_decoder(const struct device *dev,
			 const struct sensor_decoder_api **decoder)
{
	ARG_UNUSED(dev);
	*decoder = &SENSOR_DECODER_NAME();

	return 0;
}