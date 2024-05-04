/*
function to direct drive from num variables power, pitch, roll, yaw
to pwm duty for mosfets which powered motors from external power supply
*/

void valsToPWM(int8_t power, int8_t yaw, int8_t pitch, int8_t roll, int16_t* pwmFL, int16_t* pwmFR, int16_t* pwmRL, int16_t* pwmRR){

  if(power<0){
    *pwmFL=0;
    *pwmFR=0;
    *pwmRL=0;
    *pwmRR=0;
    //Serial.printf("memsi nez nula");
    }
  else{
    *pwmFL=(power*2.5);
    *pwmFR=(power*2.5);
    *pwmRL=(power*2.5);
    *pwmRR=(power*2.5);
    //Serial.printf("pwm: %i",*pwmFL);
    }
  return;
}

