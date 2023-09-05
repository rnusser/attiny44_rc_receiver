/*
ATtiny24/44/84 Pin map
                                  +-\/-+
    nRF24L01  VCC, pin2 --- VCC  1|o   |14 GND --- nRF24L01  GND, pin1
                            PB0  2|    |13 AREF
                            PB1  3|    |12 PA1                        (Robert's)
                            PB3  4|    |11 PA2 --- nRF24L01   CE pin3 (CSN  pin 4)
                            PB2  5|    |10 PA3 --- nRF24L01  CSN pin4 (CE   pin 3)
                            PA7  6|    |9  PA4 --- nRF24L01  SCK pin5 (SCK  pin 5)
    nRF24L01 MOSI, pin7 --- PA6  7|    |8  PA5 --- nRF24L01 MISO pin6 (MOSI pin 6)
    ( MISO pin7 )                 +----+

Using Clockwise pin numbering:
   +-\/-+
VCC|    |GND
 10|    |0
  9|    |1
 11|    |2
  8|    |3
  7|    |4
  6|    |5
   +----+

*/
#include <Servo_ATTinyCore.h>
#include "RF24.h"

// ATtiny
// #define CE_PIN PIN_A3  // 3 clockwise
// #define CSN_PIN PIN_A2 // 2 clockwise
#define CE_PIN PIN_PA3  // 3 clockwise
#define CSN_PIN PIN_PA2 // 2 clockwise

#define LED PIN_PB2   // 8
#define ROLL PIN_PB0  // 10
#define PITCH PIN_PB1 // 9

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

int pos = 0; // variable to store the servo position

void setup()
{
  pitch_servo.attach(PITCH); // attaches the servo on pin 9 to the servo object
  roll_servo.attach(ROLL);   // attaches the servo on pin 9 to the servo object
  pinMode(LED, OUTPUT);

  radio.begin();
  radio.openReadingPipe(1, address);
  radio.startListening();
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
}

void position_servos(PayloadStruct payload)
{
  pitch_servo.write(payload.pitch); // tell servo to go to position in variable 'pos'
  roll_servo.write(payload.roll);   // tell servo to go to position in variable 'pos'
}

void loop()
{
  while (radio.available())
  {
    digitalWrite(LED, HIGH);
    position_servos(payload);
    ppm_cnt = payload.ppm_cnt;
  }
  digitalWrite(LED, LOW);

  /*
  missing = (prev_cnt == ppm_cnt) ? missing += 1 : 0;
  if (missing > 10)
  {
    // Time to fly in a circle
    ResetData();
    position_servos(payload);
    missing = 0;
  }
  prev_cnt = ppm_cnt = payload.ppm_cnt;
  */
  sweep();
}

void sweep()
{
  digitalWrite(LED, HIGH);
  for (pos = 0; pos <= 180; pos += 1)
  { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    roll_servo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15ms for the servo to reach the position
  }
  digitalWrite(LED, LOW);
  for (pos = 180; pos >= 0; pos -= 1)
  {                        // goes from 180 degrees to 0 degrees
    roll_servo.write(pos); // tell servo to go to position in variable 'pos'
    delay(15);             // waits 15ms for the servo to reach the position
  }
}

void ResetData()
{
  payload.throttle = 127; // Center | Signal lost position / THR
  payload.pitch = 127;    // Center | Signal lost position / ELV
  payload.roll = 64;      // left | Signal lost position / AIL
  payload.yaw = 127;      // Center | Signal lost position / RUD
}