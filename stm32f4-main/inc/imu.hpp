#ifndef IMU_HPP
#define IMU_HPP

#include <cstdint>
#include <array>

namespace LSM6DS3 {
    enum class LSM6DS3_regs : uint8_t {
        FUNC_CFG_ACCESS         = 0x01,
        SENSOR_SYNC_TIME_FRAME  = 0x04,
        SENSOR_SYNC_RES_RATIO   = 0x05,
        DRDY_PULSE_CFG_G        = 0x0B,
        INT1_CTRL               = 0x0D,
        WHO_AM_I                = 0x0F,
        CTRL1_XL                = 0x10,
        CTRL2_G                 = 0x11,
        CTRL3_C                 = 0x12,
        CTRL4_C                 = 0x13,
        CTRL5_C                 = 0x14,
        CTRL6_C                 = 0x15,
        CTRL7_G                 = 0x16,
        CTRL8_XL                = 0x17,
        CTRL9_XL                = 0x18,
        CTRL10_C                = 0x19,
        D6D_SRC                 = 0x1D,
        STATUS_REG              = 0x1E,
        OUT_TEMP_L              = 0x20,
        OUT_TEMP_H              = 0x21,
        OUTX_L_G                = 0x22,
        OUTX_H_G                = 0x23,
        OUTY_L_G                = 0x24,
        OUTY_H_G                = 0x25,
        OUTZ_L_G                = 0x26,
        OUTZ_H_G                = 0x27,
        OUTX_L_XL               = 0x28,
        OUTX_H_XL               = 0x29,
        OUTY_L_XL               = 0x2A,
        OUTY_H_XL               = 0x2B,
        OUTZ_L_XL               = 0x2C,
        OUTZ_H_XL               = 0x2D,
        X_OFS_USR               = 0x73,
        Y_OFS_USR               = 0x74,
        Z_OFS_USR               = 0x75
    };

    namespace PreconfiguredRegsValues {
        constexpr uint8_t INT1_CTRL_REG_VAL =   (0b11U << 0U); //INT1 generates interrupt when gyroscope and accelerometer has ready data
        constexpr uint8_t CTRL1_XL_REG_VAL =    (0b10001000U); //1.66kHz, +/-4g scale
        constexpr uint8_t CTRL2_G_REG_VAL =     (0b10000100U); //1.66kHz, 500 dps scale
        constexpr uint8_t CTRL3_C_REG_VAL =     (0b00100100U); //INT pads goes active when low, pushpull mode, 4 wire spi, address incrementation, little endian
    }

    namespace PreparedRegsAddresses {
        constexpr uint8_t READ_WHO_AM_I =   (static_cast<uint8_t>(LSM6DS3_regs::WHO_AM_I) | (0b1U << 7U));
        constexpr uint8_t WRITE_CTRL1_XL =  (static_cast<uint8_t>(LSM6DS3_regs::CTRL1_XL));
        constexpr uint8_t WRITE_CTRL2_G =   (static_cast<uint8_t>(LSM6DS3_regs::CTRL2_G));
        constexpr uint8_t WRITE_CTRL3_C =   (static_cast<uint8_t>(LSM6DS3_regs::CTRL3_C));
        constexpr uint8_t WRITE_INT1_CTRL = (static_cast<uint8_t>(LSM6DS3_regs::INT1_CTRL));
        constexpr uint8_t READ_DATA =       (static_cast<uint8_t>(LSM6DS3_regs::OUTX_L_G) | (0b1U << 7U));
    }

    constexpr std::size_t SPI_CONFIG_SEQUENCE_LENGTH = 2U;
    constexpr std::size_t SENSOR_DATA_READ_SEQUENCE_LENGTH = 13U;
    constexpr uint8_t DUMMY_BYTE = 0xFFU;

    constexpr std::array<uint8_t, SPI_CONFIG_SEQUENCE_LENGTH> sanityCheck {
        PreparedRegsAddresses::READ_WHO_AM_I,
        DUMMY_BYTE,
    };
    constexpr std::array<uint8_t, SPI_CONFIG_SEQUENCE_LENGTH> ctrl3_c_conf_array {
        PreparedRegsAddresses::WRITE_CTRL3_C,
        PreconfiguredRegsValues::CTRL3_C_REG_VAL,
    };
    constexpr std::array<uint8_t, SPI_CONFIG_SEQUENCE_LENGTH> ctrl1_xl_conf_array {
        PreparedRegsAddresses::WRITE_CTRL1_XL,
        PreconfiguredRegsValues::CTRL1_XL_REG_VAL,
    };
    constexpr std::array<uint8_t, SPI_CONFIG_SEQUENCE_LENGTH> ctrl2_g_conf_array {
        PreparedRegsAddresses::WRITE_CTRL2_G,
        PreconfiguredRegsValues::CTRL2_G_REG_VAL,
    };
    constexpr std::array<uint8_t, SENSOR_DATA_READ_SEQUENCE_LENGTH> dataReadSequence {
        PreparedRegsAddresses::READ_DATA,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE,
        DUMMY_BYTE
    };
}


#endif