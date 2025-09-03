#include <pigpio.h>
#include "ESC.hpp"
#include <unistd.h>


int main() {
    if (gpioInitialise() >= 0){
        // primero, una prueba de los dos motores
        bicopter dron(18, 13, 1000, 1000, 1060, 1860);
        dron.arm_motors();
        dron.setSpeeds(50, 50);
        sleep(1);
        dron.setSpeeds(50, 0);
        sleep(1);
        dron.setSpeeds(0, 50);
        sleep(1);
        dron.setSpeeds(0, 0);

        gpioTerminate();
    }
  return 0;
}