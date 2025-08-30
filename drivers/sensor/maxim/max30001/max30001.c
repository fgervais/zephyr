/*
 * Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT maxim_max30001

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/rtio/rtio.h>

#if defined(CONFIG_SENSOR_ASYNC_API)
#include <zephyr/rtio/work.h>
#include <zephyr/rtio/regmap.h>
#endif /* CONFIG_SENSOR_ASYNC_API */

#include "max30001.h"
#include "max30001_reg.h"
#include "max30001_bus.h"
#include "max30001_decoder.h"
#include "max30001_trigger.h"
#include "max30001_stream.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MAX30001, CONFIG_SENSOR_LOG_LEVEL);

static inline int reg_write(const struct device *dev, uint8_t reg, uint32_t val)
{
	union {
		struct {
			uint32_t value;
		} __packed;
		uint8_t buf[4];
	} write = {0};

	write.value = sys_cpu_to_be24(val);

	LOG_DBG("reg: 0x%02x", reg);
	LOG_DBG("├── val: 0x%06x", val);
	LOG_DBG("└── write.value: 0x%06x", write.value);

	return max30001_bus_write(dev, reg, write.buf, 3);
}

static inline int reg_read(const struct device *dev, uint8_t reg, uint32_t *val)
{
	int err;
	union {
		struct {
			uint32_t value;
		} __packed;
		uint8_t buf[4];
	} read = {0};

	err = max30001_bus_read(dev, reg, read.buf, 3);
	if (err == 0) {
		*val = sys_be24_to_cpu(read.value);
	}

	LOG_DBG("reg: 0x%02x", reg);
	LOG_DBG("├── read.value: 0x%06x", read.value);
	LOG_DBG("└── val: 0x%06x", *val);

	return err;
}

static int max30001_sample_fetch(const struct device *dev,
				 enum sensor_channel chan)
{
	int err;
	struct max30001_data *data = dev->data;
	struct max30001_encoded_data *edata = &data->edata;

	if (chan != SENSOR_CHAN_ALL) {
		return -ENOTSUP;
	}

	err = max30001_bus_read(dev,
				REG_ECG_FIFO,
				edata->payload.buf,
				sizeof(edata->payload.buf));
	if (err == 0) {
		edata->payload.data = ((uint32_t)edata->payload.buf[0] << 16) |
                 		       ((uint32_t)edata->payload.buf[1] << 8) |
                 		       ((uint32_t)edata->payload.buf[2]);
	}

	LOG_HEXDUMP_DBG(edata->payload.buf,
			sizeof(edata->payload.buf),
			"MAX30001 data");

	return err;
}

static int max30001_channel_get(const struct device *dev,
				enum sensor_channel chan,
				struct sensor_value *val)
{
	struct max30001_data *data = dev->data;
	const struct max30001_config *cfg = dev->config;

	switch (chan) {
	case SENSOR_CHAN_VOLTAGE:
		LOG_DBG("max30001_channel_get:");
		LOG_DBG("├── gain: %d", cfg->settings.ecg.gain);
		LOG_DBG("└── etag: %d", data->edata.payload.etag);
		LOG_DBG("└── payload (ecg): %d", data->edata.payload.ecg);

		max30001_ecg_voltage(cfg->settings.ecg.gain,
				     data->edata.payload.ecg,
				     &val->val1);
		break;
	// case SENSOR_CHAN_ACCEL_X:
	// 	max30001_accel_ms(data->edata.header.accel_fs, data->edata.payload.accel.x, false,
	// 			  &val->val1, &val->val2);
	// 	break;
	// case SENSOR_CHAN_ACCEL_Y:
	// 	max30001_accel_ms(data->edata.header.accel_fs, data->edata.payload.accel.y, false,
	// 			  &val->val1, &val->val2);
	// 	break;
	// case SENSOR_CHAN_ACCEL_Z:
	// 	max30001_accel_ms(data->edata.header.accel_fs, data->edata.payload.accel.z, false,
	// 			  &val->val1, &val->val2);
	// 	break;
	// case SENSOR_CHAN_GYRO_X:
	// 	max30001_gyro_rads(data->edata.header.gyro_fs, data->edata.payload.gyro.x, false,
	// 			   &val->val1, &val->val2);
	// 	break;
	// case SENSOR_CHAN_GYRO_Y:
	// 	max30001_gyro_rads(data->edata.header.gyro_fs, data->edata.payload.gyro.y, false,
	// 			   &val->val1, &val->val2);
	// 	break;
	// case SENSOR_CHAN_GYRO_Z:
	// 	max30001_gyro_rads(data->edata.header.gyro_fs, data->edata.payload.gyro.z, false,
	// 			   &val->val1, &val->val2);
	// 	break;
	// case SENSOR_CHAN_DIE_TEMP:
	// 	max30001_temp_c(data->edata.payload.temp, &val->val1, &val->val2);
	// 	break;
	// case SENSOR_CHAN_ACCEL_XYZ:
	// 	max30001_accel_ms(data->edata.header.accel_fs, data->edata.payload.accel.x, false,
	// 			  &val[0].val1, &val[0].val2);
	// 	max30001_accel_ms(data->edata.header.accel_fs, data->edata.payload.accel.y, false,
	// 			  &val[1].val1, &val[1].val2);
	// 	max30001_accel_ms(data->edata.header.accel_fs, data->edata.payload.accel.z, false,
	// 			  &val[2].val1, &val[2].val2);
	// 	break;
	// case SENSOR_CHAN_GYRO_XYZ:
	// 	max30001_gyro_rads(data->edata.header.gyro_fs, data->edata.payload.gyro.x, false,
	// 			   &val->val1, &val->val2);
	// 	max30001_gyro_rads(data->edata.header.gyro_fs, data->edata.payload.gyro.y, false,
	// 			   &val[1].val1, &val[1].val2);
	// 	max30001_gyro_rads(data->edata.header.gyro_fs, data->edata.payload.gyro.z, false,
	// 			   &val[2].val1, &val[2].val2);
	// 	break;
	default:
		return -ENOTSUP;
	}

