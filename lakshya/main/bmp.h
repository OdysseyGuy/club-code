#ifndef BMP_H
#define BMP_H

#define BMP3_MIN_PRES_INT     UINT64_C(3000000)
#define BMP3_MAX_PRES_INT     UINT64_C(12500000)
#define BMP3_MIN_PRES_DOUBLE  30000.0f
#define BMP3_MAX_PRES_DOUBLE  125000.0f

#define BMP3_MIN_TEMP_INT     INT64_C(-4000)
#define BMP3_MAX_TEMP_INT     INT64_C(8500)
#define BMP3_MIN_TEMP_DOUBLE  -40.0f
#define BMP3_MAX_TEMP_DOUBLE  85.0f

#if defined(ARDUINO_TEENSY41)
  /* enable floating point compensation for teensy4.1 */
  #define FLOAT_COMPENSATION
#endif

typedef enum bmp_power_mode {
  BMP_MODE_SLEEP_MODE   = 0x00,
  BMP_MODE_FORCED_MODE  = 0x01,
  BMP_MODE_NORMAL_MODE  = 0x03,
} bmp_power_mode_t;

typedef enum bmp_oversampling {
  BMP_OVERSAMPLING_x1   = 0x00,
  BMP_OVERSAMPLING_x2   = 0x01,
  BMP_OVERSAMPLING_x4   = 0x02,
  BMP_OVERSAMPLING_x8   = 0x03,
  BMP_OVERSAMPLING_x16  = 0x04,
  BMP_OVERSAMPLING_x32  = 0x05,
} bmp_oversampling_t;

typedef enum bmp_error {
  BMP_SUCCESS           = 0x00,
  BMP_ERROR_INTERFACE   = 0x01,
  BMP_ERROR_READ        = 0x02,
  BMP_ERROR_WRITE       = 0x03,
  BMP_INVALID_CHIP_ID   = 0x04,
} bmp_error_t;

typedef enum bmp_odr {
  BMP_ODR_200_HZ        = 0x00,
  BMP_ODR_100_HZ        = 0x01,
  BMP_ODR_50_HZ         = 0x02,
  BMP_ODR_25_HZ         = 0x03,
  BMP_ODR_12P5_HZ       = 0x04,
  BMP_ODR_6P25_HZ       = 0x05,
  BMP_ODR_3P1_HZ        = 0x06,
  BMP_ODR_1P5_HZ        = 0x07,
  BMP_ODR_0P78_HZ       = 0x08,
  BMP_ODR_0P39_HZ       = 0x09, 
  BMP_ODR_0P2_HZ        = 0x0A, 
  BMP_ODR_0P1_HZ        = 0x0B, 
  BMP_ODR_0P05_HZ       = 0x0C,
  BMP_ODR_0P02_HZ       = 0x0D,
  BMP_ODR_0P01_HZ       = 0x0E,
  BMP_ODR_0P006_HZ      = 0x0F,
  BMP_ODR_0P003_HZ      = 0x10,
  BMP_ODR_0P0015_HZ     = 0x11, 
} bmp_odr_t;

typedef struct bmp_reg_calib_data {
  uint16_t  par_t1;
  uint16_t  par_t2;
  int8_t    par_t3;
  int16_t   par_p1;
  int16_t   par_p2;
  int8_t    par_p3;
  int8_t    par_p4;
  uint16_t  par_p5;
  uint16_t  par_p6;
  int8_t    par_p7;
  int8_t    par_p8;
  int16_t   par_p9;
  int8_t    par_p10;
  int8_t    par_p11;
  int64_t   t_lin;
} bmp_reg_calib_data_t;

typedef struct bmp_quantized_calib_data {
  double    par_t1;
  double    par_t2;
  double    par_t3;
  double    par_p1;
  double    par_p2;
  double    par_p3;
  double    par_p4;
  double    par_p5;
  double    par_p6;
  double    par_p7;
  double    par_p8;
  double    par_p9;
  double    par_p10;
  double    par_p11;
  double    t_lin;
} bmp_quantized_calib_data_t;

typedef struct bmp_calib_data {
#ifdef FLOAT_COMPENSATION
  /* TODO: do we really need to store the reg data? */
  bmp_reg_calib_data_t        reg_calib_data;
  bmp_quantized_calib_data_t  quantized_calib_data;
#else
  bmp_reg_calib_data_t        reg_calib_data;
#endif
} bmp_calib_data_t;

typedef struct bmp_device {
  uint8_t           chip_select;
  SPIClass         *spi;
  bool              initialized;
  bmp_calib_data_t  calib_data;
} bmp_device_t;


bmp_error_t bmp_init(bmp_device_t *bmp, SPIClass *spi, uint8_t chip_select);
bmp_error_t bmp_deinit(bmp_device_t *bmp);
bmp_error_t bmp_softreset(bmp_device_t *bmp);
bmp_error_t bmp_get_status(bmp_device_t *bmp, uint8_t *status);
bmp_error_t bmp_read_pressure(bmp_device_t *bmp, float *pressure);
bmp_error_t bmp_read_temperature(bmp_device_t *bmp, float *temperature);
bmp_error_t bmp_pressure_to_altitude(bmp_device_t *bmp, float sea_level, float *altitude);

bool        bmp_is_data_ready(bmp_device_t *bmp);
bmp_error_t bmp_get_pressure_oversampling(bmp_device_t *bmp, bmp_oversampling_t *oversampling);
bmp_error_t bmp_get_temperature_oversampling(bmp_device_t *bmp, bmp_oversampling_t *oversampling);
bmp_error_t bmp_get_odr(bmp_device_t *bmp, bmp_odr_t *odr);
bmp_error_t bmp_get_power_mode(bmp_device_t *bmp, bmp_power_mode_t *mode);

bmp_error_t bmp_set_pressure_oversampling(bmp_device_t *bmp, bmp_oversampling_t oversampling);
bmp_error_t bmp_set_temperature_oversampling(bmp_device_t *bmp, bmp_oversampling_t oversampling);
bmp_error_t bmp_set_odr(bmp_device_t *bmp, bmp_odr_t odr);
bmp_error_t bmp_set_power_mode(bmp_device_t *bmp, bmp_power_mode_t mode);

uint8_t     bmp_read_register(bmp_device_t *bmp, uint8_t reg);
void        bmp_read_register(bmp_device_t *bmp, uint8_t reg, uint8_t len, uint8_t *data);
uint8_t     bmp_write_register(bmp_device_t *bmp, uint8_t reg, uint8_t val);

#endif
