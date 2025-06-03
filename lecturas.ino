// este sketch configura el arduino como esclavo I2C y lee los valores del mando

#include <Wire.h>


// Definición de pines para los canales del mando. El mando tiene 6 potenciometros, cada uno conectado a un pin analógico del Arduino.
#define c1 A0
#define c2 A1
#define c3 A2
#define c4 A3
#define c5 A4
#define c6 A5

std::tuple<float, float, float> interpretValues(int* values) {
    // Esta función interpreta los valores leídos del mando y los convierte en la orientación deseada del dron.
    /*Tenemos 6 potenciometros. Estan acomodados en la forma de 3 joysticks. por la construccion del mando, tenemos
    4 grados de libertad (6, pero la traslacion en X y Y es dificil de medir con este modelo).
    Referirse a mis notas para mas detalles.*/

    // Los valores del ADC del arduino van de 0 a 1023, y el centro es 512.
    // ¿Cuál es la mejor manera de interpretar estos valores? Conocemos qué caracteriza a cada tipo de movimiento:
    // Rotación sobre el eje Z: el primer valor en cada joystick (c1, c3, c5) es distinto de cero. Positivo o negativo indica dirección de rotación.
    // Traslación sobre el eje Z: el segundo valor en cada joystick (c2, c4, c6) es distinto de cero. Positivo o negativo indica dirección de traslación.
    // Rotación sobre el eje X: los valores en el joystic 1 son 0. Los segundos en j2 y j3 son distintos de cero. Misma magnitud, pero signo opuesto.
    // El resto es irrelevante, pero pueden ver mis notas para más detalles.
    // EL ángulo máximo de rotación son unos 20 grados. Es decir, hay que mapear todo a un rango de -20 a 20 grados. La traslación
    // sobre Z la mapeamos entre 0 y 1, ya que será un porcentaje de la velocidad máxima del dron.

    float z_rotation = 0.0;
    float z_translation = 0.0;
    float x_rotation = 0.0;

    // Si C1, C3 y C5 son distintos de cero y tienen el mismo signo, hay rotación sobre Z.
    if (values[0] != 512 && values[2] != 512 && values[4] != 512 && 
        ((values[0] > 512 && values[2] > 512 && values[4] > 512) || (values[0] < 512 && values[2] < 512 && values[4] < 512))) {
        z_rotation = (values[0] - 512) / 512.0 * 20.0; // Mapeamos a un rango de -20 a 20 grados
    }
    // Si C2, C4 y C6 son distintos de cero y tienen el mismo signo, hay traslación sobre Z.
    if (values[1] != 512 && values[3] != 512 && values[5] != 512 && 
        ((values[1] > 512 && values[3] > 512 && values[5] > 512) || (values[1] < 512 && values[3] < 512 && values[5] < 512))) {
        z_translation = (values[1] - 512) / 512.0 * 20.0; // Mapeamos a un rango de -20 a 20 grados
    }

    //Si C1 y C2 son cero, entonces hay rotación sobre X. El ángulo dependerá de la diferencia entre C4 y C6.
    if (values[0] == 512 && values[1] == 512 && values[3] != 512 && values[5] != 512) {
        // La rotación sobre X es proporcional a la diferencia entre C4 y C6. Aquí, el valor máximo es 1024, no 512.
        x_rotation = (values[3] - values[5]) / 1024.0 * 20.0; // Mapeamos a un rango de -20 a 20 grados
    }

    // esto (en teoría) permite que extraigamos las rotaciones aún cuando tenemos gestos "mixtos".
    // Por ejemplo, si giro y presiono el mando, eso se interpreta como rotación sobre Z y traslación sobre Z.
    // Aquí falta mucha lógica; este programa es bastante rudimentario y no contempla todos los casos posibles.

    return std::make_tuple(z_rotation, z_translation, x_rotation);
}

int receiveEvent(int howMany) {
    // This function is called when data is received from the master
    while (Wire.available()) {
        char c = Wire.read(); // Read the incoming byte
        Serial.print("Received: ");
        if (c == 'R'){ 
            // El maestro envió la orden de medir los valores del mando.
            int c1_value = analogRead(c1); // leer del canal 1
            int c2_value = analogRead(c2); // leer del canal 2
            int c3_value = analogRead(c3); // leer del canal 3
            int c4_value = analogRead(c4); // leer del canal 4
            int c5_value = analogRead(c5); // leer del canal 5
            int c6_value = analogRead(c6); // leer del canal 6
            int values[6] = {c1_value, c2_value, c3_value, c4_value, c5_value, c6_value};
            // TODO: escribir una función que interprete estos valores y los convierta en la orientacion deseada del dron.
            auto [Zrr, Ztr, Xrr] = interpretValues(values); // Interpretar los valores leídos. Obtener referencia. 
            Wire.write((uint8_t*)&Zrr, sizeof(Zrr)); // Enviar el valor de Zrr al maestro
            Wire.write((uint8_t*)&Ztr, sizeof(Ztr)); // Enviar el valor de Ztr al maestro
            Wire.write((uint8_t*)&Xrr, sizeof(Xrr)); // Enviar el valor de Xrr al maestro
        }
    }
    return 0; // Return 0 to indicate success
}

void setup() {
    Wire.begin(8); // Join I2C bus with address #8
    Wire.onReceive(receiveEvent); // Register event
}

void loop() {
    // creo que no es necesario hacer nada aqui... al menos no para la lectura del mando.
}