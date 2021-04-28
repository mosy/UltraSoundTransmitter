// library for I2C
#include <Wire.h>
// librarys for the display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> //display
// library for the DAC
#include <Adafruit_MCP4725.h> // DAC
// librarys for the encoder funcion
#include <ClickEncoder.h>
#include <TimerOne.h>

// define the pins used for the HC-SR04.
const int trigger = 11;
const int echo = 10;


// Declaration for an SSD1306 display connected to I2C (SDA,SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

Adafruit_MCP4725 dac1;
#define DAC_RESOLUTION    (12) 

ClickEncoder *encoder;
int16_t last, value;
#define stepsPerNotch 4

float offset = 0;
boolean up = false;
boolean down = false;
boolean middle = false;





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
	// setup of encoder just copy this will not be explaind	
	encoder = new ClickEncoder(2,3,4);
	encoder->setAccelerationEnabled(true);
	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr); 
	last = encoder->getValue();
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
	display.print("Level = ");
	display.print((70+float(offset))-distance);
	display.print(" cm");
	display.setCursor(0,10);
	display.print("Avstand til bunn      = ");
	display.print(offset+70);
	display.print(" cm");
	// actually display all of the above
	display.display();
	
	int dacValue = int(distance*40.95);
  	dac1.setVoltage(dacValue, false);

	readRotaryEncoder();


	if (down) 
	{
		offset=offset-0.25;
		down = false;
	}
	else if (up) 
	{
		offset=offset+0.25;
		up = false;
	}

		
}

void timerIsr()
{ 
	encoder->service();
}




void readRotaryEncoder()
{
	value += encoder->getValue();
	
	if (value/2 > last) 
	{
		last = value/2;
		up = true;
		//Denne verdien kan justeres om en ønsker raskere respons
		delay(150);
	}else		if (value/2 < last) 
	{
		last = value/2;
		down = true;
		//Denne verdien kan justeres om en ønsker raskere respons
		delay(150);
	}

}
