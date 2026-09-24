#ifndef IMU_HPP
#define IMU_HPP

#include <cstdint>

#include "spi.hpp"

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

class ImuHandle {
    private:
        SpiHandle* spiPtr{nullptr};

    public:
        ImuHandle(SpiHandle* spi_instance) : spiPtr(spi_instance) {

        }
        ~ImuHandle() = default;
        ImuHandle& operator=(const ImuHandle& other) = delete;
        ImuHandle(ImuHandle&& other) = delete;
        ImuHandle& operator=(ImuHandle&& other) = delete;

        void init();
        void read();
        void write();
};

#endif