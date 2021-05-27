//	Author: Ming Chak Chan Matthew
//	Date: 13/12/2020
//	File: my_headers.c
//	Description: This file contains the functions which are called in both main.c and Thread.c files

#include "stm32f4xx.h"

/* Definition of global variables */
SPI_HandleTypeDef SPI_Params;	// Defines structure handler for SPI parameters temporarily
uint8_t Thread_State=0; // Defines current running thread; variable set to 0 initially, so that tilt switch thread runs first
uint8_t Button_State=0; // Defines button state
uint8_t X_Reg; // Defines X-axis MS 8-bit integer
uint8_t Y_Reg; // Defines Y-axis MS 8-bit integer
uint8_t data_to_send_receive[1]; // Predefine a single size array to store the required LIS3DSH register address or data in
uint16_t data_size=1; 					 // Set data size so that only one address is accessed in each transaction
uint32_t data_timeout=1000; 		 // Sets max time to wait for the SPI transaction to complete

void Init_LED(void){ // Function to initialise the LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Active clock of Port D

	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Set Port D.12(green LED) I/O direction mode as output mode
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // Set Port D.13(orange LED) I/O direction mode as output mode
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Set Port D.14(red LED) I/O direction mode as output mode
	GPIOD->MODER |= GPIO_MODER_MODER15_0; // Set Port D.15(blue LED) I/O direction mode as output mode
}

void Init_LIS3DSH(void){ // Function to set parameters of SPI, GPIOA and GPIOE for LIS3DSH
	uint8_t si;
	GPIO_InitTypeDef GPIOA_Params; // Declares the structure handle for the parameters of GPIOA
	GPIO_InitTypeDef GPIOE_Params; // Declares the structure handle for the parameters of GPIOE
	
	// Code to initialise SPI
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Activate clock for SPI1
	SPI_Params.Instance = SPI1; // Selects SPI base address for the SPI interface
	SPI_Params.Init.Mode = SPI_MODE_MASTER; // Sets the operating mode for STM32F407 as master
	SPI_Params.Init.NSS = SPI_NSS_SOFT; // Sets the slave to be controlled by software
	SPI_Params.Init.Direction = SPI_DIRECTION_2LINES; // Sets the SPI to two way transmission
	SPI_Params.Init.DataSize = SPI_DATASIZE_8BIT; // Limits each transmission data packet size to 8-bit
	SPI_Params.Init.CLKPolarity = SPI_POLARITY_HIGH; // Sets the SPI clock line to high idle polarity
	SPI_Params.Init.CLKPhase = SPI_PHASE_2EDGE; // Sets SPI clock active edge for bit capture to be sampled on the second transition of the clock line
	SPI_Params.Init.FirstBit = SPI_FIRSTBIT_MSB; // Sets data transmission from MSB initially
	SPI_Params.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // Sets the baud rate prescaler to divide the main APB2 clock (84MHz) by 32 to give a SPI clock of 2.625MHz.
	HAL_SPI_Init(&SPI_Params); // Configures the SPI using the specified parameters
	
	// Code to initialise pins 5-7 of GPIOA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Activate clock for GPIOA
	GPIOA_Params.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; // Choose pins 5,6 and 7 of GPIOA
	GPIOA_Params.Alternate = GPIO_AF5_SPI1; // Choose alternate function 5 which corresponds to SPI1
	GPIOA_Params.Mode = GPIO_MODE_AF_PP; // Specify alternate function push-pull operating mode for the selected pins
	GPIOA_Params.Speed = GPIO_SPEED_FAST; // Use fast speed for the selected pins
	GPIOA_Params.Pull = GPIO_NOPULL; // Choose no pull-up or pull-down activation for the selected pins
	HAL_GPIO_Init(GPIOA, &GPIOA_Params); // Sets GPIOA into the modes specified in GPIOA_Params
	
	// Code to initialise pin 3 of GPIOE
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Activate clock for GPIOE
	GPIOE_Params.Pin = GPIO_PIN_3; // Choose pin 3 of GPIOE
	GPIOE_Params.Mode = GPIO_MODE_OUTPUT_PP; // Choose normal push-pull operating mode
	GPIOE_Params.Speed = GPIO_SPEED_FAST; // Use fast speed for the selected pin
	GPIOE_Params.Pull = GPIO_PULLUP; // Choose pull-up activation for the selected pin
	HAL_GPIO_Init(GPIOE, &GPIOE_Params); // Sets GPIOE into the modes specified in GPIOE_Params
	
	__HAL_SPI_ENABLE(&SPI_Params); // Activate SPI
	
	// Write a new value to control register 4 of the LIS3DSH
	data_to_send_receive[0] = 0x20; // Address for control register 4 on LIS3DSH
	GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the register to be read on the LIS3DSH
	data_to_send_receive[0] = 0x13; // Set register value to give a sample rate of 3.125Hz, continuous update and disable z-axis 
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the new register value to the LIS3DSH through the SPI channelGPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
	GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
	
	for(si=1;si<30;si++){}
}

