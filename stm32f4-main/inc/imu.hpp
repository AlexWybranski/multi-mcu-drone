#ifndef IMU_HPP
#define IMU_HPP

#include <cstdint>

#include "spi.hpp"

//Maybe not needed
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