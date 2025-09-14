/*
 * Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_MAX30001_REG_H_
#define ZEPHYR_DRIVERS_SENSOR_MAX30001_REG_H_

#include <zephyr/sys/util.h>
#include <zephyr/sys/byteorder.h>

#define REG_SPI_READ_BIT	BIT(0)

/* Registers */
#define REG_NO_OP		0x00
#define REG_STATUS		0x01
#define REG_EN_INT		0x02
#define REG_EN_INT2		0x03
#define REG_MNGR_INT		0x04
#define REG_MNGR_DYN		0x05
#define REG_SW_RST		0x08
#define REG_SYNCH		0x09
#define REG_FIFO_RST		0x0A
#define REG_INFO		0x0F
#define REG_CNFG_GEN		0x10
#define REG_CNFG_CAL		0x12
#define REG_CNFG_EMUX		0x14
#define REG_CNFG_ECG		0x15
#define REG_CNFG_BMUX		0x16
#define REG_CNFG_BIOZ		0x18
#define REG_CNFG_PACE		0x1A
#define REG_CNFG_RTOR1		0x1D
#define REG_CNFG_RTOR2		0x1E
#define REG_ECG_FIFO_BURST	0x20
#define REG_ECG_FIFO		0x21
#define REG_BIOZ_FIFO_BURST	0x22
#define REG_BIOZ_FIFO		0x23
#define REG_RTOR		0x25
#define REG_PACE0_BURST		0x30
#define REG_PACE0_A		0x31
#define REG_PACE0_B		0x32
#define REG_PACE0_C		0x33
#define REG_PACE1_BURST		0x34
#define REG_PACE1_A		0x35
#define REG_PACE1_B		0x36
#define REG_PACE1_C		0x37
#define REG_PACE2_BURST		0x38
#define REG_PACE2_A		0x39
#define REG_PACE2_B		0x3A
#define REG_PACE2_C		0x3B
#define REG_PACE3_BURST		0x3C
#define REG_PACE3_A		0x3D
#define REG_PACE3_B		0x3E
#define REG_PACE3_C		0x3F
#define REG_PACE4_BURST		0x40
#define REG_PACE4_A		0x41
#define REG_PACE4_B		0x42
#define REG_PACE4_C		0x43
#define REG_PACE5_BURST		0x44
#define REG_PACE5_A		0x45
#define REG_PACE5_B		0x46
#define REG_PACE5_C		0x47
#define REG_NO_OP2		0x7F


#define REG_INFO_PATTERN			(BIT(2) | BIT(0))
#define REG_CNFG_GEN_EN_ECG			BIT(19)

#define REG_CNFG_CAL_EN_VCAL			BIT(22)
#define REG_CNFG_CAL_VMODE			BIT(21)
#define REG_CNFG_CAL_FCAL_1HZ			BIT(14)
#define REG_CNFG_CAL_FIFTY			BIT(11)

#define REG_CNFG_EMUX_ECG_OPENP			BIT(21)
#define REG_CNFG_EMUX_ECG_OPENN			BIT(20)
#define REG_CNFG_EMUX_ECG_CALP_VMID		BIT(18)
#define REG_CNFG_EMUX_ECG_CALP_VCALP		BIT(19)
#define REG_CNFG_EMUX_ECG_CALN_VMID		BIT(16)
#define REG_CNFG_EMUX_ECG_CALN_VCALN		(BIT(17) | BIT(16))

#define REG_CNFG_ECG_RATE_FAST			0
#define REG_CNFG_ECG_RATE_MEDIUM		BIT(22)
#define REG_CNFG_ECG_RATE_SLOW			BIT(23)
#define REG_CNFG_ECG_DHPF_BYPASS		0
#define REG_CNFG_ECG_DHPF_05HZ			BIT(14)
#define REG_CNFG_ECG_DLPF_BYPASS		0
#define REG_CNFG_ECG_DLPF_40HZ			BIT(12)
#define REG_CNFG_ECG_DLPF_100HZ			BIT(13)
#define REG_CNFG_ECG_DLPF_150HZ			BIT(13) | BIT(12)

