#include "imu.hpp"
#include "dma.hpp"

bool initImu(SpiHandle &spi) {
    std::array<uint8_t, DMA_SETUP::SPI_NDTR_VAL> imuData{};

    spi.setCsHigh();
    vTaskDelay(pdMS_TO_TICKS(5));
    spi.setCsLow();
    vTaskDelay(pdMS_TO_TICKS(5));
    spi.setCsHigh();
    vTaskDelay(pdMS_TO_TICKS(5));

    spi.POLLread_write(LSM6DS3::sanityCheck.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, false);
    if (imuData[1] != 0x6A) {
        return false;
    }

    spi.POLLread_write(LSM6DS3::ctrl3_c_conf_array.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, true);
    spi.POLLread_write(LSM6DS3::ctrl1_xl_conf_array.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, true);
    spi.POLLread_write(LSM6DS3::ctrl2_g_conf_array.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, true);
    spi.POLLread_write(LSM6DS3::ctrl6_c_conf_array.data(), imuData.data(), LSM6DS3::SPI_CONFIG_SEQUENCE_LENGTH, true);

    return true;
}

void normalizeDataFromImu(ImuTelemetry& imu, ImuDataNormalized& imuDataOut) {
    imuDataOut.acc_y = (imu.acc_x * LSM6DS3::Sensivity::LINEAR) * (-1.0f);
    imuDataOut.acc_x = (imu.acc_y * LSM6DS3::Sensivity::LINEAR) * (-1.0f);
    imuDataOut.acc_z = (imu.acc_z * LSM6DS3::Sensivity::LINEAR) * (-1.0f);
    imuDataOut.gyro_y = (imu.gyro_x * LSM6DS3::Sensivity::ANGULAR) * (-1.0f);
    imuDataOut.gyro_x = (imu.gyro_y * LSM6DS3::Sensivity::ANGULAR) * (-1.0f);
    imuDataOut.gyro_z = (imu.gyro_z * LSM6DS3::Sensivity::ANGULAR) * (-1.0f);
}