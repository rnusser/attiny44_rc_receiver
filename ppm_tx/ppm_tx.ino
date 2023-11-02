#include <SPI.h>
#include "RF24.h"

// RF-NANO uses the following SPI pins
// D9  - CSN
// D10 - CE
// D11 - MOSI
// D12 - MISO
// D13 - SCK

#define CE_PIN 10
#define CSN_PIN 9
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

// Let these addresses be used for the pair
byte address[6] = "1Node";

int pos = 0; // variable to store the servo position

unsigned long int current, prev, period;
// specify arrays and variables to store values
int x[15], ch1[15], channels[5], i;
uint8_t ppm_cnt;

// uint8_t = byte
struct Signal
{
    uint8_t throttle;
    uint8_t pitch;
    uint8_t roll;
    uint8_t yaw;
    uint8_t ppm_cnt;
};
Signal data;

void ResetData()
{
    data.throttle = 127; // Center | Signal lost position / THR
    data.pitch = 127;    // Center | Signal lost position / RUD
    data.roll = 64;      // Center | Signal lost position / AIL
    data.yaw = 127;      // Center | Signal lost position / ELE
}

// D2 or D3 next to GND (2 or 3)
void setup()
{
    Serial.begin(9600);
    pinMode(2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(2), read_me, FALLING);

    // initialize the transceiver on the SPI bus
    if (!radio.begin())
    {
        Serial.println("radio hardware is not responding!!");
        while (1)
        {
        } // hold in infinite loop
    }
    //// radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit
    //// radio.setPayloadSize(sizeof(payload)); // char[7] & uint8_t datatypes occupy 8 bytes

    radio.openWritingPipe(address);
    radio.stopListening();
    ResetData();
    ppm_cnt = 0;
}

void loop()
{
    read_rc();

    delay(100);

    // int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
    data.roll = mapJoystickValues(channels[1], 44, 488, 944, false);
    data.pitch = mapJoystickValues(channels[2], 64, 500, 940, false); // "true" or "false" for servo direction
    data.throttle = mapJoystickValues(channels[3], 104, 444, 792, false);
    data.yaw = mapJoystickValues(channels[4], 36, 500, 936, true);
    data.ppm_cnt = ppm_cnt;

    Serial.print("\ pitch: ");
    Serial.print(channels[2]);
    Serial.print(" -> ");
    Serial.print(data.pitch);

    Serial.print("\ roll: ");
    Serial.print(channels[1]);
    Serial.print(" -> ");
    Serial.print(data.roll);

    Serial.print("\ throttle: ");
    Serial.print(channels[3]);
    Serial.print(" -> ");
    Serial.print(data.throttle);

    Serial.print("\	yaw: ");
    Serial.print(channels[4]);
    Serial.print(" -> ");
    Serial.print(data.yaw);

    Serial.print(" ppm_cnt: ");
    Serial.print(data.ppm_cnt);
    Serial.print('\n');

    radio.write(&data, sizeof(data));
}


void read_me()
{
    // this code reads value from RC reciever from PPM pin (Pin 2 or  3)
    // this code gives channel values from 0-1000
    current = micros(); // store time value when pin value falls
    period = current - prev;    // calculating time inbetween the two peaks
    prev = current;        
    x[i] = period;     
    i = i + 1;

    if (i == 15)  // we will store 15 values in the array
    {
        for (int j = 0; j < 15; j++)
        {
            ch1[j] = x[j];
        }
        i = 0;
    }

    if (ppm_cnt < 255)
    {
        ppm_cnt += 1;
    }
    else
    {
        ppm_cnt = 0;
    }
}


void read_rc()
{
    int i, j, k = 0;
    // detecting separation space of 10000us in the array
    for (k = 14; k > -1; k--)
    {
        if (ch1[k] > 10000)
        {
            j = k;
        }
    } 
    // assign 6 channel values after separation space
    for (i = 1; i <= 6; i++)
    {
        channels[i] = (ch1[i + j] - 1000);
    }
}

int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
    val = constrain(val, lower, upper);
    if (val < middle)
        val = map(val, lower, middle, 0, 128);
    else
        val = map(val, middle, upper, 128, 255);
    return (reverse ? 255 - val : val);
}
