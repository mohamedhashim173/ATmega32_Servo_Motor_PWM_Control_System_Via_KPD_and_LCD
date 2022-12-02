/*
 * main.c
 *
 *  Created on: Nov 18, 2022
 *      Author: MSAEED99
 */



// Libraries
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include <util/delay.h>

// MCAL
#include "DIO_interface.h"
#include "TIMER1_interface.h"

// HAL
#include "LCD_interface.h"
#include "KEYPAD_interface.h"

// Global Variables
s32 servo_angle;
u32 onTimeVal;

u8 edit_mode = 1;
u8 run_mode = 0;

s8 input[10] = {0};
u8 _index = 0;
u8 num_of_digits = 0;
u32 digit_counter;


// Macros
#define DELAY		2000

// Prototypes
void reset_input(void);
void save_angle_value(void);



/* Lec10_Assignment01:
 * Using Keypad, LCD and Servo Motor, make a system that controls the rotation angle of the servo motor.
 * The user writes the desired angle on the LCD using the keypad,
 * then the servo motor shall rotate to the desired angle.
 */
void main(void)
{
	u8 local_u8PressedKey;

	/********** Timer1 Preferences **********/
	// Set OC1A pin (PinD5) to output for PWM signal
	DIO_voidSetPinDirection(DIO_u8PORTD, DIO_u8PIN5, DIO_u8PIN_OUTPUT);
	// Set Periodic Time of Servo Motor (Top value = OVF value) = 20ms = 20000 counts
	TIMER1_voidSetTopVal(20000);
	// Initialize Timer1
	TIMER1_voidInit();


	/**************** KEYPAD ****************/
	KEYPAD_voidPinInit();


	/***************** LCD ******************/
	LCD_voidInit();

	// Angle input message
	LCD_voidPositionSet(0, 0);
	LCD_voidSendString("Angle  :");
	LCD_voidPositionSet(1, 0);
	LCD_voidSendString("Current:");
	LCD_voidWriteNum(servo_angle);
	LCD_voidPositionSet(0, 8);


	while(1)
	{
		// Store pressed key
		local_u8PressedKey = KEYPAD_u8GetPressedKey();


		// ====================================================
		// Edit Mode
		// ====================================================
		if(1 == edit_mode)
		{
			// Input angle of rotation
			if(local_u8PressedKey >= '0'  && local_u8PressedKey <= '9')
			{
				// Print on LCD
				LCD_voidSendCharacter(local_u8PressedKey);

				// Store in input array
				input[_index] = local_u8PressedKey - '0';
				_index++;
				num_of_digits++;
			}
		}


		// ====================================================
		// Run Mode
		// ====================================================

		// Rotate Servo Motor with the entered value
		if('=' == local_u8PressedKey)
		{
			// Disable Edit Mode
			edit_mode = 0;

			// Calculate input angle
			save_angle_value();

			onTimeVal = (servo_angle * 2300 / 180) + 200;
			TIMER1_voidCompareMatchVal(onTimeVal);

			// Hide Courser
			LCD_voidPositionSet(0, 20);

			// Clear for new input
			reset_input();

		}


		// ====================================================
		// Start over
		// ====================================================
		if('c' == local_u8PressedKey)
		{
			// Clear for new input
			reset_input();
		}

	}
}




// Clear input memories
void reset_input(void)
{
	edit_mode = 1;
	run_mode = 0;

	_index = 0;
	num_of_digits = 0;

	for(s8 i = 0; i < 10; i++)
	{
		input[i] = 0;
	}

	// Clear LCD
	LCD_voidSendCommand(0b00000001);

	// Angle input message
	LCD_voidPositionSet(0, 0);
	LCD_voidSendString("Angle  :");
	LCD_voidPositionSet(1, 0);
	LCD_voidSendString("Current:");
	if(servo_angle > 0)
	{
		LCD_voidWriteNum(servo_angle);
	}
	else
	{
		LCD_voidSendCharacter('0');
	}
	// Degree symbol
	LCD_voidSendCharacter(0b11011111);
	LCD_voidPositionSet(0, 8);
}


// Calculate input value of the servo_angle
void save_angle_value(void)
{
	// Save input to Angle variable
	digit_counter = 1;
	s32 tempVal = 0;

	for(s8 i = num_of_digits - 1; i >= 0; i--)
	{
		tempVal += input[i] * digit_counter;
		digit_counter *= 10;
	}

	// Check the value
	if(tempVal > 180 || tempVal < 0)
	{
		LCD_voidPositionSet(0, 0);
		LCD_voidSendString(" Invalid angle! ");
		LCD_voidPositionSet(1, 0);
		LCD_voidSendString("0 < Angle < 180 ");

		// Hide Courser
		LCD_voidPositionSet(0, 20);

		_delay_ms(DELAY);
	}
	else
	{
		servo_angle = tempVal;
	}
}



