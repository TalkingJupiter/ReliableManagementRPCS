/***********************************
  * File:
  * Author:
  * Date:
  * Copyright:
  *
  * Description: 
  * This file implements the control of multi I2C sensors (SHT31)
  * with a single master device (ESP32)
  *
  * Dependencies:
  * 
  * 
  * Modification History:
  * 01-10-2026: Initial creation
***********************************/

/********** Libraries **********/
#include <Wire.h>
#include "Adafruit_SHT31.h"

/********** ADDRESS **********/
#define TCA_ADDR 0x70
#define SHT31_ADDR 0x44

/********** Constants **********/
const uint8_t NUM_Sensors = 6;
const uint8_t sensorChannels[NUM_Sensors] = {0,1,2,3,4,5};


Adafruit_SHT31 sht31 = Adafruit_SHT31(); //-> Create an object named sht31

/********** HELPERS **********/
bool tcaSelect(uint8_t channel){
  if (channel > 7) return false; //-> Since the mux has 8 channels we do not accept the channel bigger than 8!

  Wire.beginTransmission(TCA_ADDR);//-> Setup the bus line
  Wire.write(1 << channel);  //-> Inform the channel to be active
  return (Wire.endTransmission() == 0);
}

// Disable all channel incase on an emergency 
void tcaDisableAll(){
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(0x00);
  Wire.endTransmission();
}

bool initSensorOnChannel(uint8_t channel){
  if(!tcaSelect(channel)) return false; //-> If there is no selected channel do not proceed.

  if(!sht31.begin(SHT31_ADDR)) return false; //-> Re-initilazition per channel

  sht31.heater(false); //-> Heater is off by default
  return true;
}

/********** MAIN **********/
void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(); //-> Init the communication
  Wire.setClock(400000); //-> FAST MODE ACTIVE

  Serial.println("Initializing SHT31 sensors via TCA9548A...")

  for (uint8_t i=0; i<NUM_Sensors; i++){
    uint8_t ch = sensorChannels[i];
    Serial.print("CH"); Serial.print(ch); Serial.print(": ");

    if(initSensorOnChannel(ch)){
      Serial.println("OK");
    } else {
      Serial.println("FAILED: Unable to connect!");
    }
  }

  tcaDisableAll();
  Serial.println("Check completed.\n");

}

void loop() {
  for (uint8_t i=0; i<NUM_Sensors; i++){
    uint8_t ch = sensorChannels[i];

    if(!tcaSelect(ch)){
      Serial.print("CH"); Serial.print(ch);
      Serial.println(": mux select failed");
      continue;
    }

    if(!sht31.begin(SHT31_ADDR)){
      Serial.print("CH"); Serial.print(ch);
      Serial.println(": SHT31 not found");
      continue;
    }


    // Reading the values
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

    Serial.print("CH"); Serial.print(ch);
    Serial.print("    T=");

    if(isnan(t) || isnan(h)){
      Serial.println("Read Failed"); //-> If nan received print fail message else print values
    } else {
      Serial.print(t, 2); Serial.print(" C");
      Serial.print("  H="); Serial.print(h, 2); Serial.print("%");

      tcaDisableAll();
      Serial.println("-----");
      delay(2000)
    }

  }

}
