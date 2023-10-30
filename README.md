# DIY RC plane using ATtiny44 as receiver with a Nrf24l01

This is the start....

* With an old ESky 0404 35MHz 4 channel radio controller to hand,  use the training port to get the PPM signals decoded - Done
* Transmit the signals from the PPM decoding RX-Nano to another RX-Nano to test - Done
* Get ATtiny44 to receive the same signals - Done
* Get ATtiny to drive a servo motor - Done
* Create a circuit to drive the tiny motors - Currently using transostor, but will try using a mosfet
* Get the BMP180 pressure sensor working on ATtiny using soft I2C (SoftI2CMaster) as the SDA and SCL is being used for the nrf24l01 - to do later

With my last update the PPM decoder and transmitter is working well.

## The trasmitter
This is simply an RX-nano powered by the radio's defual battery power source, using a buck converter to convert the 12V to 5V. The signal has been obtained by soldering a wire to the signal pin of the trainer port on the inside.

## The receiver
This is an ATtiny44 with an nRF24L01 powered by a 3.4v LiPo battery.
![circuit diagram](docs/rc-receiver.drawio.svg)


## Refinements
Possibly use a library rather, to decode the PPM signal, that does not make use of `micros()`





