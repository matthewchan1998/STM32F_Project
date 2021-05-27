//	Author: Ming Chak Chan Matthew
//	Date: 13/12/2020
//	File: my_headers.h
//	Description: This is a header file for the declarations of functions called in main.c and Thread.c files

#include "stm32f4xx.h"

/* Declarration of global variables */
extern uint8_t Thread_State; // Declares current running thread globally

/* Declarration of functions */
void Init_LED(void); 		 // Declares function to initialise the LEDs
void Init_LIS3DSH(void); // Declares function to set parameters of SPI, GPIOA and GPIOE for LIS3DSH
void Tilt_Switch(void);  // Declares function to light up LEDs corresponding to the tilt direction of the board
void Update_State(void); // Declares function to update current running thread and button state
void Blink_LED(uint8_t); // Declares function to blink all LEDs

