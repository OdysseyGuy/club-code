#ifndef BMP_H
#define BMP_H

enum BMPPowerMode
{
  BMP_MODE_SLEEP_MODE   = 0x00,
  BMP_MODE_FORCED_MODE  = 0x01,
  BMP_MODE_NORMAL_MODE  = 0x03,
};

enum BMPOversampling
{
  BMP_OVERSAMPLING_x1   = 0x00,
  BMP_OVERSAMPLING_x2   = 0x01,
  BMP_OVERSAMPLING_x4   = 0x02,
  BMP_OVERSAMPLING_x8   = 0x03,
  BMP_OVERSAMPLING_x16  = 0x04,
  BMP_OVERSAMPLING_x32  = 0x05,
};

enum BMPOutputDataRate
{
  BMP_ODR_200_HZ    = 0x00,
  BMP_ODR_100_HZ    = 0x01,
  BMP_ODR_50_HZ     = 0x02,
  BMP_ODR_25_HZ     = 0x03,
  BMP_ODR_12P5_HZ   = 0x04,
  BMP_ODR_6P25_HZ   = 0x05,
  BMP_ODR_3P1_HZ    = 0x06,
  BMP_ODR_1P5_HZ    = 0x07,
  BMP_ODR_0P78_HZ   = 0x08,
  BMP_ODR_0P39_HZ   = 0x09, 
  BMP_ODR_0P2_HZ    = 0x0A, 
  BMP_ODR_0P1_HZ    = 0x0B, 
  BMP_ODR_0P05_HZ   = 0x0C,
  BMP_ODR_0P02_HZ   = 0x0D,
  BMP_ODR_0P01_HZ   = 0x0E,
  BMP_ODR_0P006_HZ  = 0x0F,
  BMP_ODR_0P003_HZ  = 0x10,
  BMP_ODR_0P0015_HZ = 0x11, 
};

class SPIClass;
class BMPSensor
{
public:
  void init(uint8_t chipSelect, BMPOversampling pos, BMPOversampling tos);

  uint8_t readPressure();
  uint8_t readTemperature();
  uint8_t pressureToAltitude();

  BMPOversampling getPressureOversampling();
  BMPOversampling getTemperatureOversampling();
  BMPOutputDataRate getOutputDataRate();
  BMPPowerMode getPowerMode();

  void setPressureOversampling(BMPOversampling oversampling);
  void setTemperatureOversampling(BMPOversampling oversampling);
  void setOutputDataRate(BMPOutputDataRate odr);
  void setPowerMode(BMPPowerMode mode);

private:
  uint8_t writeRegister(uint8_t reg, uint8_t val);
  uint8_t readRegister(uint8_t reg);

  void initInterface(SPIClass *spi);

private:
  SPIClass *_spi;
  uint8_t _chipSelect;
};

#endif