#define REG_ECG_FIFO_LEN			3
#define REG_ECG_FIFO_ETAG_VALID_SAMPLE		0
#define REG_ECG_FIFO_ETAG_FAST_MODE_SAMPLE	BIT(0)
#define REG_ECG_FIFO_ETAG_VALID_SAMPLE_EOF	BIT(1)
#define REG_ECG_FIFO_ETAG_FAST_MODE_EOF		(BIT(1) | BIT(0))
#define REG_ECG_FIFO_ETAG_EMPTY			(BIT(2) | BIT(1))
#define REG_ECG_FIFO_ETAG_OVERFLOW		(BIT(2) | BIT(1) | BIT(0))
#define REG_ECG_FIFO_DATA_SIZE_BITS		18


#define REG_INFO_ALT_BIT_PATTERN(val)		((val >> 20) & BIT_MASK(4))

#define REG_ECG_FIFO_ETAG(val)			((val >> 3) & BIT_MASK(3))
#define REG_ECG_FIFO_VOLTAGE_DATA(val)		((val >> 6) & BIT_MASK(18))





/* Register Bank 0 */
// #define REG_ACCEL_DATA_X1_UI			0x00
// #define REG_ACCEL_DATA_X0_UI			0x01
// #define REG_ACCEL_DATA_Y1_UI			0x02
// #define REG_ACCEL_DATA_Y0_UI			0x03
// #define REG_ACCEL_DATA_Z1_UI			0x04
// #define REG_ACCEL_DATA_Z0_UI			0x05
// #define REG_GYRO_DATA_X1_UI			0x06
// #define REG_GYRO_DATA_X0_UI			0x07
// #define REG_GYRO_DATA_Y1_UI			0x08
// #define REG_GYRO_DATA_Y0_UI			0x09
// #define REG_GYRO_DATA_Z1_UI			0x0A
// #define REG_GYRO_DATA_Z0_UI			0x0B
// #define REG_TEMP_DATA1_UI			0x0C
// #define REG_TEMP_DATA0_UI			0x0D
// #define REG_PWR_MGMT0				0x10
// #define REG_FIFO_COUNT_0			0x12
// #define REG_FIFO_COUNT_1			0x13
// #define REG_FIFO_DATA				0x14
// #define REG_INT1_CONFIG0			0x16
// #define REG_INT1_CONFIG1			0x17
// #define REG_INT1_CONFIG2			0x18
// #define REG_INT1_STATUS0			0x19
// #define REG_INT1_STATUS1			0x1A
// #define REG_ACCEL_CONFIG0			0x1B
// #define REG_GYRO_CONFIG0			0x1C
// #define REG_FIFO_CONFIG0			0x1D
// #define REG_FIFO_CONFIG1_0			0x1E
// #define REG_FIFO_CONFIG1_1			0x1F
// #define REG_FIFO_CONFIG2			0x20
// #define REG_FIFO_CONFIG3			0x21
// #define REG_FIFO_CONFIG4			0x22
// #define REG_DRIVE_CONFIG0			0x32
// #define REG_WHO_AM_I				0x72
// #define REG_IREG_ADDR_15_8			0x7C
// #define REG_IREG_ADDR_7_0			0x7D
// #define REG_IREG_DATA				0x7E
// #define REG_MISC2				0x7F









#define REG_PWR_MGMT0_ACCEL_MODE(val)			((val) & BIT_MASK(2))
#define REG_PWR_MGMT0_GYRO_MODE(val)			(((val) & BIT_MASK(2)) << 2)

#define REG_ACCEL_CONFIG0_ODR(val)			((val) & BIT_MASK(4))
#define REG_ACCEL_CONFIG0_FS(val)			(((val) & BIT_MASK(3)) << 4)

#define REG_GYRO_CONFIG0_ODR(val)			((val) & BIT_MASK(4))
#define REG_GYRO_CONFIG0_FS(val)			(((val) & BIT_MASK(4)) << 4)

#define REG_DRIVE_CONFIG0_SPI_SLEW(val)			(((val) & BIT_MASK(2)) << 1)

