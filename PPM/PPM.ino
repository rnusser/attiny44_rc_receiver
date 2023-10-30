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
// byte addresses[][6] = {"1Node", "2Node"};

int pos = 0; // variable to store the servo position
bool send = true;

unsigned long int a, b, c;
int cnt;
int x[15], ch1[15], ch[5], i;
// specifing  arrays and variables to store values
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
Signal data_last;

void ResetData()
{
    data.throttle = 90; // Center | Signal lost position / THR
    data.pitch = 90;    // Center | Signal lost position / RUD
    data.roll = 90;     // Center | Signal lost position / AIL
    data.yaw = 90;      // Center | Signal lost position / ELE
}

// D2 or D3 next to GND (2 or 3)
// four //// mean look at them later first lets keep it simple
void setup()
{
    Serial.begin(9600);
    Serial.println("Booting");
    pinMode(2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(2), read_me, RISING);

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
    // number of bytes we need to transmit a float
    //// radio.setPayloadSize(sizeof(payload)); // char[7] & uint8_t datatypes occupy 8 bytes

    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(address); // always uses pipe 0

    radio.stopListening();
    // ResetData();
    // radio.write(&data, sizeof(data));
    ppm_cnt = 0;
}

// roll = ch[1]
// pitch = ch[2]
// throttle = ch[3]
// yaw = ch[4]

void loop()
{
    read_rc();

    // int mapJoystickValues(int val, int lower, int middle, int upper, bool isAngle, bool reverse)
    data.roll = mapJoystickValues(ch[1], 44, 488, 944, true, false);  // "true" is if we want an angle in degrees
    data.pitch = mapJoystickValues(ch[2], 64, 500, 940, true, false); // "flase" for servo not in reverse direction
    data.yaw = mapJoystickValues(ch[4], 36, 500, 936, true, true);
    data.throttle = mapJoystickValues(ch[3], 104, 444, 792, false, false);
    data.ppm_cnt = ppm_cnt;

    send = send | abs(data_last.roll - data.roll) > 4 ? true : false;
    send = send | abs(data_last.pitch - data.pitch) > 4 ? true : false;
    send = send | abs(data_last.yaw - data.yaw) > 4 ? true : false;
    send = send | abs(data_last.throttle - data.throttle) > 4 ? true : false;

    Serial.print(data.throttle);
    Serial.print(",");
    Serial.print(data.pitch);
    Serial.print(",");
    Serial.print(data.roll);
    Serial.print(",");
    Serial.print(data.yaw);
    Serial.print(",");
    Serial.println(send);

    if (send)
    {
        radio.write(&data, sizeof(data));
    }
    else
    {
        radio.write(&data_last, sizeof(data_last));
    }
    data_last = data;
    send = false;
    delay(10);
}

void read_me()
{
    // this code reads value from RC reciever from PPM pin (Pin 2 or  3)
    // this code gives channel values from 0-1000 values
    a = micros(); // store time value a when pin value falling
    c = a - b;    // calculating  time inx between two peaks
    b = a;        //
    x[i] = c;     // storing 15 value in  array
    i = i + 1;
    if (i == 15)
    {
        for (int j = 0; j < 15; j++)
        {
            ch1[j] = x[j];
        }
        i = 0;
    }

    if (ppm_cnt < 250)
        ppm_cnt += 1;
    else
        ppm_cnt = 0;
} // copy  store all values from temporary array another array after 15 reading

void read_rc()
{
    int i, j, k = 0;
    for (k = 14; k > -1; k--)
    {
        if (ch1[k] > 10000)
        {
            j = k;
            cnt = j;
        }
    } // detecting separation  space 10000us in that another array
    for (i = 1; i <= 4; i++)
    {
        ch[i] = (ch1[i + j] - 1000);
    }
}

int mapJoystickValues(int val, int lower, int middle, int upper, bool isAngle, bool reverse)
{
    val = constrain(val, lower, upper);

    if (val < middle)
        val = map(val, lower, middle, 0, isAngle ? 90 : 128);
    else
        val = map(val, middle, upper, isAngle ? 90 : 128, isAngle ? 180 : 255);

    return (reverse ? 255 - val : val);
}
