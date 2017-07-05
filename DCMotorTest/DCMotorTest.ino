#include <AFMotor.h>
#include "TimerOne.h"

const int16_t filterSize = 32;
const int16_t log2Size = 5;

AF_DCMotor motor(1);
int32_t leSerial = 0;
bool ligado = true;
int32_t T = 5000;

void setup(){
    Serial.begin(115200);
    motor.setSpeed(0);
    motor.run(FORWARD);
    
    pinMode(13, OUTPUT);
    
    // Inicializa o Timer1 e configura para um período de T segundos
    Timer1.initialize(T);
    
    // Configura a função callback() como a função para ser chamada a cada interrupção do Timer1
    Timer1.attachInterrupt(callback);
}

//float a[2] = {70.0567e-3f, -60.0567e-3f};
// float a[2] = {70.0567e-3f, -65.2310e-3f};
float a[2] = {70.0567e-3f, -65.06e-3f};
int16_t buffIn[filterSize] = {0};
int16_t buffControl[2] = {0};
float gainControl = 1.0f;

float outvalGlobal = 0;
float outGlobal[4] = {0};

// (Velocidade desejada em PWM) * 4
int16_t desiredVelocity = 30.0f * 800 / 87.5f;

//87.45 == 800

void callback(){
    leSerial++;
    
    // Input
//    buffIn[0] = analogRead(A0) * 1.75f;
    buffIn[0] = (analogRead(A0) * 7) >> 2;
    
    outGlobal[0] = buffIn[0];
    
    // Moving Average
    int16_t h = 0;
    for(int i = 0; i < filterSize; i++){
        h += buffIn[i];
    }
    h >>= log2Size; // Divide by filterSize
    
    outGlobal[1] = h;
    
    // Calculate error
    int16_t error = desiredVelocity - h;
//    int16_t error = desiredVelocity - buffIn[0];

    outGlobal[2] = error;
    
    // c[n] = c[n-1] + a[0] * e[n] + a[1] * e[n-1]
    int16_t outControl = buffControl[1] + a[0] * error + a[1] * buffControl[0];
    outControl *= gainControl;
    buffControl[0] = error;
    buffControl[1] = outControl;
    
    // Set correct range of values
    int16_t outVal = outControl >> 2;

    outGlobal[3] = outControl;
    
    if(outVal < 0) {
        outVal = 0;
        buffControl[1] = 0;
    }
    if(outVal > 255){
        outVal = 255;
        buffControl[1] = 255 << 2;
    }

    // Output
    motor.setSpeed(outVal);

    // Shift the input buffer
    for(int i = 0; i < filterSize - 1; i++) {
        buffIn[filterSize - 1 - i] = buffIn[filterSize - i - 2];
    }
}


void loop(){
    if(leSerial >= 100){
        
        // Reset counter
        leSerial = 0;

        // [in, h, erro, out]
        Serial.print(outGlobal[0]);
        Serial.print("\t");
        Serial.print(outGlobal[1]);
        Serial.print("\t");
        Serial.print(outGlobal[2]);
        Serial.print("\t");
        Serial.println(outGlobal[3]);
        
        // Se receber algo pela serial
        if (Serial.available() > 0){

            String VelocidadeRPM = Serial.readString();
//             desiredVelocity = VelocidadeRPM.toFloat() * 800 / 87.5f;
            desiredVelocity = VelocidadeRPM.toFloat() * 800 / 101.5f;
//            // Lê toda string recebida
//            char recebido = Serial.read();
//
//            if (recebido == 1){
//                Serial.print("oi, tudo bem");
//            }
        }
    }
    
    
}
