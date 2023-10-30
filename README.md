# DIY RC plane using the ATtiny44 with an nRF24L01 as a receiver

* With an old ESky 0404 35MHz 4 channel radio controller to hand,  use the training port to get the PPM signals decoded - Working
* Transmit the signals from the PPM decoding RX-Nano to another RX-Nano to test - Working
* Get ATtiny44 to receive the same signals - Working
* Get ATtiny to drive a servo motor - Working
* Create a circuit to drive the tiny motors - Currently using transistor, but will try using a mosfet to see if I can avoid the voltage drop.
* Get the BMP180 pressure sensor working on ATtiny using soft I2C (SoftI2CMaster) - (because the SDA and SCL are being used for the nrf24l01) - to do later

With my last update the PPM decoder and transmitter is working well.
Now lacking some power in the motor.

## The trasmitter
This is simply an RF-Nano powered by the radio's defual batteries, using a buck converter to convert the 12V to 5V. The signal has been obtained by soldering a wire to the signal pin on the inside of the trainer port, and connecting it to one of the arduino GPIOs.

## The receiver
This is an ATtiny44 with an nRF24L01 powered by a 3.4v LiPo battery.
![circuit diagram](docs/rc-receiver.drawio.svg)


## Refinements
Possibly use a library rather, to decode the PPM signal, that does not make use of `micros()`





