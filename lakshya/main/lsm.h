
/**
 * @file lsm.h
 * @brief Implementation of the LSM6DSO32 driver.
 */

#ifndef LSM_H
#define LSM_H

enum LSMPowerMode {
  LSM_MODE_SLEEP_MODE   = 0x00,
  LSM_MODE_FORCED_MODE  = 0x01,
  LSM_MODE_NORMAL_MODE  = 0x03,
};

enum LSMAccelerometerODR {
  LSM_ODR_12P5_HZ   = 0x01,
  LSM_ODR_26_HZ     = 0x02,
  LSM_ODR_52_HZ     = 0x03,
  LSM_ODR_104_HZ    = 0x04,
  LSM_ODR_208_HZ    = 0x05,
  LSM_ODR_416_HZ    = 0x06,
  LSM_ODR_833_HZ    = 0x07,
  LSM_ODR_1660_HZ   = 0x08,
  LSM_ODR_3330_HZ   = 0x09,
  LSM_ODR_6660_HZ   = 0x0A,
};

enum LSMAccelerometerFS {
  LSM_FS_2G   = 0x00,
  LSM_FS_4G   = 0x01,
  LSM_FS_8G   = 0x02,
  LSM_FS_16G  = 0x03,
  LSM_FS_32G  = 0x04,
};

enum LSMAccelerometerLPF {
  LSM_LPF_DISABLED = 0x00,
  LSM_LPF_50_HZ    = 0x01,
  LSM_LPF_100_HZ   = 0x02,
  LSM_LPF_200_HZ   = 0x03,
};

enum LSMAccelerometerBW {
  LSM_BW_400_HZ = 0x00,
  LSM_BW_200_HZ = 0x01,
  LSM_BW_100_HZ = 0x02,
  LSM_BW_50_HZ  = 0x03,
};

enum LSMError {
  LSM_SUCCESS         = 0x00,
  LSM_ERROR_INTERFACE = 0x01,
  LSM_ERROR_READ      = 0x02,
  LSM_ERROR_WRITE     = 0x03,
};

class SPIClass;

class LSM6DSO32 {
public:
  LSM6DSO32(uint8_t chipSelect);

  uint8_t readAccelerometer();
  uint8_t readGyroscope();
  uint8_t readTemperature();

  LSMError setPowerMode(LSMPowerMode mode);
  LSMError setAccelerometerODR(LSMAccelerometerODR odr);
  LSMError setAccelerometerFS(LSMAccelerometerFS fs);
  LSMError setAccelerometerLPF(LSMAccelerometerLPF lpf);
  LSMError setAccelerometerBW(LSMAccelerometerBW bw);

  bool isDataReady();

  LSMPowerMode getPowerMode();
  LSMAccelerometerODR getAccelerometerODR();
  LSMAccelerometerFS getAccelerometerFS();
  LSMAccelerometerLPF getAccelerometerLPF();
  LSMAccelerometerBW getAccelerometerBW();

private:
  uint8_t writeRegister(uint8_t reg, uint8_t value);
  uint8_t readRegister(uint8_t reg);

  void initInterface(SPIClass *spi);

private:
  SPIClass *_spi;
  uint8_t _chipSelect;
};

#endif /* LSM_H */