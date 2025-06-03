#pragma once

#include <pigpio.h>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <string>

class MegaHandler {
public:
    // Constructor: bus = I2C bus number, addr = Arduino Mega I2C address
    MegaHandler(unsigned bus, unsigned addr)
        : i2cBus(bus), i2cAddr(addr), handle(-1)
    {
        if (gpioInitialise() < 0) {
            throw std::runtime_error("pigpio initialization failed");
        }
        handle = i2cOpen(i2cBus, i2cAddr, 0);
        if (handle < 0) {
            gpioTerminate();
            throw std::runtime_error("Failed to open I2C device");
        }
    }

    ~MegaHandler() {
        if (handle >= 0) {
            i2cClose(handle);
        }
        gpioTerminate();
    }

    // Send a command to Arduino Mega to request sensor data
    void requestSensorData(uint8_t command) {
        int status = i2cWriteByte(handle, command);
        if (status < 0) {
            throw std::runtime_error("Failed to send command to Arduino Mega");
        }
    }

    // Read sensor data from Arduino Mega
    std::vector<uint8_t> readSensorData(size_t length) {
        std::vector<uint8_t> buffer(length);
        int bytesRead = i2cReadDevice(handle, buffer.data(), length);
        if (bytesRead < 0) {
            throw std::runtime_error("Failed to read data from Arduino Mega");
        }
        buffer.resize(bytesRead);
        lastData = buffer;
        return buffer;
    }

    // Get the last data read
    const std::vector<uint8_t>& getLastData() const {
        return lastData;
    }

private:
    unsigned i2cBus;
    unsigned i2cAddr;
    int handle;
    std::vector<uint8_t> lastData;
};