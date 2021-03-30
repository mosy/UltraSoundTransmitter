// Nivå 2 Koble opp ultarlyd måler, skrive inn kode og teste krets
// Nivå 3 Koble opp display og få opp avstand

#include <LiquidCrystal.h>
#include "rotary.h"// rotary handler

#include <Wire.h> // I²C
#include <Adafruit_MCP4725.h> // DAC


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//Define pins
#define PINA 2
#define PINB 3
#define PUSHB A2
#define trigPin 10
#define echoPin 11

// DAC
Adafruit_MCP4725 dac1;
#define DAC_RESOLUTION    (12) 
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
//Define Main menu and sub menus...
#define MainMenu 0		//MainMenu
#define SubMenu1 1		//Bunnpunkt
#define SubMenu2 2		//DifficultyMenu
#define SubMenu3 3		//LevelMenu
#define SubMenu4 4		//LightMenu
#define SubMenu5 5		//There is no SubMenu5...it is used for volume
#define SubMenu6 6		//LanguageMenu
#define SubMenu7 7		//Summary

const int maxItemSize = 11;  //longest menu item..includes char \0

// Initialize the Rotary object
// Rotary(Encoder Pin 1, Encoder Pin 2, Button Pin) Attach center to ground
Rotary r = Rotary(PINA, PINB, PUSHB);				 // there is no must for using interrupt pins !!

int cursorLine = 0;
int displayFirstLine = 0;

//Define Menu Arrays
char startMenu[][maxItemSize] = {"Bottom Level", "Top Level", "Adjust Voltage", "Light", "Volume", "Language", "Reset", "Oversikt"};
// changed to value char One[][maxItemSize] = {"Reverserende", "Direkte"};
//char Two[][maxItemSize] = {"Easy", "Medium", "Hard", "Return"};
char Three[][maxItemSize] = {"One", "Two", "Three", "Four", "Return"};
char Four[][maxItemSize] = {"ON", "OFF", "Return"};
//No Array needed for volume
char Six[][maxItemSize] = {"English", "Spanish", "Return"};

int Summary[] = {0, 0, 0, 0, 0, 0, 0, 0};		//store selected SubMenu

const int itemsPerScreen = 1;  // one less than max rows..
int menuItems;
char *menuSelected = NULL;
int valueHandler = 0 ; //0=no handling, 1=BunnJustering,5=volumeHandling 
int value = 0;
int menuOption = 0;

int backlightPin = 10;	 //PWM pin
int brightness = 255;
int fadeAmount = 5;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds

// Define variables:
float duration;
float distance;
void setup ()
{
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
	display.display();
	// Clear the buffer.
	display.clearDisplay();
	display.display();
	digitalWrite (PINA, HIGH);		 // enable pull-ups
	digitalWrite (PINB, HIGH);
	digitalWrite (PUSHB, HIGH);

	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	pinMode(backlightPin, OUTPUT);					//backlightPin as an output
	digitalWrite(backlightPin, HIGH);

	Serial.begin(9600);
	Serial.println("dette er en test");
	dac1.begin(0x62);

	//menuItems = sizeof(startMenu) / sizeof(startMenu[0]);
	menuItems = sizeof startMenu / sizeof * startMenu;
	menuSelected = &startMenu[0][0]; //Start Menu
	menuOption = MainMenu; //Main Menu

	startMillis = millis();  //initial start time
}//end of setup

void loop ()
{
	measure();
	Serial.println(distance);
	dac1.setVoltage(distance*100, false);
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(0,0);
	display.print("Avstand = ");
	display.print(distance);
	display.print(" cm");
	display.setCursor(0,0);
	display.display(); // actually display all of the above
}//end loop()

/**********************FUNCTIONS*************************/

void measure()
{

	// Clear the trigPin by setting it LOW:
	digitalWrite(trigPin, LOW);
	delayMicroseconds(5);
	// Trigger the sensor by setting the trigPin high for 10 microseconds:
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
	// Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
	duration = (float)pulseIn(echoPin, HIGH);
	  // Calculate the distance:
	distance = duration * 0.034 / 2;

  // Print the distance on the Serial Monitor (Ctrl+Shift+M):
//	Serial.println(distance);
}

void move_down()
{
	if (cursorLine == (displayFirstLine + itemsPerScreen - 1)) {
		displayFirstLine++;
	}
	//If reached last item...roll over to first item
	if (cursorLine == menuItems) {
		cursorLine = 1;
		displayFirstLine = 1;
	} else {
		cursorLine = cursorLine + 1;
	}
}//end move_down

void move_up()
{
	if ((displayFirstLine == 1) & (cursorLine == 1)) {
		if (menuItems > itemsPerScreen - 1) {
			displayFirstLine = menuItems - itemsPerScreen + 1;
		}
	} else if (displayFirstLine == cursorLine) {
		displayFirstLine--;
	}

	if (cursorLine == 1) {
		if (menuItems > itemsPerScreen - 1) {
			cursorLine = menuItems; //roll over to last item
		}
	} else {
		cursorLine = cursorLine - 1;
	}
}//end move_up

