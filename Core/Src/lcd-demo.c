/*
 * lcd-demo.c
 *
 *  Created on: 30.03.2018
 *      Author: pawel
 */

#include "LCD_tools.h"

extern I2C_HandleTypeDef hi2c1;

void createCustomChars(void);

void setup()
{
	LCD_init(&hi2c1, PCF8574T_ADDRESS, 16, 2);
	LCD_ledOn();
	createCustomChars();
}

void loop()
{
	uint8_t i;

	LCD_clear();

	// LCD_printStr
	LCD_printStr("LCD_tools DEMO");
	HAL_Delay(1000);

	// LCD_ledOn, LCD_ledOff
	LCD_printStrXY(0, 1, "ledOn/Off");
	HAL_Delay(500);
	for (i = 0; i < 2; i++)
	{
		HAL_Delay(400);
		LCD_ledOff();
		HAL_Delay(400);
		LCD_ledOn();
	}
	LCD_clearLine(1);
	HAL_Delay(1000);

	// LCD_displayOn, LCD_displayOff
	LCD_printStrXY(0, 1, "displayOn/Off");
	HAL_Delay(500);
	for (i = 0; i < 2; i++)
	{
		HAL_Delay(400);
		LCD_displayOff();
		HAL_Delay(400);
		LCD_displayOn();
	}
	LCD_clearLine(1);
	HAL_Delay(1000);

	// LCD_cursorOn, LCD_cursorOff, LCD_setCursor
	LCD_printStrXY(0, 1, "cursorOn");
	LCD_cursorOn();
	HAL_Delay(1000);
	LCD_printStrXY(0, 1, "cursorSet");
	for (i = LCD_getX(); i < LCD_getCols(); i++)
	{
		LCD_setCursor(i, 1);
		HAL_Delay(500);
	}
	LCD_printStrXY(0, 1, "cursorOff");
	LCD_cursorOff();
	HAL_Delay(1000);
	LCD_clearLine(1);
	HAL_Delay(500);

	// LCD_blinkOn, LCD_blinkOff
	LCD_printStrXY(0, 1, "blinkOn");
	LCD_blinkOn();
	HAL_Delay(2000);
	LCD_printStrXY(0, 1, "blinkOff");
	LCD_blinkOff();
	HAL_Delay(1000);
	LCD_clearLine(1);
	HAL_Delay(1000);

	// LCD_scrollLeft, LCD_scrollRight
	LCD_printStrXY(0, 1, "scrollLeft");
	HAL_Delay(1000);
	for (i = 0; i < 13; i++)
	{
		LCD_scrollLeft();
		HAL_Delay(200);
	}
	LCD_printStrXY(0, 1, "scrollRight");
	for (i = 0; i < 19; i++)
	{
		LCD_scrollRight();
		HAL_Delay(200);
	}
	LCD_clearLine(1);
	HAL_Delay(500);
	LCD_printStrXY(0, 1, "home");
	HAL_Delay(1000);
	LCD_home();
	HAL_Delay(1000);
	LCD_clearLine(1);
	LCD_printStrXY(0, 1, "clear");
	HAL_Delay(500);
	LCD_clear();
	HAL_Delay(1000);

	// LCD_printChar, LCD_printStr, LCD_printf
	LCD_printStr("Custom chars\n");
	HAL_Delay(500);
	for (i = 1; i <= 8; i++)
	{
		LCD_printf("\rchar %d ->  ", i);
		LCD_printChar(i);
		HAL_Delay(500);
	}
	HAL_Delay(500);
	LCD_printStr("\r \x1 \x2 \x3 \x4 \x5 \x6 \x7 \x8");
	HAL_Delay(4000);
}

void createCustomChars(void)
{
	LCD_createChar(1, (uint8_t[8]){
						  0b00000,
						  0b00000,
						  0b00000,
						  0b01110,
						  0b01110,
						  0b00000,
						  0b00000,
						  0b00000});
	LCD_createChar(2, (uint8_t[8]){
						  0b00000,
						  0b01110,
						  0b01110,
						  0b00000,
						  0b00000,
						  0b01110,
						  0b01110,
						  0b00000});
	LCD_createChar(3, (uint8_t[8]){
						  0b01110,
						  0b01110,
						  0b00000,
						  0b01110,
						  0b01110,
						  0b00000,
						  0b01110,
						  0b01110});
	LCD_createChar(4, (uint8_t[8]){
						  0b00000,
						  0b11011,
						  0b11011,
						  0b00000,
						  0b00000,
						  0b11011,
						  0b11011,
						  0b00000});
	LCD_createChar(5, (uint8_t[8]){
						  0b11011,
						  0b11011,
						  0b00000,
						  0b01110,
						  0b01110,
						  0b00000,
						  0b11011,
						  0b11011});
	LCD_createChar(6, (uint8_t[8]){
						  0b11011,
						  0b11011,
						  0b00000,
						  0b11011,
						  0b11011,
						  0b00000,
						  0b11011,
						  0b11011});
	LCD_createChar(7, (uint8_t[8]){
						  0b10101,
						  0b10101,
						  0b00000,
						  0b00100,
						  0b00100,
						  0b00000,
						  0b10101,
						  0b10101});
	LCD_createChar(8, (uint8_t[8]){
						  0b10101,
						  0b10101,
						  0b00000,
						  0b01010,
						  0b01010,
						  0b00000,
						  0b10101,
						  0b10101});
}