	return 0;
}

#if defined(CONFIG_SENSOR_ASYNC_API)

// static void max30001_complete_result(struct rtio *ctx,
// 				     const struct rtio_sqe *sqe,
// 				     void *arg)
// {
// 	struct rtio_iodev_sqe *iodev_sqe = (struct rtio_iodev_sqe *)sqe->userdata;
// 	struct rtio_cqe *cqe;
// 	int err = 0;

// 	do {
// 		cqe = rtio_cqe_consume(ctx);
// 		if (cqe != NULL) {
// 			err = cqe->result;
// 			rtio_cqe_release(ctx, cqe);
// 		}
// 	} while (cqe != NULL);

// 	if (err) {
// 		rtio_iodev_sqe_err(iodev_sqe, err);
// 	} else {
// 		rtio_iodev_sqe_ok(iodev_sqe, 0);
// 	}

// 	LOG_DBG("One-shot fetch completed");
// }

static void max30001_one_shot_complete_cb(struct rtio *ctx,
					  const struct rtio_sqe *sqe,
					  void *arg)
{
	struct rtio_iodev_sqe *iodev_sqe = (struct rtio_iodev_sqe *)sqe->userdata;
	int err = 0;

	rtio_flush_completion_queue(ctx);

	if (err) {
		rtio_iodev_sqe_err(iodev_sqe, err);
	} else {
		rtio_iodev_sqe_ok(iodev_sqe, 0);
	}
}

static inline void max30001_submit_one_shot(const struct device *dev,
					    struct rtio_iodev_sqe *iodev_sqe)
{
	const struct sensor_read_config *cfg = iodev_sqe->sqe.iodev->data;
	const struct sensor_chan_spec *const channels = cfg->channels;
	const size_t num_channels = cfg->count;
	uint32_t min_buf_len = sizeof(struct max30001_encoded_data);
	int err;
	uint8_t *buf;
	uint32_t buf_len;
	struct max30001_encoded_data *edata;
	struct max30001_data *data = dev->data;

	err = rtio_sqe_rx_buf(iodev_sqe, min_buf_len, min_buf_len, &buf, &buf_len);
	if (err != 0) {
		LOG_ERR("Failed to get a read buffer of size %u bytes", min_buf_len);
		rtio_iodev_sqe_err(iodev_sqe, err);
		return;
	}

	edata = (struct max30001_encoded_data *)buf;

	err = max30001_encode(dev, channels, num_channels, buf);
	if (err != 0) {
		LOG_ERR("Failed to encode sensor data");
		rtio_iodev_sqe_err(iodev_sqe, err);
		return;
	}



	struct rtio_regs out_fifo_regs;
	struct rtio_regs_list fifo_regs_list[] = {
		{
			REG_ECG_FIFO | REG_SPI_READ_BIT,
			(uint8_t *)edata->payload.buf,
			3,
		},
	};

