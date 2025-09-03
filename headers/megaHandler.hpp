#pragma once

#include <pigpio.h>
#include <vector>
#include <stdexcept>
#include <cstdint>

class MegaHandler {
public:
    // Constructor: bus = I2C bus number, addr = Arduino Mega I2C address
    MegaHandler(unsigned bus, unsigned addr)
        : i2cBus(bus), i2cAddr(addr), handle(-1)
    {
        handle = i2cOpen(i2cBus, i2cAddr, 0);
        if (handle < 0) {
            throw std::runtime_error("Failed to open I2C device");
        }
    }

    // Delete copy constructor and copy assignment to avoid double-close
    MegaHandler(const MegaHandler&) = delete;
    MegaHandler& operator=(const MegaHandler&) = delete;

    // Allow move semantics
    MegaHandler(MegaHandler&& other) noexcept
        : i2cBus(other.i2cBus), i2cAddr(other.i2cAddr), handle(other.handle), lastData(std::move(other.lastData))
    {
        other.handle = -1;
    }
    MegaHandler& operator=(MegaHandler&& other) noexcept {
        if (this != &other) {
            if (handle >= 0) i2cClose(handle);
            i2cBus = other.i2cBus;
            i2cAddr = other.i2cAddr;
            handle = other.handle;
            lastData = std::move(other.lastData);
            other.handle = -1;
        }
        return *this;
    }

    ~MegaHandler() {
        if (handle >= 0) {
            i2cClose(handle);
        }
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