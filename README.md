# STM32F407 Project

This is a program to control the STM32F407 Discovery kit. The main.c file initialise the four LEDs and accelerometer LIS3DSH. 

The main functions for the project are:
1. Lighting up LEDs corresponding to the tilt direction of the board
2. Blinking LEDs with 0.5s interval
3. Switch between the two tasks when user press and release the button.

The flowchart below summarises the three level multi-thread systems, including Tilt_Thread, Blink_LED_Thread and Button_Thread in Thread.c file:

![image](https://user-images.githubusercontent.com/8566958/119899725-6a294f80-bf3b-11eb-89ae-2f78f65144e5.png)