	out_fifo_regs.rtio_regs_list = fifo_regs_list;
	out_fifo_regs.rtio_regs_num = ARRAY_SIZE(fifo_regs_list);

	/*
	 * Prepare rtio enabled bus to read IIS3DWB_OUTX_L_A register
	 * where accelerometer data is available.
	 * Then iis3dwb_one_shot_complete_cb callback will be invoked.
	 *
	 * STMEMSC API equivalent code:
	 *
	 *   uint8_t accel_raw[6];
	 *
	 *   iis3dwb_acceleration_raw_get(&dev_ctx, accel_raw);
	 */
	rtio_read_regs_async(data->rtio.ctx, data->rtio.iodev, RTIO_BUS_SPI,
			     &out_fifo_regs, iodev_sqe, dev,
			     max30001_one_shot_complete_cb);






	// struct rtio_sqe *write_sqe = rtio_sqe_acquire(data->rtio.ctx);
	// struct rtio_sqe *read_sqe = rtio_sqe_acquire(data->rtio.ctx);
	// struct rtio_sqe *complete_sqe = rtio_sqe_acquire(data->rtio.ctx);

	// if (!write_sqe || !read_sqe | !complete_sqe) {
	// 	LOG_ERR("Failed to acquire RTIO SQEs");
	// 	rtio_iodev_sqe_err(iodev_sqe, -ENOMEM);
	// 	return;
	// }

	// uint8_t val = REG_ECG_FIFO | REG_SPI_READ_BIT;

	// rtio_sqe_prep_tiny_write(write_sqe,
	// 			 data->rtio.iodev,
	// 			 RTIO_PRIO_HIGH,
	// 			 &val,
	// 			 1,
	// 			NULL);
	// write_sqe->flags |= RTIO_SQE_TRANSACTION;

	// rtio_sqe_prep_read(read_sqe,
	// 		   data->rtio.iodev,
	// 		   RTIO_PRIO_HIGH,
	// 		   edata->payload.buf,
	// 		   sizeof(edata->payload.buf),
	// 		   NULL);
	// read_sqe->flags |= RTIO_SQE_CHAINED;

	// rtio_sqe_prep_callback_no_cqe(complete_sqe,
	// 			      max30001_complete_result,
	// 			      (void *)dev,
	// 			      iodev_sqe);

	// rtio_submit(data->rtio.ctx, 0);
}

static void max30001_submit(const struct device *dev, struct rtio_iodev_sqe *iodev_sqe)
{
	const struct sensor_read_config *cfg = iodev_sqe->sqe.iodev->data;

	if (!cfg->is_streaming) {
		max30001_submit_one_shot(dev, iodev_sqe);
	} else if (IS_ENABLED(CONFIG_MAX30001_STREAM)) {
		max30001_stream_submit(dev, iodev_sqe);
	} else {
		LOG_ERR("Streaming not supported");
		rtio_iodev_sqe_err(iodev_sqe, -ENOTSUP);
	}
}

#endif /* CONFIG_SENSOR_ASYNC_API */

// static DEVICE_API(sensor, max30001_driver_api) = {
static const struct sensor_driver_api max30001_driver_api = {
	.sample_fetch = max30001_sample_fetch,
	.channel_get = max30001_channel_get,
#if defined(CONFIG_MAX30001_TRIGGER)
	.trigger_set = max30001_trigger_set,
#endif /* CONFIG_MAX30001_TRIGGER */
#if defined(CONFIG_SENSOR_ASYNC_API)
	.get_decoder = max30001_get_decoder,
	.submit = max30001_submit,
#endif /* CONFIG_SENSOR_ASYNC_API */
};

