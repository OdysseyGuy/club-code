#include "SPI.h"
#include "bmp.h"

#define BMP390_REG_CMD          0x7E
#define BMP390_REG_NVM_PAR_T1_L 0x31
#define BMP390_REG_NVM_PAR_T1_H 0x32
#define BMP390_REG_NVM_PAR_T2_L 0x33
#define BMP390_REG_NVM_PAR_T2_H 0x34
#define BMP390_REG_NVM_PAR_T3   0x35
#define BMP390_REG_NVM_PAR_P1_L 0x36
#define BMP390_REG_NVM_PAR_P1_H 0x37
#define BMP390_REG_NVM_PAR_P2_L 0x38
#define BMP390_REG_NVM_PAR_P2_H 0x39
#define BMP390_REG_NVM_PAR_P3   0x3A
#define BMP390_REG_NVM_PAR_P4   0x3B
#define BMP390_REG_NVM_PAR_P5_L 0x3C
#define BMP390_REG_NVM_PAR_P5_H 0x3D
#define BMP390_REG_NVM_PAR_P6_L 0x3E
#define BMP390_REG_NVM_PAR_P6_H 0x3F
#define BMP390_REG_NVM_PAR_P7   0x40
#define BMP390_REG_NVM_PAR_P8   0x41
#define BMP390_REG_NVM_PAR_P9_L 0x42
#define BMP390_REG_NVM_PAR_P9_H 0x43
#define BMP390_REG_NVM_PAR_P10  0x44
#define BMP390_REG_NVM_PAR_P11  0x45
#define BMP_REG_CONFIG          0x1F
#define BMP_REG_ODR             0x1D
#define BMP_REG_OSR             0x1C
#define BMP_REG_PWR_CTRL        0x1B
#define BMP_REG_IF_CONF         0x1A
#define BMP_REG_DATA_5          0x09
#define BMP_REG_DATA_4          0x08
#define BMP_REG_DATA_3          0x07
#define BMP_REG_DATA_2          0x06
#define BMP_REG_DATA_1          0x05
#define BMP_REG_DATA_0          0x04
#define BMP_REG_STATUS          0x00
#define BMP_REG_CHIP_ID         0x00

bmp_error_t bmp_init(bmp_device_t *bmp, SPIClass *spi, uint8_t chip_select) {
  bmp->spi = spi;
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  bmp->chip_select = chip_select;
  
  /* verify the chip id */
  uint8_t chip_id = bmp_read_register(bmp, BMP_REG_CHIP_ID);
  if (chip_id != 0x50) {
    return BMP_INVALID_CHIP_ID;
  }

  /* read calibration data */
  bmp_get_calibration_data(bmp);

  /* set the power mode to normal */
  bmp_set_power_mode(bmp, BMP_MODE_NORMAL_MODE);

  /* enable pressure and tempearature measurement */
  bmp_write_register(bmp, BMP_REG_PWR_CTRL, 0x03);

  bmp->initialized = true;
  return BMP_SUCCESS;
}

bmp_error_t bmp_deinit(bmp_device_t *bmp) {
  return bmp_error_t();
}

bmp_error_t bmp_softreset(bmp_device_t *bmp) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t res = bmp_write_register(bmp, BMP390_REG_CMD, 0xB6);
  if (res != 0) {
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

bmp_error_t bmp_get_status(bmp_device_t *bmp, uint8_t *status) {
  return bmp_error_t();
}

bmp_error_t bmp_read_pressure(bmp_device_t *bmp, float *pressure) {
  return bmp_error_t();
}

bmp_error_t bmp_read_temperature(bmp_device_t *bmp, float *temperature) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
}

bmp_error_t bmp_pressure_to_altitude(bmp_device_t *bmp, float sea_level, float *altitude) {
  float atmospheric;
  bmp_error_t err = bmp_read_pressure(bmp, &atmospheric);
  atmospheric = atmospheric / 100.0f;
  *altitude = 44330.0 * (1.0 - pow(atmospheric / sea_level, 0.1903));
  return err;
}

bool bmp_is_data_ready(bmp_device_t *bmp) {
  return false;
}

