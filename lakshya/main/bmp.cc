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

void BMPSensor::init(uint8_t chipSelect, BMPOversampling pos, BMPOversampling tos)
{
  _chipSelect = chipSelect;
  if (_spi == nullptr) {
    LOG_ERROR("BMP SPI Interface is not initialized properly.");
  }
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

BMPOversampling BMPSensor::getPressureOversampling()
{
  return BMPOversampling();
}

BMPOversampling BMPSensor::getTemperatureOversampling()
{
  return BMPOversampling();
}

BMPOutputDataRate BMPSensor::getOutputDataRate()
{
  return BMPOutputDataRate();
}

BMPPowerMode BMPSensor::getPowerMode()
{
  return BMPPowerMode();
}

void BMPSensor::setPressureOversampling(BMPOversampling oversampling)
{
  if (_spi == nullptr) {
    LOG_ERROR("Missing SPI Handle in BMPSensor::setPressureOversampling");
  }
  uint8_t prev = readRegister(BMP_REG_OSR);
  prev &= ~(7 << 0);
  prev |= oversampling << 0;
  uint8_t res = writeRegister(BMP_REG_OSR, prev);
  if (res != 0) {
    LOG_ERROR("Failed to write Pressure oversampling!");
  }
}

void BMPSensor::setTemperatureOversampling(BMPOversampling oversampling)
{
  if (_spi == nullptr) {
    LOG_ERROR("Missing SPI Handle in BMPSensor::setTemperatureOversampling");
  }
  uint8_t prev = readRegister(BMP_REG_OSR);
  prev &= ~(7 << 3);
  prev |= oversampling << 3;
  uint8_t res = writeRegister(BMP_REG_OSR, prev);
  if (res != 0) {
    LOG_ERROR("Failed to write Pressure oversampling!");
  }
}

void BMPSensor::setOutputDataRate(BMPOutputDataRate odr)
{
  if (_spi == nullptr) {
    LOG_ERROR("Missing SPI Handle in BMPSensor::setOutputDataRate");
  }
  uint8_t prev = readRegister(BMP_REG_ODR);
  prev &= ~(31 << 0);
  prev |= odr << 0;
  uint8_t res = writeRegister(BMP_REG_ODR, prev);
  if (res != 0) {
    LOG_ERROR("Failed to write Pressure oversampling!");
  }
}

void BMPSensor::setPowerMode(BMPPowerMode mode)
{
  if (_spi == nullptr) {
    LOG_ERROR("Missing SPI Handle in BMPSensor::setPowerMode");
  }
  uint8_t prev = readRegister(BMP_REG_PWR_CTRL);
  prev &= ~(7 << 0);
  prev |= mode;
  uint8_t res = writeRegister(BMP_REG_PWR_CTRL, prev);
  if (res != 0) {
    LOG_ERROR("Failed to write Pressure oversampling!");
  }
}

uint8_t BMPSensor::writeRegister(uint8_t reg, uint8_t val)
{
  uint8_t result = 0;
  uint8_t write_addr = reg & 0x7f; // MSB 0: Write 1: Write
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
