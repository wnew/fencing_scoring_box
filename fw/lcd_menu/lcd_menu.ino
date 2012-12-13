/*********************************************************************
This is an example sketch for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// pin 8 - Serial clock out (SCLK)
// pin 7 - Serial data out (DIN)
// pin 6 - Data/Command select (D/C)
// pin 5 - LCD chip select (CS)
// pin 4 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 7, 6, 5, 4);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

int menu_state = 0;
int select_state = 1;

static unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

void setup()   {
  Serial.begin(9600);
  Serial.println("Start");

  display.begin();
  display.setContrast(40);

  pinMode(2, OUTPUT);
  attachInterrupt(0, down_button, CHANGE);
  pinMode(3, OUTPUT);
  attachInterrupt(1, select_button, CHANGE);

  //display.display(); // show splashscreen
  //delay(2000);
  display.clearDisplay();   // clears the screen and buffer
}

void loop() {
  display.clearDisplay();
  display_menu();
  delay(70);
}

void select_button() {
  Serial.println("select button");
  if (menu_state == 0) {
    if (select_state == 1) {
      menu_state = 1;
      foil_menu(1);
    }
    if (select_state == 2) {
      menu_state = 2;
      epee_menu(1);
    }
    if (select_state == 3) {
      menu_state = 3;
      sabre_menu(1);
    }
  }
  if (menu_state == 1) {
    if (select_state == 1) {
      menu_state = 0;
    }
    if (select_state == 2) {
      menu_state = 0;
    }
    if (select_state == 3) {
      menu_state = 0;
    }
  }
  if (menu_state == 2) {
    if (select_state == 1) {
      menu_state = 0;
    }
    if (select_state == 2) {
      menu_state = 0;
    }
    if (select_state == 3) {
      menu_state = 0;
    }
  }
  if (menu_state == 3) {
    if (select_state == 1) {
      menu_state = 0;
    }
    if (select_state == 2) {
      menu_state = 0;
    }
    if (select_state == 3) {
      menu_state = 0;
    }
  }
  select_state = 1;
}

void down_button() {
  Serial.println("select_state");
  if (select_state == 3)
    select_state = 1;
  else
    select_state++;
}


void display_menu() {
  if (menu_state == 0) {
    main_menu(select_state);
  }
  if (menu_state == 1) {
    foil_menu(select_state);
  }
  if (menu_state == 2) {
    epee_menu(select_state);
  }
  if (menu_state == 3) {
    sabre_menu(select_state);
  }
}

void main_menu(int i) {
  // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.setTextColor(BLACK, WHITE);
  display.println("Select Weapon:");
  if (i == 1)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("1. Foil");
  if (i == 2)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("2. Epee");
  if (i == 3)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("3. Sabre");
  display.display();
}

void foil_menu(int i) {
  // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.setTextColor(BLACK, WHITE);
  display.println("Foil Tests:");
  if (i == 1)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("1.Lockout");
  if (i == 2)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("2.Depress");
  if (i == 3)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("3.All Senarios");
  display.display();
}

void epee_menu(int i) {
  // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.setTextColor(BLACK, WHITE);
  display.println("Foil Tests:");
  if (i == 1)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("1.Lockout");
  if (i == 2)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("2.Depress");
  if (i == 3)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("3.All Senarios");
  display.display();
}  
  
void sabre_menu(int i) {
  // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.setTextColor(BLACK, WHITE);
  display.println("Foil Tests:");
  if (i == 1)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("1.Lockout");
  if (i == 2)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("2.Depress");
  if (i == 3)
    display.setTextColor(WHITE, BLACK);
  else
    display.setTextColor(BLACK, WHITE);
  display.println("3.All Senarios");
  display.display();
}