void Tilt_Switch(void){ // Function to initialise X&Y axis and light up LEDs corresponding to the tilt direction of the board
	data_to_send_receive[0] = 0x80|0x29; // Address for the MSB X-axis (H) data register on the LIS3DSH
	GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the register to be read on the LIS3DSH
	data_to_send_receive[0] = 0x00; // Set a blank address to wait for data input
	HAL_SPI_Receive(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Get the data from the LIS3DSH through the SPI channel
	GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
	X_Reg = data_to_send_receive[0]; // Read the data from the SPI data array into our internal variable.
	
	data_to_send_receive[0] = 0x80|0x2B; // Address for the MSB Y-axis (H) data register on the LIS3DSH
	GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the register to be read on the LIS3DSH
	data_to_send_receive[0] = 0x00; // Set a blank address to wait for data input
	HAL_SPI_Receive(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Get the data from the LIS3DSH through the SPI channel
	GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
	Y_Reg = data_to_send_receive[0]; // Read the data from the SPI data array into our internal variable.
	
	if(X_Reg<=0xFA &&  X_Reg>=0x80){ // When the board is tilted facing green LED
		GPIOD->BSRR |= (1<<12); // Turn on green LED
		GPIOD->BSRR |= (1<<(14+16)); // Turn off red LED	
		if(Y_Reg<=0xFA && Y_Reg>=0x80){ // When the board is tilted facing blue LED
			GPIOD->BSRR |= (1<<15); // Turn on blue LED
			GPIOD->BSRR |= (1<<(13+16)); // Turn off orange LED
		} else if(Y_Reg>=0x05 && Y_Reg<=0x80){ // When the board is tilted facing orange LED
			GPIOD->BSRR |= (1<<13); // Turn on orange LED
			GPIOD->BSRR |= (1u<<(15+16)); // Turn off blue LED
		} 
	} else if(X_Reg>=0x05 && X_Reg<=0x80){ // When the board is tilted facing red LED 
			GPIOD->BSRR |= (1<<14); // Turn on red LED
			GPIOD->BSRR |= (1<<(12+16)); // Turn off green LED
			if(Y_Reg<=0xFA && Y_Reg>=0x80){ // When the board is tilted facing blue LED
				GPIOD->BSRR |= (1<<15); // Turn on blue LED
				GPIOD->BSRR |= (1<<(13+16)); // Turn off orange LED
			} else if(Y_Reg>=0x05 && Y_Reg<=0x80){ // When the board is tilted facing orange LED 
					GPIOD->BSRR |= (1<<13); // Turn on orange LED
					GPIOD->BSRR |= (1u<<(15+16)); // Turn off blue LED
			} 
	} else if(Y_Reg<=0xFA && Y_Reg>=0x80){ // When the board is tilted facing blue LED 
			GPIOD->BSRR |= (1<<15); // Turn on blue LED
			GPIOD->BSRR |= (1<<(13+16)); // Turn off orange LED
	} else if(Y_Reg>=0x05 && Y_Reg<=0x80){ // When the board is tilted facing orange LED
			GPIOD->BSRR |= (1<<13); // Turn on orange LED
			GPIOD->BSRR |= (1u<<(15+16)); // Turn off blue LED
	} else { // When the board sits horizontal or tilted but in X or Y-axis deadzone
		GPIOD->BSRR |= (1<<(12+16));	// Turn off Green LED
		GPIOD->BSRR |= (1<<(13+16));	// Turn off Orange LED
		GPIOD->BSRR |= (1<<(14+16));	// Turn off Red LED
		GPIOD->BSRR |= (1u<<(15+16));	// Turn off Blue LED
	}
}

void Update_State(void){ // Function update current running thread and button state
	if((GPIOA->IDR & 0x00000001) == 0x00000001){ // Change state when button is pressed
		Button_State = 1;
	}
	if(Button_State == 1 && ((GPIOA->IDR & 0x00000001) != 0x00000001)){ // When button is pressed and released 
		switch(Thread_State) {
			case 1:
				Button_State = 0; // Resets button state
				Thread_State = 0; // Switch thread state from blink LED thread to tilt switch thread
				break;
			case 0:
				Button_State = 0; // Resets button state
				Thread_State = 1; // Switch thread state from tilt switch thread to blink LED thread 
				break;
		}
	}
}
	
void Blink_LED(uint8_t LED_state){ // Function to blink all LEDs
	switch(LED_state) {
		case 1:
			GPIOD->BSRR |= (1<<12);	// Turn on Green LED
			GPIOD->BSRR |= (1<<13);	// Turn on Orange LED
			GPIOD->BSRR |= (1<<14);	// Turn on Red LED
			GPIOD->BSRR |= (1<<15);	// Turn on Blue LEDs
			break;
		case 0:
			GPIOD->BSRR |= (1<<(12+16));	// Turn off Green LED
			GPIOD->BSRR |= (1<<(13+16));	// Turn off Orange LED
			GPIOD->BSRR |= (1<<(14+16));	// Turn off Red LED
			GPIOD->BSRR |= (1u<<(15+16));	// Turn off Blue LED
			break;
	}
}
