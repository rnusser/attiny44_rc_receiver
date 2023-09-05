#include <SPI.h>
#include "RF24.h"
#include "nRF24L01.h"

#define CE_PIN 10
#define CSN_PIN 9
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

// Let these addresses be used for the pair
byte address[6] = "1Node";
// byte addresses[][6] = {"1Node", "2Node"};
uint8_t ppm_cnt, prev_cnt, missing;

struct PayloadStruct
{
    uint8_t throttle;
    uint8_t pitch;
    uint8_t roll;
    uint8_t yaw;
    uint8_t ppm_cnt;
};
PayloadStruct payload;

void setup()
{
    Serial.begin(9600);
    radio.begin();
    radio.openReadingPipe(1, address);
    radio.startListening();
    Serial.print("Lets go");
}

void loop()
{
    Serial.print(".");
    unsigned long started_waiting_at = micros();
    bool timeout = false;

    while (radio.available())
    { // PayloadStruct received;
        uint8_t bytes = radio.getPayloadSize();
        //  or sizeof(received) instead of bytes
        radio.read(&payload, sizeof(payload)); // get incoming payload
        Serial.print("Received ");
        Serial.print(byte); // print the size of the payload
        Serial.print(" bytes -> throttle: ");
        Serial.print(payload.throttle); // print the payload's value
        Serial.print(", pitch: ");
        Serial.print(payload.pitch); // print the payload's value
        Serial.print(", roll: ");
        Serial.print(payload.roll); // print the payload's value
        Serial.print(", yaw: ");
        Serial.print(payload.yaw); // print the payload's value
        Serial.print(", PPM count: ");
        Serial.println(payload.ppm_cnt); // print the payload's value
        ppm_cnt = payload.ppm_cnt;
    }
    missing = (prev_cnt == ppm_cnt) ? missing += 1 : 0;

    /*
    if (prev_cnt == ppm_cnt)
    {
        missing += 1;
    }
    else
    {
        missing = 0;
    }
    */

    if (missing > 10)
    {
        // Time to fly in a circle
        Serial.println("Missing too many PPM signals");
        missing = 0;
    }
    prev_cnt = ppm_cnt;
    Serial.print(".");
    delay(100);
}
