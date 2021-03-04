// Nivå 2 Koble opp ultarlyd måler, skrive inn kode og teste krets
// Nivå 3 Koble opp display og få opp avstand

#include <LiquidCrystal.h>
#include "rotary.h"// rotary handler

#include <Wire.h> // I²C
#include <Adafruit_MCP4725.h> // DAC
LiquidCrystal lcd(8,9,4,5,6,7);

//Define pins
#define PINA 2
#define PINB 3
#define PUSHB A2
#define trigPin 10
#define echoPin 11

// DAC
Adafruit_MCP4725 dac1;
#define DAC_RESOLUTION    (12) 

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
	lcd.begin (20, 4);
	welcome();
	lcd.clear();

	//menuItems = sizeof(startMenu) / sizeof(startMenu[0]);
	menuItems = sizeof startMenu / sizeof * startMenu;
	menuSelected = &startMenu[0][0]; //Start Menu
	menuOption = MainMenu; //Main Menu

	startMillis = millis();  //initial start time
	display_menu(menuSelected, menuItems, maxItemSize);
}//end of setup

void loop ()
{
//	display();
	measure();
	Serial.println(distance);
	dac1.setVoltage(distance*100, false);
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
void display()
{
	volatile unsigned char result = r.process();

	currentMillis = millis();			//get the number of milliseconds since the program started
	//lcd.setCursor(18, 0);					//(col, row)
	//lcd.print((currentMillis - startMillis) / 1000);
	if (currentMillis - startMillis >= period)	//test whether the period has elapsed
	{
		//LCDfadeOut();			 //set LCD to sleep...
	Serial.print(Summary[0]);
	Serial.print(",");
	Serial.print(Summary[1]);
	Serial.print(",");
	Serial.print(Summary[2]);
	Serial.print(",");
	Serial.print(Summary[3]);
	Serial.print(",");
	Serial.print(Summary[4]);
	Serial.print(",");
	Serial.println(Summary[5]);
	Serial.println(distance);
	startMillis = millis();
	} //End if currenMillis...

	if (result) {
		init_backlight();  //wake up LCD...
		if (result == DIR_CCW) {
			if (valueHandler > 0 ) {			//only used to increase or decrease value
				value--;
				lcd.setCursor(0, 3);
				lcd.print(value);
			} else {
				move_up();
				chooseMenu();
			}
		} else {
			if (valueHandler > 0) {		 //only used to increase or decrease value
				value++;
				lcd.setCursor(0, 3);
				lcd.print(value);
			} else {
				move_down();
				chooseMenu();
			}
		}//end else
	}//end if Result

	if (r.buttonPressedReleased(25)) {
		init_backlight();  //wake up LCD...
		if (menuOption == MainMenu) {
			selectionMainMenu();
		}
		else if (menuOption == SubMenu7)
		{
			returnToMainMenu();
		} else {
			selectionSubMenu();
		}//end if menuSelected

		if (valueHandler == 0) {
			chooseMenu();
		}//end if valueHandler
	}//endif buttonPressedReleased
}
void welcome()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("		Ultralydsensor");
	lcd.setCursor(0, 2);
	lcd.print("		Gand VGS	");
	lcd.setCursor(0, 3);
	lcd.print("		Version 1.0  ");
	delay(1000);
}//end welcome

void display_menu(const char *menuInput, int ROWS, int COLS)
{
	int n = 4;		 //4 rows
	lcd.clear();
	lcd.setCursor(0, 0);
//	lcd.print("		Menu	");

	if (ROWS < n - 1) {
		n = ROWS + 1;
	}

	for (int i = 0; i < n ; i++) {
		lcd.setCursor(1, i); //(col, row)
		for (int j = 0; j < COLS; j++) {
			if (*(menuInput + ((displayFirstLine + i - 1) * COLS + j)) != '\0') {
				lcd.print(*(menuInput + ((displayFirstLine + i - 1) * COLS + j)));
			}//end if
		}//end for j
	}//end for i

	lcd.setCursor(0, (cursorLine - displayFirstLine) );
	lcd.print(">");
}//end display_menu

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

void chooseMenu()
{
	// Check in which menu we are
	if (menuOption == SubMenu7)
	{
		display_summary();
	} else {
		display_menu(menuSelected, menuItems, maxItemSize);
	}
}//end chooseMenu

