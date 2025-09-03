#include <Wire.h>

// Definición de pines
#define c1 A0
#define c2 A1
#define c3 A2
#define c4 A3
#define c5 A4
#define c6 A5

// Función que interpreta los valores leídos del mando
void interpretValues(int* values, float* z_rotation, float* z_translation, float* x_rotation) {
    *z_rotation = 0.0;
    *z_translation = 0.0;
    *x_rotation = 0.0;

    if (values[0] != 512 && values[2] != 512 && values[4] != 512 && 
        ((values[0] > 512 && values[2] > 512 && values[4] > 512) || (values[0] < 512 && values[2] < 512 && values[4] < 512))) {
        *z_rotation = (values[0] - 512) / 512.0 * 20.0;
    }

    if (values[1] != 512 && values[3] != 512 && values[5] != 512 && 
        ((values[1] > 512 && values[3] > 512 && values[5] > 512) || (values[1] < 512 && values[3] < 512 && values[5] < 512))) {
        *z_translation = (values[1] - 512) / 512.0 * 20.0;
    }

    if (values[0] == 512 && values[1] == 512 && values[3] != 512 && values[5] != 512) {
        *x_rotation = (values[3] - values[5]) / 1024.0 * 20.0;
    }
}

// Función llamada cuando el maestro envía datos
void receiveEvent(int howMany) {
    while (Wire.available()) {
        char c = Wire.read();
        if (c == 'R') {
            int values[6];
            values[0] = analogRead(c1);
            values[1] = analogRead(c2);
            values[2] = analogRead(c3);
            values[3] = analogRead(c4);
            values[4] = analogRead(c5);
            values[5] = analogRead(c6);

            float z_rotation, z_translation, x_rotation;
            interpretValues(values, &z_rotation, &z_translation, &x_rotation);

            Wire.write((uint8_t*)&z_rotation, sizeof(z_rotation));
            Wire.write((uint8_t*)&z_translation, sizeof(z_translation));
            Wire.write((uint8_t*)&x_rotation, sizeof(x_rotation));
        }
    }
}

void setup() {
    Wire.begin(8); // Dirección I2C esclava
    Wire.onReceive(receiveEvent);
}

void loop() {
    // Nada aquí, todo ocurre en receiveEvent
}
