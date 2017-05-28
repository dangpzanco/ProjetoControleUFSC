#include <AFMotor.h>

AF_DCMotor motor(1);
void setup() {
  Serial.begin(115200);
  motor.setSpeed(0);
  motor.run(FORWARD);
}

int32_t i = 0;
bool ligado = true;

void loop() {
  auto ti = micros();
  int sensorValue = analogRead(A0);
  Serial.print(sensorValue/1024.0*5.0);
  Serial.print(",");
  Serial.println(ligado);
  
  delay(1);
  i++;

  if(i == 4000){
    ligado = false;
    motor.setSpeed(100);
  } else if(i == 6000){
    ligado = true;
    motor.setSpeed(255);
    i = 0;
  }

//  auto to = micros();
//  Serial.print("AAAAAAAAAAAAAAAA         ");
//  Serial.println(to-ti);
  
}
