//	Author: Ming Chak Chan Matthew
//	Date: 13/12/2020
//	File: Thread.h
//	Description: This is a header file for the declarations of three operation and initialization threads

extern int Init_Tilt_Thread (void); // Standard format to declare the function to initialise the main thread function.
void Tilt_Thread (void const *argument); // Standard format to declare the main thread function.

extern int Init_Button_Thread (void); // Standard format to declare the function to initialise the main thread function.
void Button_Thread (void const *argument); // Standard format to declare the main thread function.

extern int Init_Blink_LED_Thread (void); // Standard format to declare the function to initialise the main thread function.
void Blink_LED_Thread (void const *argument); // Standard format to declare the main thread function.

