#include <AFMotor.h>
#include "TimerOne.h"

const int16_t filterSize = 8;
const int16_t log2Size = 3;

AF_DCMotor motor(1);
int32_t leSerial = 0;
bool ligado = true;

void setup(){
    Serial.begin(115200);
    motor.setSpeed(0);
    motor.run(FORWARD);
    
    pinMode(13, OUTPUT);
    
    // Inicializa o Timer1 e configura para um período de 0,5 segundos
    Timer1.initialize(10000);
    
    // Configura a função callback() como a função para ser chamada a cada interrupção do Timer1
    Timer1.attachInterrupt(callback);
}

float a[2] = {70.0567e-3f, -60.0567e-3f};
int16_t buffIn[filterSize] = {0};
int16_t buffControl[2] = {0};
float gainControl = 1.0f;

// (Velocidade desejada em PWM) * 4
int16_t desiredVelocity = 600;

void callback(){

    leSerial++;
    

    buffIn[0] = analogRead(A0);

    // Moving Average
    int16_t h = 0;
    for(int i = 0; i < filterSize; i++){
        h += buffIn[i];
    }
    h >>= log2Size; // Divide by filterSize

    int16_t error = desiredVelocity - h;

    // c[n] = c[n-1] + a[0] * e[n] + a[1] * e[n-1]
    int16_t outControl = buffControl[1] + a[0] * error + a[1] * buffControl[0];
    outControl *= gainControl;
    buffControl[0] = error;
    buffControl[1] = outControl;

    // Set correct range of values
    int16_t outVal = outVal >> 2;
    if(outVal < 100) outVal = 100;
    if(outVal > 255) outVal = 255;
    
    // Output
    motor.setSpeed(outVal);

    // Shift the input buffer
    for(int i = 0; i < filterSize - 1; i++) {
        buffIn[filterSize - 1 - i] = buffIn[filterSize - i - 2];
    }

    digitalWrite(13, digitalRead(13) ^ 1);
}


void loop(){
  if(leSerial >= 100){
    // Se receber algo pela serial
    if (Serial.available() > 0){
      // Lê toda string recebida

      char recebido = Serial.read();
        
      if (recebido == 1){
        Serial.print("oi, tudo bem");
      }
    }
  }
}
