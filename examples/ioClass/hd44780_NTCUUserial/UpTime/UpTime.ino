// vi:ts=4
// ----------------------------------------------------------------------------
// UpTime - simple demonstration of lcd
// Created by Bill Perry 2017-05-11
// bperrybap@opensource.billsworld.billandterrie.com
//
// This example code is unlicensed and is released into the public domain
// ----------------------------------------------------------------------------
//
// This sketch is for the Noritake CU-U series VFDs, in native serial mode
//
// Sketch will print "UpTime" on top row of lcd
// and will print the amount of time since the Arduino has been reset
// on the second row.
//
// If initialization of the LCD fails and the arduino supports a built in LED,
// the sketch will simply blink the built in LED.
//

#include <SPI.h>                                // optional, include to use h/w spi
#include <hd44780.h>                            // main hd44780 header
#include <hd44780ioClass/hd44780_NTCUUserial.h> // Noritake CU-U serial i/o class header

// constructor parameters:
// lcd([cs], [clock, data])
// If no parameters, then library will use SS, SCK, and MOSI pins
// If cs parameter specified, then use it for chip select then SCK and MOSI
// If <SPI.h> is included and clock & data pins match h/w SPI pins SCK and MOSI,
//  h/w spi will be used
// If h/w spi is not possible, then the code will fall back to bit banging.
//
// NOTE:
//  - Leonardo h/w is "stupid" and does not bring out SS
//     (it only drives an LED)
//  - Leonardo does not bring SPI signals to female headers,
//     they are only on 6 pin ISP header.
//  - ESP8266 is does not use naked constants for digital pin numbers
//
//
// To work around these pin issues in this sketch,
// Leonardo will use uno digital pins for SPI h/w which means it will
// not use h/w spi. All the other boards will use the h/w SPI pins.
// Consult board pinout diagram to see where SS, SCK, and MOSI are available.
//

#if defined(ARDUINO_AVR_LEONARDO) || ( (USB_VID == 0x2341) && (USB_PID == 0x8036) )
const int cs=10, clk=13, data=11; // uno SPI pins (s/w bit banging will be used)
#else
const int cs=SS, clk=SCK, data=MOSI; // use h/w SPI pins on all other boards
#endif
hd44780_NTCUUserial lcd(cs, clk, data); // declare lcd object

// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

void setup()
{
	int status;

	// initialize LCD with number of columns and rows:
	// hd44780 returns a status from begin() that can be used
	// to determine if initalization failed.
	// the actual status codes are defined in <hd44780.h>
	// See the values RV_XXXX
	//
	// looking at the return status from begin() is optional
	// it is being done here to provide feedback should there be an issue
	//

	status = lcd.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		status = -status; // convert negative status value to positive number

		// hd44780 has a fatalError() routine that blinks an led if possible
		// begin() failed so call fatalError() with the error code.
		hd44780::fatalError(status); // does not return
	}

	// Print a message to the LCD
	lcd.print(" UpTime");

	if(LCD_ROWS < 2)
		delay(3000);
}

void loop()
{
static unsigned long lastsecs = -1; // pre-initialize with non zero value
unsigned long secs;
int status;

	secs = millis() / 1000;

	// see if 1 second has passed
	// so the display is only updated once per second
	if(secs != lastsecs)
	{
		lastsecs = secs; // keep track of last seconds

		// set the cursor position to column 0, row 1
		// note: row 1 is the second row from top,
		// since row counting begins with 0
		// if display has only 1 line, it will appear on that line
		status = lcd.setCursor(0, 1);
		if(status) // non zero status means it was unsuccesful
		{
			status = -status; // convert negative status to positive number
			// setCursor() failed so call fatalError() with the error code.
			hd44780::fatalError(status); // does not return
		}

		// print uptime on lcd device: (time since last reset)
		PrintUpTime(lcd, secs);
	}
}

// PrintUpTime(outdev, secs) - print uptime in HH:MM:SS format
// outdev - the device to send output
//   secs - the total number of seconds uptime
//
// This is a fancy output routine.
// outdev is a Print class object which indicates
// where the output should be sent.
// PrintUpTime can be used with any object that uses the Print class.
// This code works with Serial objects, as well as the the hd44780 lcd objects.
// i.e. you can call with Serial: PrintUpTime(Serial, seconds);

void PrintUpTime(Print &outdev, unsigned long secs)
{
unsigned int hr, mins, sec;

	// convert total seconds to hours, mins, seconds
	mins =  secs / 60;	// how many total minutes
	hr = mins / 60;		// how many total hours
	mins = mins % 60;	// how many minutes within the hour
	sec = secs % 60;	// how many seconds within the minute
		
	// print uptime in HH:MM:SS format

	if(hr > 99)
		hr %= 100; // limit hr to 0-99

	// Print class does not support fixed width formatting
	// so insert a zero if number smaller than 10
		
	if(hr < 10)
		outdev.write('0');
	outdev.print((int)hr);
	outdev.write(':');
	if(mins < 10)
		outdev.write('0');
	outdev.print((int)mins);
	outdev.write(':');
	if(sec < 10)
		outdev.write('0');
	outdev.print((int)sec);
}