#define REG_MISC2_SOFT_RST(val)				((val << 1) & BIT(1))

#define REG_IPREG_SYS1_REG_172_GYRO_LPFBW_SEL(val)	(val & BIT_MASK(3))

#define REG_IPREG_SYS2_REG_131_ACCEL_LPFBW_SEL(val)	(val & BIT_MASK(3))

#define REG_INT1_CONFIG0_STATUS_EN_DRDY(val)		(((val) & BIT_MASK(1)) << 2)
#define REG_INT1_CONFIG0_STATUS_EN_FIFO_THS(val)	(((val) & BIT_MASK(1)) << 1)
#define REG_INT1_CONFIG0_STATUS_EN_FIFO_FULL(val)	((val) & BIT_MASK(1))

#define REG_INT1_CONFIG2_EN_OPEN_DRAIN(val)		(((val) & BIT_MASK(1)) << 2)
#define REG_INT1_CONFIG2_EN_LATCH_MODE(val)		(((val) & BIT_MASK(1)) << 1)
#define REG_INT1_CONFIG2_EN_ACTIVE_HIGH(val)		((val) & BIT_MASK(1))

#define REG_INT1_STATUS0_DRDY(val)			(((val) & BIT_MASK(1)) << 2)
#define REG_INT1_STATUS0_FIFO_THS(val)			(((val) & BIT_MASK(1)) << 1)
#define REG_INT1_STATUS0_FIFO_FULL(val)			((val) & BIT_MASK(1))

#define REG_FIFO_CONFIG0_FIFO_MODE_BYPASS		0
#define REG_FIFO_CONFIG0_FIFO_MODE_STREAM		1
#define REG_FIFO_CONFIG0_FIFO_MODE_STOP_ON_FULL		2

#define REG_FIFO_CONFIG0_FIFO_DEPTH_2K			0x07
#define REG_FIFO_CONFIG0_FIFO_DEPTH_8K			0x1F

#define REG_FIFO_CONFIG0_FIFO_MODE(val)			(((val) & BIT_MASK(2)) << 6)
#define REG_FIFO_CONFIG0_FIFO_DEPTH(val)		((val) & BIT_MASK(6))

#define REG_FIFO_CONFIG1_0_FIFO_WM_THS(val)		((val) & BIT_MASK(8))
#define REG_FIFO_CONFIG1_1_FIFO_WM_THS(val)		(((val) >> 8) & BIT_MASK(8))

#define REG_FIFO_CONFIG2_FIFO_FLUSH(val)		(((val) & BIT_MASK(1)) << 7)
#define REG_FIFO_CONFIG2_FIFO_WM_GT_THS(val)		(((val) & BIT_MASK(1)) << 3)

#define REG_FIFO_CONFIG3_FIFO_HIRES_EN(val)		(((val) & BIT_MASK(1)) << 3)
#define REG_FIFO_CONFIG3_FIFO_GYRO_EN(val)		(((val) & BIT_MASK(1)) << 2)
#define REG_FIFO_CONFIG3_FIFO_ACCEL_EN(val)		(((val) & BIT_MASK(1)) << 1)
#define REG_FIFO_CONFIG3_FIFO_EN(val)			((val) & BIT_MASK(1))

/* Misc. Defines */
#define REG_IREG_PREPARE_WRITE_ARRAY(base, reg, val)	{((base) >> 8) & 0xFF, reg, val}

#define FIFO_HEADER_EXT_HEADER_EN(val)			(((val) & BIT_MASK(1)) << 7)
#define FIFO_HEADER_ACCEL_EN(val)			(((val) & BIT_MASK(1)) << 6)
#define FIFO_HEADER_GYRO_EN(val)			(((val) & BIT_MASK(1)) << 5)
#define FIFO_HEADER_HIRES_EN(val)			(((val) & BIT_MASK(1)) << 4)

#define FIFO_NO_DATA					0x8000
#define FIFO_COUNT_MAX_HIGH_RES				104

#endif /* ZEPHYR_DRIVERS_SENSOR_MAX30001_REG_H_ */