#include "Nicla_System.h"
#include "Arduino_BHY2.h"
#include "Wire.h"

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);

uint8_t command = 0x00;
void requestHandler(); //request event callback
void receiveEvent(int howMany); //receive event callback

void setup(){
    BHY2.begin();

    //Configure Sensors
    accel.configure(1, 0);
    gyro.configure(1, 0);
    temp.configure(1, 0);

    //Give LED feedback to the user
    nicla::leds.begin();
    nicla::leds.setColor(green);

    Wire.begin(0x1A); // join i2c bus with address #0x1A , do not use 0x60 nor 0x6B, the MKR board has those i2c devices

    Wire.onRequest(requestHandler); // Callback triggered from `Wire.requestFrom()` done by the Host

    Wire.onReceive(receiveEvent); // Callback triggered from `Wire.beginTransmission()` done by the Host
}

void loop(){
    BHY2.update(10);
}

void I2CWrite16(int16_t data){
    Wire.write((byte)(data & 0xFF));
    Wire.write((byte)((data >> 8) & 0xFF));
}

void receiveEvent(int howMany){

    nicla::leds.setColor(blue);
    while (Wire.available()){
        command = Wire.read();
    }

    nicla::leds.setColor(off);
}

void requestHandler(){
    nicla::leds.setColor(green);

    int16_t dataX;
    int16_t dataY;
    int16_t dataZ;

    switch (command){
        
    //Update readings command
    case 0:
        break;

    case 1:
        dataX = accel.x();
        I2CWrite16(dataX);
        Serial.println(accel.toString());
        break;

    case 2:
        dataY = accel.y();
        I2CWrite16(dataY);
        break;

    case 3:
        dataZ = accel.z();
        I2CWrite16(dataZ);
        break;

    default:
        break;
    }

    nicla::leds.setColor(off);
}