void selectionMainMenu()
{
	//valueHandler activates volume to decrease/increase amount
	if (valueHandler > 0 ) {
		Summary[valueHandler-1] = value;		 //Sets the volume...
		valueHandler = 0;
		value = 0;
		returnToMainMenu();
	} else {

		lcd.clear();

		switch (cursorLine - 1)
		{
			case 0:
				displayFirstLine = 1;				//initialize display_menu to 1st line
				cursorLine = 1;							//initialize display_menu to 1st line
				lcd.setCursor(0, 0);
				lcd.print("Adjust bottom");
				lcd.setCursor(0, 1);
				lcd.print("level and click");
				valueHandler = 1;
				value = Summary[0];
				lcd.setCursor(0, 3);
				lcd.print(value);
				break;
			case 1:
				displayFirstLine = 1;				//initialize display_menu to 1st line
				cursorLine = 1;							//initialize display_menu to 1st line
				lcd.setCursor(0, 0);
				lcd.print("Adjust top");
				lcd.setCursor(0, 1);
				lcd.print("level and click");
				valueHandler = 2;
				value = Summary[valueHandler-1];
				lcd.setCursor(0, 3);
				lcd.print(value);
				break;
			case 2:
				displayFirstLine = 1;				//initialize display_menu to 1st line
				cursorLine = 1;							//initialize display_menu to 1st line
				lcd.setCursor(0, 0);
				lcd.print("Adjust voltage");
				lcd.setCursor(0, 1);
				lcd.print("level and click");
				valueHandler = 3;
				value = Summary[valueHandler-1];
				lcd.setCursor(0, 3);
				lcd.print(value);
				break;
			case 3:
				displayFirstLine = 1;				//initialize display_menu to 1st line
				cursorLine = 1;							//initialize display_menu to 1st line
				menuItems = sizeof Four / sizeof * Four;
				menuSelected = &Four[0][0];
				menuOption = SubMenu4;
				break;
			case 4:
				displayFirstLine = 1;				//initialize display_menu to 1st line
				cursorLine = 1;							//initialize display_menu to 1st line
				lcd.setCursor(0, 0);
				lcd.print("Please set the ");
				lcd.setCursor(0, 1);
				lcd.print("Volume and click");
				valueHandler = 5;
				value = Summary[4];
				lcd.setCursor(0, 3);
				lcd.print(value);
				break;
			case 5:
				displayFirstLine = 1;				//initialize display_menu to 1st line
				cursorLine = 1;							//initialize display_menu to 1st line
				menuItems = sizeof Six / sizeof * Six;
				menuSelected = &Six[0][0];
				menuOption = SubMenu6;
				break;
			case 6:
				displayFirstLine = 1;				//initialize display_menu to 1st line
				cursorLine = 1;							//initialize display_menu to 1st line
				Reset();
				returnToMainMenu();
				break;
			case 7:
				displayFirstLine = 1;				//initialize display_menu to 1st line
				cursorLine = 1;							//initialize display_menu to 1st line
				menuItems = sizeof startMenu / sizeof * startMenu;
				menuOption = SubMenu7;
				break;
		}//end switch
	}//end else
}//end selectionMainMenu

void selectionSubMenu()
{
	lcd.clear();
	switch (cursorLine - 1)
	{
		case 0:
			lcd.setCursor(0, 0);
			lcd.print("option 1");
			Summary[menuOption - 1] = 1;	//user input saved in Summary array..to be used later
			break;
		case 1:
			lcd.setCursor(0, 0);
			lcd.print("option 2");
			Summary[menuOption - 1] = 2;	//user input saved in Summary array..to be used later
			break;
		case 2:
			lcd.setCursor(0, 0);
			lcd.print("option 3");
			if (menuOption != SubMenu4 && menuOption != SubMenu6)		//this is where return option is in the refered SubMenu
				Summary[menuOption - 1] = 3;	//user input saved in Summary array..to be used later
			break;
		case 3:
			lcd.setCursor(0, 0);
			lcd.print("option 4");
			if (menuOption != SubMenu2)			//this is where return option is in the refered SubMenu
				Summary[menuOption - 1] = 4;	//user input saved in Summary array..to be used later
			break;
		case 4:
			lcd.setCursor(0, 0);
			lcd.print("option 5");
			if (menuOption != SubMenu3)			//this is where return option is in the refered SubMenu
				Summary[menuOption - 1] = 5;	//user input saved in Summary array..to be used later
			break;
		case 5:
			lcd.setCursor(0, 0);
			lcd.print("option 6");
			if (menuOption != SubMenu1)			//this is where return option is in the refered SubMenu
				Summary[menuOption - 1] = 6;	//user input saved in Summary array..to be used later
			break;
	}//end switch
	delay(800);
	returnToMainMenu();
}//end selectionSubMenu

void Reset()
{
	for (int i = 0; i < sizeof(startMenu) / maxItemSize + 1; i++)
	{
		Summary[i] = 0;
	}
}//end Reset

void display_summary()
{
	int n = 4;			// 4 rows in LCD
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Menu - distance=");lcd.print(distance);

	if (menuItems < n - 1) {
		n = menuItems + 1;
	}

	for (int i = 1; i < n; i++)
	{
		lcd.setCursor(1, i);	 //(col, row)
		lcd.print(startMenu[displayFirstLine + i - 2]);
		lcd.setCursor(15, i);
		lcd.print(Summary[displayFirstLine + i - 2]);
	}
	lcd.setCursor(0, (cursorLine - displayFirstLine) + 1);
	lcd.print("<");
}//end display_summary

void returnToMainMenu()
{
	displayFirstLine = 1;
	cursorLine = 1;
	menuItems = sizeof startMenu / sizeof * startMenu;
	menuSelected = &startMenu[0][0];
	menuOption = MainMenu;
}//end returnToMainMenu

void LCDfadeOut()
{
	while (brightness > 0) {
		analogWrite(backlightPin, brightness);
		brightness -= fadeAmount;
		delay(20);
	}//end while
	digitalWrite(backlightPin, LOW);
	lcd.clear();
}//end LCDfadeOut

void init_backlight()
{
	digitalWrite(backlightPin, HIGH);
	startMillis = millis();  //initial start time
	brightness = 255;		//reset to initial brightness
}//end init_backlight
