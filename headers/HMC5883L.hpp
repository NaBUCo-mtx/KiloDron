#ifndef HMC5883L_HPP
#define HMC5883L_HPP

#include <pigpio.h>
#include <cmath>
#include <tuple>
#include <stdexcept>

#ifndef M_PIº
#define M_PI 3.14159265358979323846
#endif

class HMC5883L {
    /*Esta clase emplea pigpio para controlar el magnetómetro HMC5883L.*/

private:
    int i2c_handle;
    int address;
    float r0, p0, y0; // valores de calibración. Serán el "0" de cada eje.
    float roll, pitch, yaw; // valores leídos del sensor

    // Register addresses and config values
    static constexpr uint8_t REG_CONFIG = 0x00;
    static constexpr uint8_t REG_DATA = 0x03;
    static constexpr uint8_t CONFIG_8SAMPLES_75HZ = 0x70;
    static constexpr uint8_t MODE_CONTINUOUS = 0x00;

public:
    HMC5883L(int i2c_address)
        : address(i2c_address), r0(0), p0(0), y0(0), roll(0), pitch(0), yaw(0)
    {
        i2c_handle = i2cOpen(1, address, 0);
        if (i2c_handle < 0) {
            throw std::runtime_error("Error al abrir el dispositivo I2C");
        }
    };

    ~HMC5883L() {
        if (i2c_handle >= 0) {
            i2cClose(i2c_handle);
        }
    };

    std::tuple<float, float, float> readSensor() {
        char buf[6];
        int status = i2cReadI2CBlockData(i2c_handle, REG_DATA, buf, 6);
        if (status != 6) {
            throw std::runtime_error("Error al leer los datos del sensor");
        }
        int16_t x = (buf[0] << 8) | buf[1];
        int16_t z = (buf[2] << 8) | buf[3];
        int16_t y = (buf[4] << 8) | buf[5];

        // Convertir a grados
        roll = atan2(y, z) * 180.0f / M_PI;
        pitch = atan2(-x, sqrtf(y * y + z * z)) * 180.0f / M_PI;
        yaw = atan2(z, x) * 180.0f / M_PI;

        return std::make_tuple(roll - r0, pitch - p0, yaw - y0);
    };

    void initialize() {
        // Calibrar el sensor
        char buf[3];
        buf[0] = REG_CONFIG; // Registro de configuración
        buf[1] = CONFIG_8SAMPLES_75HZ; // Configuración: 8 muestras, 75 Hz
        buf[2] = MODE_CONTINUOUS; // Modo: continuo
        int status = i2cWriteI2CBlockData(i2c_handle, REG_CONFIG, buf, 3);
        if (status != 0) {
            throw std::runtime_error("Error al escribir en el sensor");
        }
        // establecer r0, p0 y y0 como los valores leídos
        char initBuf[6];
        status = i2cReadI2CBlockData(i2c_handle, REG_DATA, initBuf, 6);
        if (status != 6) {
            throw std::runtime_error("Error al leer los datos de calibración del sensor");
        }
        int16_t x = (initBuf[0] << 8) | initBuf[1];
        int16_t z = (initBuf[2] << 8) | initBuf[3];
        int16_t y = (initBuf[4] << 8) | initBuf[5];

        // Convertir a grados
        r0 = atan2(y, z) * 180.0f / M_PI;
        p0 = atan2(-x, sqrtf(y * y + z * z)) * 180.0f / M_PI;
        y0 = atan2(z, x) * 180.0f / M_PI;
    }
};

#endif // HMC5883L_HPP