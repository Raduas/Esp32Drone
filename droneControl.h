#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
function which contoling num variables power, pitch, roll, yaw
*/

/*void controlDrive(String)  {
  static int storeAcc[4][5]; //collumbs: 0-2 Acc:x,y,z;3-5 rotation x,y,z
  static int storeVelo[4][5];
  static int storeMove[4][5];
  // take time for derivation
  unsigned int timeGet = millis();
  Serial.println(timeGet);
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

      // array accel,gyro FIFO 
      // Print out the values 
      Serial.print("Acceleration X: ");
      Serial.print(a.acceleration.x);
      Serial.print(", Y: ");
      Serial.print(a.acceleration.y);
      Serial.print(", Z: ");
      Serial.print(a.acceleration.z);
      Serial.println(" m/s^2");

      Serial.print("Rotation X: ");
      Serial.print(g.gyro.x);
      Serial.print(", Y: ");
      Serial.print(g.gyro.y);
      Serial.print(", Z: ");
      Serial.print(g.gyro.z);
      Serial.println(" rad/s");
}

*/

void partitionInputString(char* input, int8_t* powerP, int8_t* yawP, int8_t* pitchP, int8_t* rollP) { //dodělat pointery https://forum.arduino.cc/t/returning-two-values-from-a-function/90068/6
    int8_t id;

    Serial.printf("input: %s\n", input);
    char* token = strtok(input, ":"); //char*
    id = atoi(token);
    token = strtok(NULL, ":");
    float value1 = atof(token);
    int8_t value1Char = round(value1*100);

    if (token != NULL)
      token = strtok(NULL, ":");
      float value2 = atof(token);
      int8_t value2Char = round(value2*100);

    
    
    Serial.printf("id: %i\n", id);
    switch (id) {
      case 1:
        *rollP = value1Char;
        *pitchP = value2Char;
        Serial.printf(" roll: %i\n", *rollP); 
        break;
      case 2: 
        *yawP = value1Char;
        *powerP = value2Char;
        Serial.printf(" power: %i\n", *powerP);
        break;
      default:
        
    }
    return;
}