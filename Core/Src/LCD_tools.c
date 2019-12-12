/*
 * LCD_tools.c
 *
 *  Created on: 25.03.2018
 *      Author: Pawel Rogalinski
 */

#include "LCD_tools.h"
#include "stdarg.h"


/*
 * DWT (Data watchpoint trigger) jest to sprz�towy licznik zegarowy,
 * kt�ry zlicza ilo�� cykli wykonanych przez CPU.
 * Mo�na go u�y� do bardzo dok�adnego pomiaru czasu wykonywania
 * fragment�w kodu lub do generowania op�nie� na poziomie mikrosekund.
 * UWAGA: Licznik dzia�a tylko w trybie debuggowania.
 * Poni�sze funkcje mog� byc zak��cone przez u�ycie debuggera
 * lib innych narz�dzi do optymalizacji kodu programu (code profilers)
 *
 * UWAGA: Je�eli licznik DWT nie ma byc u�ywany do generowania
 * op�nie� to zakomentuj poni�sz� definicj� symbolu USE_DWT_TOOLS.
 */
//#define USE_DWT_TOOLS


#ifdef USE_DWT_TOOLS
#include "DWT_tools.h"
#endif  /* USE_DWT_TOOLS */

static inline void LCD_delayUs(volatile uint32_t micros) {
#ifdef USE_DWT_TOOLS
	DWT_Delay_us(micros);
#else
	micros *= (HAL_RCC_GetHCLKFreq() / 1000000 / 6);
	while (micros--);
#endif
}


#define LCD_PCF8574_RS       0x01    // D0 on PCF8574 chipset
#define LCD_PCF8574_RW       0x02    // D1 on PCF8574 chipset
#define LCD_PCF8574_ENABLE   0x04    // D2 on PCF8574 chipset
#define LCD_PCF8574_LED      0x08    // D3 on PCF8574 chipset


// Commands
#define HD44780_CLEARDISPLAY        0x01
#define HD44780_RETURNHOME          0x02
#define HD44780_ENTRYMODESET        0x04
#define HD44780_DISPLAYCONTROL      0x08
#define HD44780_CURSORSHIFT         0x10
#define HD44780_FUNCTIONSET         0x20
#define HD44780_SETCGRAMADDR        0x40
#define HD44780_SETDDRAMADDR        0x80

// Flags for display entry mode
#define HD44780_ENTRYRIGHT          0x00
#define HD44780_ENTRYLEFT           0x02
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYSHIFTDECREMENT 0x00

// Flags for display on/off control
#define HD44780_DISPLAYON           0x04
#define HD44780_CURSORON            0x02
#define HD44780_BLINKON             0x01

// Flags for display/cursor shift
#define HD44780_DISPLAYMOVE         0x08
#define HD44780_CURSORMOVE          0x00
#define HD44780_MOVERIGHT           0x04
#define HD44780_MOVELEFT            0x00

// Flags for function set
#define HD44780_8BITMODE            0x10
#define HD44780_4BITMODE            0x00
#define HD44780_2LINE               0x08
#define HD44780_1LINE               0x00
#define HD44780_5x10DOTS            0x04
#define HD44780_5x8DOTS             0x00


// Private HD44780 structure
typedef struct {
	I2C_HandleTypeDef * I2C_HandlePtr;
	uint8_t PCF8574_Address;
	uint8_t PCF8574_Port;
	uint8_t DisplayControl;
	uint8_t Rows;
	uint8_t Cols;
	uint8_t X;
	uint8_t Y;
} LCD_Options_t;


// Private variable
static LCD_Options_t LCD_Opts;


// Private functions
#define PCF8574Port  (LCD_Opts.PCF8574_Port)

static inline void LCD_PCF8574_writePort(void) {
	HAL_I2C_Master_Transmit(LCD_Opts.I2C_HandlePtr, LCD_Opts.PCF8574_Address,
			&PCF8574Port, 1, 100);
}

void LCD_sendHalfbyte(uint8_t c){
	PCF8574Port = (PCF8574Port & 0x0f) | (c & 0xf0) | LCD_PCF8574_ENABLE;
	LCD_PCF8574_writePort();
	PCF8574Port &= ~LCD_PCF8574_ENABLE;
	LCD_PCF8574_writePort();
}

void LCD_sendCmd (char cmd){
	PCF8574Port &= ~LCD_PCF8574_RS;
	LCD_PCF8574_writePort();
	LCD_sendHalfbyte(cmd);
	LCD_sendHalfbyte(cmd<<4);
	LCD_delayUs(45);
}

void LCD_sendData (char data){
	PCF8574Port |= LCD_PCF8574_RS;
	LCD_PCF8574_writePort();
	LCD_sendHalfbyte(data);
    LCD_sendHalfbyte(data<<4);
    LCD_delayUs(45);
}




