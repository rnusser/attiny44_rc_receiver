/*
ATtiny24/44/84 Pin map
                                  +-\/-+
                            VCC  1|o   |14 GND
                            PB0  2|    |13 PA0  (AREF)
                            PB1  3|    |12 PA1
                            PB3  4|    |11 PA2
                      (PWM) PB2  5|    |10 PA3
        (Analog Input, PWM) PA7  6|    |9  PA4 (SCK)
        (Analog input, PWM) PA6  7|    |8  PA5 (PWM, MOSI_slave )
                                  +----+


Connection to nRF24L01, servos and motor

                        +-\/-+
                  VCC  1|    |14 GND
   roll servo --- PB0  2|    |13 PA0
  pitch servo --- PB1  3|    |12 PA1
                  PB3  4|    |11 PA2 --- nRF24L01 CSN
          LED --- PB2  5|    |10 PA3 --- nRF24L01 CE
        motor --- PA7  6|    |9  PA4 --- nRF24L01 SCK
nRF24L01 MOSI --- PA6  7|    |8  PA5 --- nRF24L01 MOSI
                        +----+

*/

// Uploaded 30.09.2023

#include <Servo_ATTinyCore.h>
#include "RF24.h"

// ATtiny
// #define CE_PIN PIN_A3  // 3 clockwise
// #define CSN_PIN PIN_A2 // 2 clockwise
#define CE_PIN PIN_PA3  // 3 clockwise
#define CSN_PIN PIN_PA2 // 2 clockwise

#define MOTOR PIN_PA7 // 8
#define ROLL PIN_PB0  // 10
#define PITCH PIN_PB1 // 9
#define LED PIN_PB2   // 9

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

byte address[6] = "1Node";

uint8_t ppm_cnt, prev_cnt, missing;

struct PayloadStruct
{
  uint8_t throttle; // Thrust
  uint8_t pitch;    // Elevator
  uint8_t roll;     // Aileron
  uint8_t yaw;      // Rudder
  uint8_t ppm_cnt;
};
PayloadStruct payload;

Servo pitch_servo; // create servo object to control a servo
Servo roll_servo;  // create servo object to control a servo

void setup()
{
  pitch_servo.attach(PITCH); // attaches the servo on pin 9 to the servo object
  roll_servo.attach(ROLL);   // attaches the servo on pin 9 to the servo object
  pinMode(MOTOR, OUTPUT);
  pinMode(LED, OUTPUT);

  radio.begin();
  radio.openReadingPipe(1, address);

  radio.startListening();
  delay(1000);
  digitalWrite(LED, LOW);
  // initilize flashing LED
}

void loop()
{
  while (radio.available())
  {
    radio.read(&payload, sizeof(payload));
    update_hardware(payload);
    ppm_cnt = payload.ppm_cnt;
    delay(10);
  }

  missing = (prev_cnt == ppm_cnt) ? missing += 1 : 0;
  if (missing > 1000)
  {
    // Time to fly in a circle
    ResetData();
    update_hardware(payload);
    missing = 0;
  }
  prev_cnt = ppm_cnt;
}

void update_hardware(PayloadStruct payload)
{
  pitch_servo.write(payload.pitch); // tell servo to go to position in variable 'pos'
  roll_servo.write(payload.roll);   // tell servo to go to position in variable 'pos'
  analogWrite(MOTOR, payload.throttle);
}

void ResetData()
{
  payload.throttle = 0; // Center | Signal lost position / THR
  payload.pitch = 127;  // Center | Signal lost position / ELV
  payload.roll = 30;    // left | Signal lost position / AIL
  payload.yaw = 127;    // Center | Signal lost position / RUD
}