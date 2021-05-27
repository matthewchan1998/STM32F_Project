//	Author: Ming Chak Chan Matthew
//	Date: 13/12/2020
//	File: Thread.c
//	Description: This file contains three operation and initialization threads

#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "my_headers.h"

void Tilt_Thread (void const *argument); // Declares the tilt switch thread
osThreadId tid_Tilt_Thread; // Give an ID to the thread for referencing when using OS functions
osThreadDef (Tilt_Thread, osPriorityNormal, 1, 0); // Define tilt switch thread

void Button_Thread (void const *argument); // Declares the button thread
osThreadId tid_Button_Thread; // Give an ID to the thread for referencing when using OS functions
osThreadDef (Button_Thread, osPriorityNormal, 1, 0); // Define button thread

void Blink_LED_Thread (void const *argument); // Declares the blink LED thread
osThreadId tid_Blink_LED_Thread; // Give an ID to the thread for referencing when using OS functions
osThreadDef (Blink_LED_Thread, osPriorityNormal, 1, 0); // Define blink LED thread

/*----------------------------------------------------------------------------
 * Thread 1 'Tilt_Thread': Level 1 Light up corresponding LEDs as board tilt
 *---------------------------------------------------------------------------*/
 
// Definition of the initialization thread function of tilt switch thread. This will be called in main.c
int Init_Tilt_Thread (void) {
  tid_Tilt_Thread = osThreadCreate (osThread(Tilt_Thread), NULL); // Creates the thread object and assigns it with dedicated thread ID 
  if(!tid_Tilt_Thread) return(-1); // Checks whether the thread has been created successfully
  return(0);
}

// Definition of the operation of tilt switch thread.
void Tilt_Thread (void const *argument) {
    for(;;) { // Loop the operation code indefinitely
		osSignalWait(0x01,osWaitForever); // Waits for the setting of flag 0x01 so that the thread can run to next line
		Tilt_Switch(); // Calls the function from my_hearders.c
		osThreadYield(); // Suspend this thread and run next avaliable thread that is ready to run. If there is no other avaliable thread, this thread will repeat.
		}
}

/*----------------------------------------------------------------------------
 * Thread 2 'Button_Thread': Switch between tilt switch and blink LED thread
 *---------------------------------------------------------------------------*/
// Definition of the initialization thread function of user switch thread. This will be called in main.c
int Init_Button_Thread (void) {
  tid_Button_Thread = osThreadCreate (osThread(Button_Thread), NULL); // Creates the thread object and assigns it with dedicated thread ID
  if(!tid_Button_Thread) return(-1); // Checks whether the thread has been created successfully
  return(0);
}

// Definition of the operation of user button thread.
void Button_Thread (void const *argument) {
	for(;;) { // Loop the operation code indefinitely
		Update_State(); // Calls the function from my_headers.c
		switch(Thread_State) {
			case 0:
				osSignalClear(tid_Blink_LED_Thread,0x02); // Clear flag 0x02 for the blink LED thread so that it is paused
				osSignalSet(tid_Tilt_Thread,0x01);// Set flag 0x01 to the tilt switch thread so that it can pass wait line next time
				break;
			case 1:
				osSignalClear(tid_Tilt_Thread,0x01); // Clear flag 0x01 for the tilt switch thread so that it is paused
				osSignalSet(tid_Blink_LED_Thread,0x02);// Set flag 0x02 to the blink LED thread so that it can pass wait line next time
				break;
		}
		osThreadYield(); // Suspend this thread and run next avaliable thread that is ready to run. If there is no other avaliable thread, this thread will repeat.
  }
}

/*----------------------------------------------------------------------------
 *    Thread 3 'Blink_LED_Thread': Blink all four LEDs with 0.5s interval
 *---------------------------------------------------------------------------*/
// Definition of the initialization thread function of blink LED thread. This will be called in main.c
int Init_Blink_LED_Thread (void) {
  tid_Blink_LED_Thread = osThreadCreate (osThread(Blink_LED_Thread), NULL); // Creates the thread object and assigns it with dedicated thread ID
  if(!tid_Blink_LED_Thread) return(-1); // Checks whether the thread has been created successfully
  return(0);
}

// Definition of the operation of blink LED thread.
void Blink_LED_Thread (void const *argument) {
  uint8_t LED_on = 1; // Defines parameter for LED on
	uint8_t LED_off = 0; // Defines parameter for LED off
	for(;;){  // Loop the operation code indefinitely
		osSignalWait(0x02,osWaitForever); // Waits for the setting of flag 0x02 so that the thread can run to next line
		Blink_LED(LED_on); // LEDs ON
		osDelay(500); // Uses the built in delay function for the OS to create a 0.5 second delay.
		Blink_LED(LED_off); // LEDs OFF
		osDelay(500); // Uses the built in delay function for the OS to create a 0.5 second delay.
		osThreadYield(); // Suspends this thread and run next avaliable thread that is pending. If there is no avaliable pending thread, this thread will repeat.
	}
}
