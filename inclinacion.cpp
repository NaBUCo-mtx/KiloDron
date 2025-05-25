#include <pigpio.h>
#include "ESC.hpp"
#include "HMC5883L.hpp"
#include <cstdio>   // For printf
#include <unistd.h> // For sleep
#include <stdexcept> // For exception handling

float getSetPoint(); // Forward declaration

int main() {
    // Inicializar pigpio
    if (gpioInitialise() < 0) {
        printf("No se pudo inicializar pigpio.\n");
        return 1;
    }

    try {
        // Inicializar el magnetómetro
        HMC5883L magnetometro(0x1E);
        magnetometro.initialize();

        bicopter test_1d(18, 13, 1000, 1000, 1060, 1860);
        test_1d.arm_motors();

        // PID parameters
        float Kp = 1.0, Ki = 0.0, Kd = 0.1;
        float setpoint = 0.0; // Desired roll
        float integral = 0.0, prev_error = 0.0;

        // Base speed for both motors
        float base_speed = 50.0;

        while (true) {
            // leemos el sensor
            auto [roll, pitch, yaw] = magnetometro.readSensor();
            setpoint = getSetPoint(); // Get the desired setpoint
            float error = setpoint - roll;
            integral += error;

            // Clamp integral to prevent windup
            if (integral > 100) integral = 100;
            if (integral < -100) integral = -100;

            float derivative = error - prev_error;
            float output = Kp * error + Ki * integral + Kd * derivative;
            prev_error = error;

            // Adjust motor speeds based on PID output
            float speed_1 = base_speed + output;
            float speed_2 = base_speed - output;

            // Clamp speeds to [0, 100]
            if (speed_1 > 100) speed_1 = 100;
            if (speed_1 < 0) speed_1 = 0;
            if (speed_2 > 100) speed_2 = 100;
            if (speed_2 < 0) speed_2 = 0;

            test_1d.setSpeeds(speed_1, speed_2);

            printf("Roll: %.2f, Pitch: %.2f, Yaw: %.2f | Output: %.2f | Speeds: %.2f, %.2f\n",
                   roll, pitch, yaw, output, speed_1, speed_2);

            usleep(10000); // 10 ms for smoother control
        }
    } catch (const std::exception& e) {
        printf("Excepción: %s\n", e.what());
    }

    gpioTerminate();
    return 0;
}

float getSetPoint() {
    // Por el momento, para probar, siempre retorna 0.0
    return 0.0;
}