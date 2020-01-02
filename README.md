![Photo](./photo.jpg)

This is the firmware for a simple thermocromic selfie camera based on

- A OV7670 camera module (without FIFO),
- A STM32F103C8T6 microcontroller, and
- A LTP1245 thermal printer module.

Apart from that, only a level shifter for the thermal printer (which needs 5 V signals), a motor driver for its stepper, a pullup for its thermistor, and a MOSFET for turning all power off are needed.

More details can be found [here](https://25120.org/post/inverse_thermal_camera/).
