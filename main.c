//	Author: Ming Chak Chan Matthew
//	Date: 13/12/2020
//	File: main.c
//	Description: This is the main file to initialise and start the system

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"
#include "my_headers.h"
#include "Thread.h"

int main (void) {
	SystemCoreClockUpdate();
	osKernelInitialize ();	//Initialize CMSIS-RTOS
	
	// Initialise any peripherals or system components
	Init_LED(); // Function to initialise the LEDs
	Init_LIS3DSH(); // Function to set parameters of SPI, GPIOA and GPIOE for LIS3DSH
	
	// Initialise the main three threads
	Init_Tilt_Thread(); // Thread initialization function of tilt switch thread
	Init_Blink_LED_Thread(); // Thread initialization function of blink LED thread
	Init_Button_Thread(); // Thread initialization function of button thread

  osKernelStart (); // start thread execution 
	
	while(1){}; // While loop to run the program indefinitely
}
