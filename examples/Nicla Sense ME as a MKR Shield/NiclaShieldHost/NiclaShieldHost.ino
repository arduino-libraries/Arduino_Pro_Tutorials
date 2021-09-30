#include "Wire.h"

#define NICLA_I2C_ADDRESS 0x1A

void setup(){
    Serial.begin(9600);
    while (!Serial);

    Wire.begin(); // Join the I2C bus as a Host
    Serial.println("Host started");
}

void loop(){

    I2CWrite(1); // Request Acceleration X
    int16_t acX = getData16();

    I2CWrite(2); // Request Acceleration Y
    int16_t acY = getData16();

    I2CWrite(3); // Request Acceleration Z
    int16_t acZ = getData16();

    Serial.print("ACCELERATION :");
    Serial.print("  X: ");
    Serial.print(acX);
    Serial.print("  Y: ");
    Serial.print(acY);
    Serial.print("  Z: ");
    Serial.print(acZ);
    Serial.println();

    delay(2500);
}

void I2CWrite(int command){
    Wire.beginTransmission(NICLA_I2C_ADDRESS);
    Wire.write(command);
    Wire.endTransmission();
    delay(100);
}

int16_t getData16(){
    int16_t data = 0;

    Wire.requestFrom(0x1A, 2);

    while (Wire.available()){
        for (int i = 0; i < 2; i++){
            data |= Wire.read() << (8 * i);
        }
    }

    return data;
}