static int max30001_init(const struct device *dev)
{
	struct max30001_data *data = dev->data;
	// const struct max30001_config *cfg = dev->config;
	uint32_t read_val = 0;
	// uint8_t val;
	int err;

	if (!spi_is_ready_iodev(data->rtio.iodev)) {
		LOG_ERR("Bus is not ready");
		return -ENODEV;
	}

	/* Soft-reset sensor to restore config to defaults */
	err = reg_write(dev, REG_SW_RST, 0x00);
	if (err) {
		LOG_ERR("Failed to write soft-reset: %d", err);
		return err;
	}
	/* Wait for soft-reset to take effect */
	k_sleep(K_MSEC(1));

	/* A complete soft-reset clears the bit */
	// err = reg_read(dev, REG_MISC2, &read_val);
	// if (err) {
	// 	LOG_ERR("Failed to read soft-reset: %d", err);
	// 	return err;
	// }
	// if ((read_val & REG_MISC2_SOFT_RST(1)) != 0) {
	// 	LOG_ERR("Soft-reset command failed");
	// 	return -EIO;
	// }

	/* Set Slew-rate to 10-ns typical, to allow proper SPI readouts */
	// err = reg_write(dev, REG_DRIVE_CONFIG0, REG_DRIVE_CONFIG0_SPI_SLEW(2));
	// if (err) {
	// 	LOG_ERR("Failed to write slew-rate: %d", err);
	// 	return err;
	// }
	// /* Wait for register to take effect */
	// k_sleep(K_USEC(2));

	/* Do any command so INFO will read correctly after a soft reset */
	err = reg_write(dev, REG_NO_OP, 0x00);
	if (err) {
		LOG_ERR("Failed to write to no-op: %d", err);
		return err;
	}

	/* Confirm ID Value matches */
	err = reg_read(dev, REG_INFO, &read_val);
	if (err) {
		LOG_ERR("Failed to read INFO: %d", err);
		return err;
	}
	if (REG_INFO_ALT_BIT_PATTERN(read_val) != REG_INFO_PATTERN) {
		LOG_ERR("Unexpected INFO value - expected: 0x%02lx, actual: 0x%02lx",
			REG_INFO_PATTERN, REG_INFO_ALT_BIT_PATTERN(read_val));
		return -EIO;
	}
	LOG_INF("INFO value - 0x%08x", read_val);





	// err = reg_read(dev, REG_INFO, &read_val);
	// if (err) {
	// 	LOG_ERR("Failed to read INFO: %d", err);
	// 	return err;
	// }
	// if (REG_INFO_ALT_BIT_PATTERN(read_val) != REG_INFO_PATTERN) {
	// 	LOG_ERR("Unexpected INFO value - expected: 0x%02lx, actual: 0x%02lx",
	// 		REG_INFO_PATTERN, REG_INFO_ALT_BIT_PATTERN(read_val));
	// 	return -EIO;
	// }
	// LOG_INF("INFO value - 0x%08x", read_val);





	/* Sensor Configuration */

	err = reg_write(dev, REG_CNFG_EMUX,
			!REG_CNFG_EMUX_ECG_OPENP |
			!REG_CNFG_EMUX_ECG_OPENN);
	if (err) {
		LOG_ERR("Failed to configure the input multiplexer: %d", err);
		return err;
	}

	err = reg_write(dev, REG_CNFG_GEN, REG_CNFG_GEN_EN_ECG);
	if (err) {
		LOG_ERR("Failed to enable ECG channel: %d", err);
		return err;
	}





	// val = REG_PWR_MGMT0_ACCEL_MODE(cfg->settings.accel.pwr_mode) |
	//       REG_PWR_MGMT0_GYRO_MODE(cfg->settings.gyro.pwr_mode);
	// err = reg_write(dev, REG_PWR_MGMT0, val);
	// if (err) {
	// 	LOG_ERR("Failed to write Power settings: %d", err);
	// 	return err;
	// }

	// val = REG_ACCEL_CONFIG0_ODR(cfg->settings.accel.odr) |
	//       REG_ACCEL_CONFIG0_FS(cfg->settings.accel.fs);
	// err = reg_write(dev, REG_ACCEL_CONFIG0, val);
	// if (err) {
	// 	LOG_ERR("Failed to write Accel settings: %d", err);
	// 	return err;
	// }

	// val = REG_GYRO_CONFIG0_ODR(cfg->settings.gyro.odr) |
	//       REG_GYRO_CONFIG0_FS(cfg->settings.gyro.fs);
	// err = reg_write(dev, REG_GYRO_CONFIG0, val);
	// if (err) {
	// 	LOG_ERR("Failed to write Gyro settings: %d", err);
	// 	return err;
	// }

	// /** Write Low-pass filter settings through indirect register access */
	// uint8_t gyro_lpf_write_array[] = REG_IREG_PREPARE_WRITE_ARRAY(
	// 					REG_IPREG_SYS1_OFFSET,
	// 					REG_IPREG_SYS1_REG_172,
	// 					REG_IPREG_SYS1_REG_172_GYRO_LPFBW_SEL(
	// 						cfg->settings.gyro.lpf));

	// err = max30001_bus_write(dev, REG_IREG_ADDR_15_8, gyro_lpf_write_array,
	// 			 sizeof(gyro_lpf_write_array));
	// if (err) {
	// 	LOG_ERR("Failed to set Gyro BW settings: %d", err);
	// 	return err;
	// }

	// /** Wait before indirect register write is made effective
	//  * before proceeding with next one.
	//  */
	// k_sleep(K_MSEC(1));

	// uint8_t accel_lpf_write_array[] = REG_IREG_PREPARE_WRITE_ARRAY(
	// 					REG_IPREG_SYS2_OFFSET,
	// 					REG_IPREG_SYS2_REG_131,
	// 					REG_IPREG_SYS2_REG_131_ACCEL_LPFBW_SEL(
	// 						cfg->settings.accel.lpf));

	// err = max30001_bus_write(dev, REG_IREG_ADDR_15_8, accel_lpf_write_array,
	// 			 sizeof(accel_lpf_write_array));
	// if (err) {
	// 	LOG_ERR("Failed to set Accel BW settings: %d", err);
	// 	return err;
	// }

	if (IS_ENABLED(CONFIG_MAX30001_TRIGGER)) {
		err = max30001_trigger_init(dev);
		if (err) {
			LOG_ERR("Failed to initialize triggers: %d", err);
			return err;
		}
	} else if (IS_ENABLED(CONFIG_MAX30001_STREAM)) {
		err = max30001_stream_init(dev);
		if (err) {
			LOG_ERR("Failed to initialize streaming: %d", err);
			return err;
		}
	}

	LOG_DBG("Init OK");

	return 0;
}

