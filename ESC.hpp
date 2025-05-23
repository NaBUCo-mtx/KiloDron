#include <pigpio.h>
#include <unistd.h>

#ifndef ESC_HPP
#define ESC_HPP

class PWM_ESC {
    /*Esta clase emplea pigpio para controlar ESCs que reciben entradas por PWM. */
    private:
    int GPIO;
    int FREQ;
    int RANGE;
    int PULSE_MIN;
    int PULSE_MAX;

    public:

    PWM_ESC(int gpio, int freq, int range, int min, int max){
        GPIO = gpio;
        FREQ = freq;
        RANGE = range;
        PULSE_MIN = min;
        PULSE_MAX = max;
        bool armed;
        if (gpioInitialise() < 0){
            sleep(3) // aca hay que poner algun error
        }
        else{
            /*NOTA: el rango y la frecuencia se pueden poner así por pura coincidencia.
            Si trabajaremos con otros ESCs, el rango debe ser 1E6/freq; simplemente resulta
            que en este caso son 1000 y 1000. Como 1E6/1000=1000, lo escribimos así*/ 
            gpioSetMode(GPIO, PI_OUTPUT);
            gpioSetPWMrange(GPIO, RANGE);
            gpioSetPWMfrequency(GPIO, FREQ);
        };
    };

    bool arm_ESC(){
        /*TODO: definitivamente debe haber una mejor manera de hacer esto... q puta verguenza usar sleep.
        quizas se puede sin usar sleep, pero es necesario asegurar que se envie el pulso por un minimo de tiempo*/
        gpioServo(GPIO, PULSE_MIN);
        sleep(3);
        return true;
    };

    bool setSpeed(float speed){
        /*Recibe una nueva velocidad expresada como un porcentaje de la velocidad máxima.
        flotantes del 1 al 100.*/
        if (speed > 100){
            speed = 100;
        };
        if (speed < 0){
            speed = 0;
        };
        speed=speed/100;
        int pWidth = int(PULSE_MIN + speed*(PULSE_MAX-PULSE_MIN));
        gpioServo(GPIO, pWidth);
        return true;
    };

    bool stop(){
        /*Detiene el motor.*/
        setSpeed(0);
        return true;
    }
};


class bicopter {
    /*Esta clase utiliza 2 instancias de PWM_ESC para controlar 2 motores, como 
    en un bicóptero*/
    private:
    int gpio_1;
    int gpio_2;
    int FREQ;
    int RANGE;
    int PULSE_MIN;
    int PULSE_MAX;
    PWM_ESC ESC_1;
    PWM_ESC ESC_2;

    public:
    bicopter(int esc1, int esc2, int freq, int range, int min, int max){
        PWM_ESC ESC_1(esc1, freq, range, min, max);
        PWM_ESC ESC_2(esc2, freq, range, min, max);
    };

    bool arm_motors(){
        /*Arma los motores uno por uno.*/
        ESC_1.arm_ESC();
        ESC_2.arm_ESC();
        return true;
    };

    bool setSpeeds(int speed_1, int speed_2){
        /*establece las velocidades para los dos. Si quieres controlarlos individualmentes,
        considera utilizar la clase PWM_ESC directamente*/
        ESC_1.setSpeed(speed_1);
        ESC_2.setSpeed(speed_2);
        return true;
    };

    bool stop(){
        /*Detiene los dos motores.*/
        ESC_1.stop();
        ESC_2.stop();
        return true;
    };

};

#endif