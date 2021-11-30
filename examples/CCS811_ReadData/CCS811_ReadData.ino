/*
   Created by ASTRON by nikitagricanuk 2021 Nov 30
   After enabling, sensor should be heated to operating temperature.
   It usually takes ~ 30 minutes.
*/

#include <ASTRON_CCS811.h>
#include <Wire.h> // I2C library

//Constants
#define I2Cdelay 50 //50 for ESP, 0 for other platforms
#define addr ADDRESS //If doesn`t work - change to ALT_ADDRESS

CCS811 sensor(I2Cdelay, addr);
void setup() {
  Serial.begin(9600);
  Wire.begin();

  //Check errors
  if (!sensor.begin()) {
    Serial.println("CCS811Init.... Err");
  }
  else {
    Serial.println("CCS811Init.... OK");
  }
  delay(1000);
}


void loop() {
  // Read

  if (sensor.readCO2() != 0) { //if sensor is measured data
    //Printing data
    Serial.print("CO2: ");
    Serial.print(sensor.readCO2());
    Serial.println(" ppm");

    Serial.print("CO2(mg/m3): ");
    Serial.print(sensor.readCO2mg());
    Serial.println(" mg/m3");

    Serial.print("TVOC: ");
    Serial.print(sensor.readTVOC());
    Serial.println(" ppb");

    Serial.println();
  }
  delay(1000); // Wait
}
