#include <Wire.h>//I2C
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> //display
#include <Adafruit_MCP4725.h> // DAC

// Declaration for an SSD1306 display connected to I2C (SDA,SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// define the pins used for the HC-SR04.
const int trigger = 11;
const int echo = 10;

Adafruit_MCP4725 dac1;
#define DAC_RESOLUTION    (12) 

void setup()
{
	// set pin functions
	pinMode(trigger,OUTPUT);
	pinMode(echo,INPUT);
	//sette starte displayet og sette inn I2C adressen
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();
	display.display();
	dac1.begin(0x62);
}
void loop()
{
	//first we trun off the trigger pin to be sure we have a clean high pulse
	digitalWrite(trigger, LOW);
	delayMicroseconds(2);
	//turn on the trigger pin for 10us
	digitalWrite(trigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigger, LOW);
	// get time echo pin is on
	long duration = pulseIn(echo, HIGH);
	//recalculate time to distance
	float distance = duration * 0.0343 / 2;
	// first we clear previous display
	display.clearDisplay();
	// The size we want
	display.setTextSize(1);
	// if we du not set som color the display will be black
	display.setTextColor(SSD1306_WHITE);
	// Where shall we put the new text
	display.setCursor(0,0);
	//the next 3 lines is displaying the actual text
	display.print("Avstand = ");
	display.print(distance);
	display.print(" cm");
	// actually display all of the above
	display.display();
	int dacValue = int(distance*40.95);
  	dac1.setVoltage(dacValue, false);
}
