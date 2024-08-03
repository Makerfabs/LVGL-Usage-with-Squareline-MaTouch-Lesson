#include <Wire.h>
#include "AS5600.h"

#define SDA_PIN 21//17
#define SCL_PIN 22//18

AMS_5600 ams5600;


void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    i2c_scan();

    Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>> ");
    if (ams5600.detectMagnet() == 0)
    {
        while (1)
        {
            if (ams5600.detectMagnet() == 1)
            {
                Serial.print("Current Magnitude: ");
                Serial.println(ams5600.getMagnitude());
                break;
            }
            else
            {
                Serial.println("Can not detect magnet");
            }
            delay(1000);
        }
    }
}
/*******************************************************
/* Function: convertRawAngleToDegrees
/* In: angle data from AMS_5600::getRawAngle
/* Out: human readable degrees as float
/* Description: takes the raw angle and calculates
/* float value in degrees.
/*******************************************************/
float convertRawAngleToDegrees(word newAngle)
{
    /* Raw data reports 0 - 4095 segments, which is 0.087890625 of a degree */
    float retVal = newAngle * 0.087890625;
    return retVal;
}
void loop()
{
    Serial.println(String(convertRawAngleToDegrees(ams5600.getRawAngle()), DEC));
}

//  -- END OF FILE --

void i2c_scan()
{
    byte error, address;
    int nDevices;

    static int s = 0;
    Serial.print(s++);
    Serial.println(". Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
    {
        Serial.print(">>>> Found total ");
        ;
        Serial.print(nDevices);
        Serial.println(" devices\n");
    }
}