bmp_error_t bmp_get_pressure_oversampling(bmp_device_t *bmp, bmp_oversampling_t *oversampling) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = bmp_read_register(bmp, BMP_REG_OSR);
  *oversampling = (bmp_oversampling_t)((prev >> 0) & 0x07);
  return BMP_SUCCESS;
}

bmp_error_t bmp_get_temperature_oversampling(bmp_device_t *bmp, bmp_oversampling_t *oversampling) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = bmp_read_register(bmp, BMP_REG_OSR);
  *oversampling = (bmp_oversampling_t)((prev >> 3) & 0x07);
  return BMP_SUCCESS;
}

bmp_error_t bmp_get_odr(bmp_device_t *bmp, bmp_odr_t *odr) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = bmp_read_register(bmp, BMP_REG_ODR);
  *odr = (bmp_odr_t)((prev >> 0) & 31);
  return BMP_SUCCESS;
}

bmp_error_t bmp_get_power_mode(bmp_device_t *bmp, bmp_power_mode_t *mode) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = bmp_read_register(bmp, BMP_REG_PWR_CTRL);
  *mode = (bmp_power_mode_t)((prev >> 4) & 0x03);
  return BMP_SUCCESS;
}

bmp_error_t bmp_set_pressure_oversampling(bmp_device_t *bmp, bmp_oversampling_t oversampling) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = bmp_read_register(bmp, BMP_REG_OSR);
  prev &= ~(7 << 0);
  prev |= oversampling << 0;
  uint8_t res = bmp_write_register(bmp, BMP_REG_OSR, prev);
  if (res != 0) {
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

bmp_error_t bmp_set_temperature_oversampling(bmp_device_t *bmp, bmp_oversampling_t oversampling) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = bmp_read_register(bmp, BMP_REG_OSR);
  prev &= ~(7 << 3);
  prev |= oversampling << 3;
  uint8_t res = bmp_write_register(bmp, BMP_REG_OSR, prev);
  if (res != 0) {
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

bmp_error_t bmp_set_odr(bmp_device_t *bmp, bmp_odr_t odr) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = bmp_read_register(bmp, BMP_REG_ODR);
  prev &= ~(31 << 0);
  prev |= odr << 0;
  uint8_t res = bmp_write_register(bmp, BMP_REG_ODR, prev);
  if (res != 0) {
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

bmp_error_t bmp_set_power_mode(bmp_device_t *bmp, bmp_power_mode_t mode) {
  if (bmp->spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = bmp_read_register(bmp, BMP_REG_PWR_CTRL);
  prev &= ~(7 << 0);
  prev |= mode;
  uint8_t res = bmp_write_register(bmp, BMP_REG_PWR_CTRL, prev);
  if (res != 0) {
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

static void bmp_get_calibration_data(bmp_device_t *bmp) {
  uint8_t reg_data[21] = {0};
  bmp_read_register(bmp, BMP390_REG_NVM_PAR_T1_L, 21, reg_data);

#ifdef FLOAT_COMPENSATION
  bmp_reg_calib_data_t *reg_calib_data = &bmp->calib_data.reg_calib_data;
  bmp_quantized_calib_data_t *quantized_calib_data = &bmp->calib_data.quantized_calib_data;

  double temp_var = 0.00390625f;
  reg_calib_data->par_t1 = (((uint16_t)reg_data[1] << 8) | (uint16_t)reg_data[0]);
  quantized_calib_data->par_t1 = ((double)reg_calib_data->par_t1 / temp_var);
  reg_calib_data->par_t2 = (((uint16_t)reg_data[3] << 8) | (uint16_t)reg_data[2]);
  temp_var = 1073741824.0f;
  quantized_calib_data->par_t2 = ((double)reg_calib_data->par_t2 / temp_var);
  reg_calib_data->par_t3 = (int8_t)reg_data[4];
  temp_var = 281474976710656.0f;
  quantized_calib_data->par_t3 = ((double)reg_calib_data->par_t3 / temp_var);
  reg_calib_data->par_p1 = (int16_t)(((uint16_t)reg_data[6] << 8) | (uint16_t)reg_data[5]);
  temp_var = 1048576.0f;
  quantized_calib_data->par_p1 = ((double)(reg_calib_data->par_p1 - (16384)) / temp_var);
  reg_calib_data->par_p2 = (int16_t)(((uint16_t)reg_data[8] << 8) | (uint16_t)reg_data[7]);
  temp_var = 536870912.0f;
  quantized_calib_data->par_p2 = ((double)(reg_calib_data->par_p2 - (16384)) / temp_var);
  reg_calib_data->par_p3 = (int8_t)reg_data[9];
  temp_var = 4294967296.0f;
  quantized_calib_data->par_p3 = ((double)reg_calib_data->par_p3 / temp_var);
  reg_calib_data->par_p4 = (int8_t)reg_data[10];
  temp_var = 137438953472.0f;
  quantized_calib_data->par_p4 = ((double)reg_calib_data->par_p4 / temp_var);
  reg_calib_data->par_p5 = (((uint16_t)reg_data[12] << 8) | (uint16_t)reg_data[11]);

  temp_var = 0.125f;
  quantized_calib_data->par_p5 = ((double)reg_calib_data->par_p5 / temp_var);
  reg_calib_data->par_p6 = (((uint16_t)reg_data[14] << 8) | (uint16_t)reg_data[13]);
  temp_var = 64.0f;
  quantized_calib_data->par_p6 = ((double)reg_calib_data->par_p6 / temp_var);
  reg_calib_data->par_p7 = (int8_t)reg_data[15];
  temp_var = 256.0f;
  quantized_calib_data->par_p7 = ((double)reg_calib_data->par_p7 / temp_var);
  reg_calib_data->par_p8 = (int8_t)reg_data[16];
  temp_var = 32768.0f;
  quantized_calib_data->par_p8 = ((double)reg_calib_data->par_p8 / temp_var);
  reg_calib_data->par_p9 = (int16_t)(((uint16_t)reg_data[18] << 8) | (uint16_t)reg_data[17]);
  temp_var = 281474976710656.0f;
  quantized_calib_data->par_p9 = ((double)reg_calib_data->par_p9 / temp_var);
  reg_calib_data->par_p10 = (int8_t)reg_data[19];
  temp_var = 281474976710656.0f;
  quantized_calib_data->par_p10 = ((double)reg_calib_data->par_p10 / temp_var);
  reg_calib_data->par_p11 = (int8_t)reg_data[20];
  temp_var = 36893488147419103232.0f;
  quantized_calib_data->par_p11 = ((double)reg_calib_data->par_p11 / temp_var);
#else
  bmp_reg_calib_data_t *reg_calib_data = &bmp->calib_data.reg_calib_data;
  reg_calib_data->par_t1 = (((uint16_t)reg_data[1] << 8) | (uint16_t)reg_data[0]);
  reg_calib_data->par_t2 = (((uint16_t)reg_data[3] << 8) | (uint16_t)reg_data[2]);
  reg_calib_data->par_t3 = (int8_t)reg_data[4];
  reg_calib_data->par_p1 = (int16_t)(((uint16_t)reg_data[6] << 8) | (uint16_t)reg_data[5]);
  reg_calib_data->par_p2 = (int16_t)(((uint16_t)reg_data[8] << 8) | (uint16_t)reg_data[7]);
  reg_calib_data->par_p3 = (int8_t)reg_data[9];
  reg_calib_data->par_p4 = (int8_t)reg_data[10];
  reg_calib_data->par_p5 = (((uint16_t)reg_data[12] << 8) | (uint16_t)reg_data[11]);
  reg_calib_data->par_p6 = (((uint16_t)reg_data[14] << 8) | (uint16_t)reg_data[13]);
  reg_calib_data->par_p7 = (int8_t)reg_data[15];
  reg_calib_data->par_p8 = (int8_t)reg_data[16]; 
  reg_calib_data->par_p9 = (int16_t)(((uint16_t)reg_data[18] << 8) | (uint16_t)reg_data[17]);
  reg_calib_data->par_p10 = (int8_t)reg_data[19];
  reg_calib_data->par_p11 = (int8_t)reg_data[20];
#endif
}

static
#ifdef FLOAT_COMPENSATION
double
#else
uint64_t
#endif
bmp_compensate_temperature(bmp_device_t *bmp, uint32_t data) {
#ifdef FLOAT_COMPENSATION
  bmp_quantized_calib_data *calib_data = &bmp->calib_data.quantized_calib_data;
  double uncomp_data = data;
  double partial_data1;
  double partial_data2;

  partial_data1 = (double)(uncomp_data - bmp->calib_data.quantized_calib_data.par_t1);
  partial_data2 = (double)(partial_data1 * bmp->calib_data.quantized_calib_data.par_t2);
  calib_data->t_lin = partial_data2 + (partial_data1 * partial_data1) * calib_data->par_t3;

  if (calib_data->t_lin < BMP3_MIN_TEMP_DOUBLE) {
    calib_data->t_lin = BMP3_MIN_TEMP_DOUBLE;
  }
  if (calib_data->t_lin > BMP3_MAX_TEMP_DOUBLE) {
    calib_data->t_lin = BMP3_MAX_TEMP_DOUBLE;
  }

  return calib_data->t_lin;
#else
  bmp_reg_calib_data_t *calib_data = &bmp->calib_data.reg_calib_data;

  uint64_t partial_data1;
  uint64_t partial_data2;
  uint64_t partial_data3;
  int64_t partial_data4;
  int64_t partial_data5;
  int64_t partial_data6;
  int64_t comp_temp;

  partial_data1 = (uint64_t)(data - (256 * (uint64_t)(calib_data->par_t1)));
  partial_data2 = (uint64_t)(calib_data->par_t2 * partial_data1);
  partial_data3 = (uint64_t)(partial_data1 * partial_data1);
  partial_data4 = (int64_t)(((int64_t)partial_data3) * ((int64_t)calib_data->par_t3));
  partial_data5 = ((int64_t)(((int64_t)partial_data2) * 262144) + (int64_t)partial_data4);
  partial_data6 = (int64_t)(((int64_t)partial_data5) / 4294967296U);
  calib_data->t_lin = partial_data6;
  comp_temp = (int64_t)((partial_data6 * 25)  / 16384);

  return comp_temp;
#endif
}

static
#ifdef FLOAT_COMPENSATION
double
#else
uint64_t
#endif
bmp_compensate_pressure(bmp_device_t *bmp, uint32_t data) {
#ifdef FLOAT_COMPENSATION
  const bmp_quantized_calib_data *quantized_calib_data = &bmp->calib_data.quantized_calib_data;
  double uncomp_data = (double)data;
  double comp_press;

  double partial_data1;
  double partial_data2;
  double partial_data3;
  double partial_data4;
  double partial_out1;
  double partial_out2;

  partial_data1 = quantized_calib_data->par_p6 * quantized_calib_data->t_lin;
  partial_data2 = quantized_calib_data->par_p7 * quantized_calib_data->t_lin * quantized_calib_data->t_lin;
  partial_data3 = quantized_calib_data->par_p8 * quantized_calib_data->t_lin * quantized_calib_data->t_lin * quantized_calib_data->t_lin;
  partial_out1 = quantized_calib_data->par_p5 + partial_data1 + partial_data2 + partial_data3;
  partial_data1 = quantized_calib_data->par_p2 * quantized_calib_data->t_lin;
  partial_data2 = quantized_calib_data->par_p3 * quantized_calib_data->t_lin * quantized_calib_data->t_lin;
  partial_data3 = quantized_calib_data->par_p4 * quantized_calib_data->t_lin * quantized_calib_data->t_lin * quantized_calib_data->t_lin;
  partial_out2 = uncomp_data * (quantized_calib_data->par_p1 + partial_data1 + partial_data2 + partial_data3);
  partial_data1 = uncomp_data * (double)data;
  partial_data2 = quantized_calib_data->par_p9 + quantized_calib_data->par_p10 * quantized_calib_data->t_lin;
  partial_data3 = partial_data1 * partial_data2;
  partial_data4 = partial_data3 + uncomp_data * uncomp_data * uncomp_data * quantized_calib_data->par_p11;
  comp_press = partial_out1 + partial_out2 + partial_data4;

  if (comp_press < BMP3_MIN_PRES_DOUBLE) {
    comp_press = BMP3_MIN_PRES_DOUBLE;
  }
  if (comp_press > BMP3_MAX_PRES_DOUBLE) {
    comp_press = BMP3_MAX_PRES_DOUBLE;
  }

  return comp_press;
#else
  const bmp_reg_calib_data_t *reg_calib_data = &bmp->calib_data.reg_calib_data;
  int64_t partial_data1;
  int64_t partial_data2;
  int64_t partial_data3;
  int64_t partial_data4;
  int64_t partial_data5;
  int64_t partial_data6;
  int64_t offset;
  int64_t sensitivity;
  uint64_t comp_press;

  partial_data1 = (int64_t)(reg_calib_data->t_lin * reg_calib_data->t_lin);
  partial_data2 = (int64_t)(partial_data1 / 64);
  partial_data3 = (int64_t)((partial_data2 * reg_calib_data->t_lin) / 256);
  partial_data4 = (int64_t)((reg_calib_data->par_p8 * partial_data3) / 32);
  partial_data5 = (int64_t)((reg_calib_data->par_p7 * partial_data1) * 16);
  partial_data6 = (int64_t)((reg_calib_data->par_p6 * reg_calib_data->t_lin) * 4194304);
  offset = (int64_t)((reg_calib_data->par_p5 * 140737488355328) + partial_data4 + partial_data5 + partial_data6);
  partial_data2 = (int64_t)((reg_calib_data->par_p4 * partial_data3) / 32);
  partial_data4 = (int64_t)((reg_calib_data->par_p3 * partial_data1) * 4);
  partial_data5 = (int64_t)((reg_calib_data->par_p2 - (int32_t)16384) * reg_calib_data->t_lin * 2097152);
  sensitivity = (int64_t)(((reg_calib_data->par_p1 - (int32_t)16384) * 70368744177664) + partial_data2 + partial_data4 + partial_data5);
  partial_data1 = (int64_t)((sensitivity / 16777216) * data);
  partial_data2 = (int64_t)(reg_calib_data->par_p10 * reg_calib_data->t_lin);
  partial_data3 = (int64_t)(partial_data2 + ((int32_t)65536 * reg_calib_data->par_p9));
  partial_data4 = (int64_t)((partial_data3 * data) / (int32_t)8192);
  partial_data5 = (int64_t)((data * (partial_data4 / 10)) / (int32_t)512);
  partial_data5 = (int64_t)(partial_data5 * 10);
  partial_data6 = (int64_t)((uint64_t)data * (uint64_t)data);
  partial_data2 = (int64_t)((reg_calib_data->par_p11 * partial_data6) / (int32_t)65536);
  partial_data3 = (int64_t)((int64_t)(partial_data2 * data) / 128);
  partial_data4 = (int64_t)((offset / 4) + partial_data1 + partial_data5 + partial_data3);
  comp_press = (((uint64_t)partial_data4 * 25) / (uint64_t)1099511627776);
  
  if (comp_press < BMP3_MIN_PRES_INT) {
    comp_press = BMP3_MIN_PRES_INT;
  }
  if (comp_press > BMP3_MAX_PRES_INT) {
    comp_press = BMP3_MAX_PRES_INT;
  }

  return comp_press;
#endif
}

uint8_t bmp_read_register(bmp_device_t *bmp, uint8_t reg) {
  bmp->spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(bmp->chip_select, LOW);
  bmp->spi->transfer(reg | 0x80);
  uint8_t result = bmp->spi->transfer(0x00); 
  digitalWrite(bmp->chip_select, HIGH);
  bmp->spi->endTransaction();
  return result;
}

void bmp_read_register(bmp_device_t *bmp, uint8_t reg, uint8_t len, uint8_t *data) {
  bmp->spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(bmp->chip_select, LOW);
  bmp->spi->transfer(reg | 0x80);
  for (uint8_t i = 0; i < len; i++) {
    data[i] = bmp->spi->transfer(0x00);
  }
  digitalWrite(bmp->chip_select, HIGH);
  bmp->spi->endTransaction();
}

uint8_t bmp_write_register(bmp_device_t *bmp, uint8_t reg, uint8_t val) {
  uint8_t result = 0;
  bmp->spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(bmp->chip_select, LOW);
  bmp->spi->transfer(reg & 0x7f);
  bmp->spi->transfer(val);
  digitalWrite(bmp->chip_select, HIGH);
  bmp->spi->endTransaction();
  return result;
}
