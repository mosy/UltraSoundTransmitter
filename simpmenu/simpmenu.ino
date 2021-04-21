#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

int menuitem = 1;
int frame = 1;
int page = 1;
int lastMenuItem = 1;

String menuItem1 = "Contrast";
String menuItem2 = "Volume";

int contrast=60;
int volume = 50;



boolean up = false;
boolean down = false;
boolean middle = false;

ClickEncoder *encoder;
int16_t last, value;

#define OLED_RESET 4
Adafruit_SSD1306 display(128,32, &Wire,OLED_RESET);
//Adafruit_PCD8544 display = Adafruit_PCD8544( 5, 4, 3); //Download the latest Adafruit Library in order to use this constructor

void setup() {
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	pinMode(7,OUTPUT);
	turnBacklightOn();
	
	encoder = new ClickEncoder(2,3,4);
	encoder->setAccelerationEnabled(false);
	 
	display.begin();			
	display.clearDisplay(); 
	setContrast();	

	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr); 
	
	last = encoder->getValue();
}

void loop() 
{

	drawMenu();

	readRotaryEncoder();

	 ClickEncoder::Button b = encoder->getButton();
	 if (b != ClickEncoder::Open) {
	 switch (b) {
			case ClickEncoder::Clicked:
				 middle=true;
				break;
		}
	}		 
	
	if (up && page == 1 ) {
		 
		up = false;
		if(menuitem==2 && frame ==2)
		{
			frame--;
		}
		lastMenuItem = menuitem;
		menuitem--;
		if (menuitem==0)
		{
			menuitem=1;
		} 
	}else if (up && page == 2 && menuitem==1 ) {
		up = false;
		contrast--;
	}
	else if (up && page == 2 && menuitem==2 ) {
		up = false;
		volume--;
	}

	if (down && page == 1) //We have turned the Rotary Encoder Clockwise
	{
		down = false;
		lastMenuItem = menuitem;
	}else if (down && page == 2 && menuitem==1) {
		down = false;
		contrast++;
	}
	else if (down && page == 2 && menuitem==2) {
		down = false;
		volume++;
	}
	
	if (middle) //Middle Button is Pressed
	{
		middle = false;
	 
		if (page == 1 && menuitem<=4) 
			page=2;
		else if (page == 2) 
			page=1; 
	 }	 
}
	
	void drawMenu()
	{
		
	if (page==1) 
	{		 
		display.setTextSize(1);
		display.clearDisplay();
		display.setTextColor(WHITE,BLACK );
//		display.setCursor(15, 0);
		//display.println("		 MAIN MENU");
		//display.drawFastHLine(0,7,128,WHITE);//BLACK

		if(menuitem==1 && frame ==1)
		{		
			displayMenuItem(menuItem1, 1,true);
			displayMenuItem(menuItem2, 11,false);
		}
		else if(menuitem == 2 && frame == 1)
		{
			displayMenuItem(menuItem1, 1,false);
			displayMenuItem(menuItem2, 11,true);
			displayMenuItem(menuItem3, 21,false);
		}
		display.display();
	}
	else if (page==2 && menuitem == 1) 
	{		 
	 displayIntMenuPage(menuItem1, contrast);
	}

	else if (page==2 && menuitem == 2) 
	{
	 displayIntMenuPage(menuItem2, volume);
	}
	
	}

	void resetDefaults()
	{
		contrast = 60;
		volume = 50;
	}


	void timerIsr() {
	encoder->service();
}

void displayIntMenuPage(String menuItem, int value)
{
		display.setTextSize(1);
		display.clearDisplay();
		display.setTextColor(WHITE,BLACK );
		display.setCursor(15, 0);
		display.println(menuItem);
		display.drawFastHLine(0,10,127,WHITE);//BLACK
		display.setCursor(5, 15);
		display.println("Value");
		display.setTextSize(2);
		display.setCursor(5, 25);
		display.println(value);
		display.setTextSize(1);
		display.display();
}

void displayStringMenuPage(String menuItem, String value)
{
		display.setTextSize(1);
		display.clearDisplay();
		display.setTextColor( WHITE,BLACK);
		display.setCursor(15, 0);
		display.println(menuItem);
		display.drawFastHLine(0,10,128,WHITE);//BLACK
		display.setCursor(5, 15);
		display.println("Value");
		display.setTextSize(2);
		display.setCursor(5, 25);
		display.println(value);
		display.setTextSize(2);
		display.display();
}

void displayMenuItem(String item, int position, boolean selected)
{
		if(selected)
		{
			display.setTextColor(BLACK,WHITE);//, BLACK
		}else
		{
			display.setTextColor(WHITE);//BLACK,
		}
		display.setCursor(0, position);
		display.print(">"+item);
}

void readRotaryEncoder()
{
	value += encoder->getValue();
	
	if (value/2 > last) {
		last = value/2;
		down = true;
		delay(150);
	}else		if (value/2 < last) {
		last = value/2;
		up = true;
		delay(150);
	}
}



