/*
 * LCD_tools.h
 *
 *  Created on: 25.03.2018
 *      Author: Pawel Rogalinski
 */

#ifndef LCD_TOOLS_H_
#define LCD_TOOLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"


// Adres urz�dzenia I2C dla konwertera I2C->LCD zale�y od
// typu u�ytego uk�adu PCF8574
#define PCF8574_ADDRESS    0x20   // Chipset PCF8574   - 0x20
#define PCF8574T_ADDRESS   0x27   // Chipset PCF8574T  - 0x27
#define PCF8574A_ADDRESS   0x38   // Chipset PCF8574A  - 0x38
#define PCF8574AT_ADDRESS  0x3F   // Chipset PCF8574AT - 0x3F


void LCD_init(I2C_HandleTypeDef *hi2c, uint8_t i2c_address, uint8_t cols, uint8_t rows);
uint8_t LCD_getCols(void);
uint8_t LCD_retRows(void);
uint8_t LCD_getX(void);
uint8_t LCD_getY(void);
void LCD_clear(void);
void LCD_clearLine(uint8_t y);
void LCD_home(void);
void LCD_displayOn(void);
void LCD_displayOff(void);
void LCD_blinkOn(void);
void LCD_blinkOff(void);
void LCD_cursorOn(void);
void LCD_cursorOff(void);
void LCD_setCursor(uint8_t col, uint8_t row);
void LCD_scrollLeft(void);
void LCD_scrollRight(void);
void LCD_createChar(uint8_t location, uint8_t* data);
void LCD_printCustomChar(uint8_t location);
void LCD_printCustomCharXY(uint8_t x, uint8_t y, uint8_t location);
void LCD_printChar(char c);
void LCD_printCharXY(uint8_t x, uint8_t y, char c);
void LCD_printStr(char* str);
void LCD_printStrXY(uint8_t x, uint8_t y, char* str);
void LCD_printf( const char * format, ... );
void LCD_printfXY(uint8_t x, uint8_t y, const char * format, ... );
void LCD_ledOn(void);
void LCD_ledOff(void);


#ifdef __cplusplus
}
#endif

#endif /* LCD_TOOLS_H_ */
