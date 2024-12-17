#include "SPI.h"
#include "bmp.h"
#include "log.h"

#define BMP_REG_CONFIG    0x1F
#define BMP_REG_ODR       0x1D
#define BMP_REG_OSR       0x1C
#define BMP_REG_PWR_CTRL  0x1B
#define BMP_REG_IF_CONF   0x1A
#define BMP_REG_DATA_5    0x09
#define BMP_REG_DATA_4    0x08
#define BMP_REG_DATA_3    0x07
#define BMP_REG_DATA_2    0x06
#define BMP_REG_DATA_1    0x05
#define BMP_REG_DATA_0    0x04

BMPSensor::BMPSensor(uint8_t chipSelect)
{
  _chipSelect = chipSelect;
  _spi = nullptr;
}

uint8_t BMPSensor::readPressure()
{
  // uint8_t value = readRegister();
}

uint8_t BMPSensor::readTemperature()
{
  // uint8_t value = readRegister();
}

uint8_t BMPSensor::pressureToAltitude()
{
}

bool BMPSensor::isDataReady()
{
  return false;
}

BMPOversampling BMPSensor::getPressureOversampling()
{
  return BMPOversampling();
}

BMPOversampling BMPSensor::getTemperatureOversampling()
{
  return BMPOversampling();
}

BMPODR BMPSensor::getODR()
{
  return BMPODR();
}

BMPPowerMode BMPSensor::getPowerMode()
{
  return BMPPowerMode();
}

BMPError BMPSensor::setPressureOversampling(BMPOversampling oversampling)
{
  if (_spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = readRegister(BMP_REG_OSR);
  prev &= ~(7 << 0);
  prev |= oversampling << 0;
  uint8_t res = writeRegister(BMP_REG_OSR, prev);
  if (res != 0) {
    LOG_ERROR("Failed to write Pressure oversampling!");
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

BMPError BMPSensor::setTemperatureOversampling(BMPOversampling oversampling)
{
  if (_spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = readRegister(BMP_REG_OSR);
  prev &= ~(7 << 3);
  prev |= oversampling << 3;
  uint8_t res = writeRegister(BMP_REG_OSR, prev);
  if (res != 0) {
    LOG_ERROR("Failed to write Pressure oversampling!");
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

BMPError BMPSensor::setODR(BMPODR odr)
{
  if (_spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = readRegister(BMP_REG_ODR);
  prev &= ~(31 << 0);
  prev |= odr << 0;
  uint8_t res = writeRegister(BMP_REG_ODR, prev);
  if (res != 0) {
    LOG_ERROR("Failed to write Pressure oversampling!");
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

BMPError BMPSensor::setPowerMode(BMPPowerMode mode)
{
  if (_spi == nullptr) {
    return BMP_ERROR_INTERFACE;
  }
  uint8_t prev = readRegister(BMP_REG_PWR_CTRL);
  prev &= ~(7 << 0);
  prev |= mode;
  uint8_t res = writeRegister(BMP_REG_PWR_CTRL, prev);
  if (res != 0) {
    LOG_ERROR("Failed to write Pressure oversampling!");
    return BMP_ERROR_WRITE;
  } else {
    return BMP_SUCCESS;
  }
}

uint8_t BMPSensor::writeRegister(uint8_t reg, uint8_t val)
{
  uint8_t result = 0;
  // MSB 0: Write 1: Write
  uint8_t write_addr = reg & 0x7f;
  _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_chipSelect, LOW);
  _spi->transfer(write_addr);
  _spi->transfer(val);
  digitalWrite(_chipSelect, HIGH);
  _spi->endTransaction();
  return result;
}

uint8_t BMPSensor::readRegister(uint8_t reg)
{
  uint8_t result = 0;
  uint8_t read_addr = reg | 0x80;
  _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_chipSelect, LOW);
  _spi->transfer(read_addr);
  result = _spi->transfer(0x00);
  digitalWrite(_chipSelect, HIGH);
  _spi->endTransaction();
  return result;
}

void BMPSensor::initInterface(SPIClass *spi)
{
  _spi = spi;
}