#define MAX30001_VALID_ACCEL_ODR(pwr_mode, odr)							   \
	((pwr_mode == MAX30001_DT_ACCEL_LP && odr >= MAX30001_DT_ACCEL_ODR_400) ||		   \
	 (pwr_mode == MAX30001_DT_ACCEL_LN && odr <= MAX30001_DT_ACCEL_ODR_12_5) ||		   \
	 (pwr_mode == MAX30001_DT_ACCEL_OFF))

#define MAX30001_VALID_GYRO_ODR(pwr_mode, odr)							   \
	((pwr_mode == MAX30001_DT_GYRO_LP && odr >= MAX30001_DT_GYRO_ODR_400) ||		   \
	 (pwr_mode == MAX30001_DT_GYRO_LN && odr <= MAX30001_DT_GYRO_ODR_12_5) ||		   \
	 (pwr_mode == MAX30001_DT_GYRO_OFF))

#define MAX30001_INIT(inst)									   \
												   \
	RTIO_DEFINE(max30001_rtio_ctx_##inst, 8, 8);						   \
	SPI_DT_IODEV_DEFINE(max30001_bus_##inst,						   \
			    DT_DRV_INST(inst),							   \
			    SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,		   \
			    0U);								   \
												   \
	static const struct max30001_config max30001_cfg_##inst = {				   \
		.settings = {									   \
			.ecg = {								   \
				.gain = DT_INST_PROP(inst, ecg_gain),				   \
			},									   \
			.fifo_watermark = DT_INST_PROP_OR(inst, fifo_watermark, 0),		   \
		},										   \
		.int_gpio = GPIO_DT_SPEC_INST_GET_OR(inst, int_gpios, {0}),			   \
	};											   \
	static struct max30001_data max30001_data_##inst = {					   \
		.edata.header = {								   \
		},										   \
		.rtio = {									   \
			.iodev = &max30001_bus_##inst,						   \
			.ctx = &max30001_rtio_ctx_##inst,					   \
		},										   \
	};											   \
												   \
	/* Build-time settings verification: Inform the user of invalid settings at build time */  \
	BUILD_ASSERT(MAX30001_VALID_ACCEL_ODR(DT_INST_PROP(inst, accel_pwr_mode),		   \
					      DT_INST_PROP(inst, accel_odr)),			   \
		     "Invalid accel ODR setting. Please check supported ODRs for LP and LN");	   \
	BUILD_ASSERT(MAX30001_VALID_GYRO_ODR(DT_INST_PROP(inst, gyro_pwr_mode),			   \
					     DT_INST_PROP(inst, gyro_odr)),			   \
		     "Invalid gyro ODR setting. Please check supported ODRs for LP and LN");	   \
												   \
	SENSOR_DEVICE_DT_INST_DEFINE(inst, max30001_init,					   \
				     NULL,							   \
				     &max30001_data_##inst,					   \
				     &max30001_cfg_##inst,					   \
				     POST_KERNEL,						   \
				     CONFIG_SENSOR_INIT_PRIORITY,				   \
				     &max30001_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MAX30001_INIT)