// Public functions
void LCD_init(I2C_HandleTypeDef *hi2c, uint8_t i2c_address, uint8_t cols, uint8_t rows) {
	// Initialize delay
#ifdef USE_DWT_TOOLS
	DWT_Init();
#endif  // USE_DWT_TOOLS

	LCD_Opts.I2C_HandlePtr = hi2c;
	LCD_Opts.PCF8574_Address = i2c_address << 1;
	LCD_Opts.PCF8574_Port = 0;
	LCD_Opts.Cols = cols;
	LCD_Opts.Rows = rows;

	// Set cursor pointer to beginning for LCD
    LCD_Opts.X = 0;
    LCD_Opts.Y = 0;

    // Set beginning state for LCD
	LCD_Opts.DisplayControl = HD44780_DISPLAYON;

	// Wait at least 40ms
	LCD_delayUs(45000);

	// Try to set 4bit mode
	LCD_sendHalfbyte(0x30);
	LCD_delayUs(4500);

	// Second try
	LCD_sendHalfbyte(0x30);
	LCD_delayUs(150);

	// Third goo!
	LCD_sendHalfbyte(0x30);
	LCD_delayUs(150);

	// Set 4-bit interface
	LCD_sendHalfbyte(0x20);
	LCD_delayUs(150);

	// Set # lines, font size, etc.
	if (rows>1){
		LCD_sendCmd(HD44780_FUNCTIONSET | HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_2LINE);
	} else {
		LCD_sendCmd(HD44780_FUNCTIONSET | HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE);
	}

	// Turn the display on
	LCD_displayOn();

	// Clear lcd
	LCD_clear();

	// Default font directions
	LCD_sendCmd(HD44780_ENTRYMODESET | HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT);
}

uint8_t LCD_getCols(void){
	return LCD_Opts.Cols;
}
uint8_t LCD_retRows(void){
	return LCD_Opts.Rows;
}

uint8_t LCD_getX(void){
	return LCD_Opts.X;
}
uint8_t LCD_getY(void){
	return LCD_Opts.Y;
}

void LCD_clear(void) {
	LCD_sendCmd(HD44780_CLEARDISPLAY);
	LCD_delayUs(2000);
	LCD_Opts.X = LCD_Opts.Y = 0;
}

void LCD_clearLine(uint8_t y){
	LCD_setCursor(0,y);
	for(uint8_t x=0; x<LCD_getCols(); x++)
		LCD_printChar(' ');
}

void LCD_home(void) {
	LCD_sendCmd(HD44780_RETURNHOME);
	LCD_delayUs(2000);
	LCD_Opts.X = LCD_Opts.Y = 0;
}

void LCD_displayOn(void) {
	LCD_Opts.DisplayControl |= HD44780_DISPLAYON;
	LCD_sendCmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_displayOff(void) {
	LCD_Opts.DisplayControl &= ~HD44780_DISPLAYON;
	LCD_sendCmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_blinkOn(void) {
	LCD_Opts.DisplayControl |= HD44780_BLINKON;
	LCD_sendCmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_blinkOff(void) {
	LCD_Opts.DisplayControl &= ~HD44780_BLINKON;
	LCD_sendCmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_cursorOn(void) {
	LCD_Opts.DisplayControl |= HD44780_CURSORON;
	LCD_sendCmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_cursorOff(void) {
	LCD_Opts.DisplayControl &= ~HD44780_CURSORON;
	LCD_sendCmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_setCursor(uint8_t col, uint8_t row) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	if (row >= LCD_Opts.Rows) {
		row = 0;
	}
	LCD_Opts.X = col;
	LCD_Opts.Y = row;
	LCD_sendCmd(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_scrollLeft(void) {
	LCD_sendCmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void LCD_scrollRight(void) {
	LCD_sendCmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void LCD_createChar(uint8_t location, uint8_t *data) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

	// We have 8 locations available for custom characters
	location &= 0x07;
	LCD_sendCmd(HD44780_SETCGRAMADDR | (location << 3));
	for (uint8_t i = 0; i < 8; i++) {
		LCD_sendData(data[i]);
	}
	LCD_sendCmd(HD44780_SETDDRAMADDR | (LCD_Opts.X + row_offsets[LCD_Opts.Y]));
}

void LCD_printCustomChar(uint8_t location) {
	LCD_sendData(location);
	LCD_Opts.X++;
}

void LCD_printCustomCharXY(uint8_t x, uint8_t y, uint8_t location) {
	LCD_setCursor(x, y);
	LCD_sendData(location);
	LCD_Opts.X++;
}

void LCD_printChar(char c) {
	if (c == '\r') {
		LCD_setCursor(0, LCD_Opts.Y);
	} else if (c == '\n') {
		LCD_Opts.Y++;
		LCD_setCursor(LCD_Opts.X, LCD_Opts.Y);
	} else {
		if (LCD_Opts.X >= LCD_Opts.Cols) {
		   LCD_Opts.X = 0;
		   LCD_Opts.Y++;
		   LCD_setCursor(LCD_Opts.X, LCD_Opts.Y);
		}
		LCD_sendData(c);
		LCD_Opts.X++;
	}
}

void LCD_printCharXY(uint8_t x, uint8_t y, char c) {
	LCD_setCursor(x, y);
	LCD_printChar(c);
}

void LCD_printStr(char* str) {
	while (*str) {
		LCD_printChar(*str++);
	}
}

void LCD_printStrXY(uint8_t x, uint8_t y, char* str) {
	LCD_setCursor(x, y);
	LCD_printStr(str);
}

void LCD_printf( const char * format, ... )
{
  char buffer[41];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  LCD_printStr(buffer);
  va_end (args);
}

void LCD_printfXY(uint8_t x, uint8_t y,  const char * format, ... )
{
  char buffer[41];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  LCD_setCursor(x, y);
  LCD_printStr(buffer);
  va_end (args);
}

void LCD_ledOn(void){
	PCF8574Port |= LCD_PCF8574_LED;
	LCD_PCF8574_writePort();
}

void LCD_ledOff(void){
	PCF8574Port &= ~LCD_PCF8574_LED;
	LCD_PCF8574_writePort();
}
