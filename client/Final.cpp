//------------------------------------------------------
// Names: Amro Amanuddein and Pushkar Sabharwal
// ID's: 1572498 (Amro) 1588927 (Pushkar)
// CMPUT275, Winter 2020
//
// Final Project: The Integrapher
//------------------------------------------------------

/*
    Graphics library test for MCU Friend 3.5" TFT LCD shield.

    Tests out various functions from the graphics library.
*/
#include <Arduino.h>
// core graphics library for displays (written by Adafruit)
#include <Adafruit_GFX.h>
// necessary for using a GFX font
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSerifBoldItalic9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
// LCD and SD card will communicate using the Serial Peripheral Interface (SPI)
// e.g., SPI is used to display images stored on the SD card
#include <SPI.h>
// needed for reading/writing to SD card
#include <SD.h>
#include <TouchScreen.h>
// Graphics library for MCU Friend 3.5" TFT LCD shield
#include <MCUFRIEND_kbv.h>
// make an object from MCUFRIEND_kbv class
MCUFRIEND_kbv tft;
//MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#include "lcd_image.h"
#define SD_CS 10
// touch screen pins
#define YP A3
#define XM A2
#define YM 9
#define XP 8
// calibration data for the touch screen, minimum/maximum readings from the touch point
#define TS_MINX 100
#define TS_MINY 110
#define TS_MAXX 960
#define TS_MAXY 910
#define MINPRESSURE 10
#define MAXPRESSURE 1000
// Assign human-readable names to some common 16-bit color values (UTFT color code)
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0xC618
TouchScreen ts = TouchScreen(XP,YP,XM,YM,300);

lcd_image_t Integrapher_logo = {"integrapher.lcd",362,66};
lcd_image_t Symbol = {"symbol.lcd",67,160};

// String of the function, the lower limit and the upper limit to be sent
String Functionstring="";
String sLowerlimit = "";
String sUpperlimit = "" ; 

double Lowerlimit =0;
double Upperlimit =0;

// Initialise all the finite states we have using enumeration
enum {Main,Function,Limit,Graph} current;
// we start at the main menu
int current_state=Main;

// Variables for communication
const uint16_t buf_size = 256;
uint16_t buf_len = 0;
char* buffer = (char *) malloc(buf_size);
//double area1;
///////////////////////////////////////////////// END OF GLOBAL VARIABLE DECLARATIONS ///////////////////////////////////////////////////////

void setup() {
    init();
    Serial.begin(9600);

    //    tft.reset();             // hardware reset
    uint16_t ID = tft.readID();    // read ID from display
    Serial.print("ID = 0x");
    Serial.println(ID, HEX);
    if (ID == 0xD3D3) ID = 0x9481; // write-only shield
    //    ID = 0x9329;             // force ID

    tft.begin(ID);                 // LCD gets ready to work

    // For communication
    Serial.println("PHASE01");
    Serial.println("Client side is waiting for input.");
    Serial.println("PHASE02");

    // set up buffer as empty string
    buf_len = 0;
    buffer[buf_len] = 0;
}

/* * * * * * * * * * *  * * * * * * * * * * * * * * * * *
mainMenu function takes no parameters.

It also does not return any parameters


* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
void mainMenu(){
    tft.setRotation(1);
    tft.fillScreen(WHITE);
    tft.fillRect(110,150,260,80,BLACK);
    tft.fillRect(120,160,240,60,GREEN);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setTextSize(2);
    tft.setTextColor(BLACK);
    tft.setCursor(180,198);
    tft.print("START!");
    tft.setCursor(60,50);
    tft.print("THE INTEGRAPHER");
    lcd_image_draw(&Integrapher_logo, &tft,
         // upper left corner in the image to draw
         0,0,
         // upper left corner of the screen to draw it in
         480,320,
         // width and height of the patch of the image to draw
         362, 132);

}

/* * * * * * * * * * *  * * * * * * * * * * * * * * * * *
graphDisplay function takes no parameters.

It also does not return any parameters


* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 

void process_line() {
  // print what's in the buffer back to server
  Serial.print("Got: ");
  Serial.println(buffer);

  if ((buffer[0] == 'A') && (buffer[1] == 'r')){
    tft.setTextColor(WHITE);
    tft.setFont(&FreeSerifBoldItalic9pt7b);
    tft.setTextSize(1);
    tft.setCursor(210,20);
    tft.println(buffer);
  }  

  if ((buffer[0] == "P")) {
    tft.setTextColor(WHITE);
    tft.setFont(&FreeSerifBoldItalic9pt7b);
    tft.setTextSize(1);
    tft.setCursor(50,50);
    tft.println(buffer);
  }
  // clear the buffer
  buf_len = 0;
  buffer[buf_len] = 0;  
}




void graphDisplay(){
    tft.fillScreen(WHITE);
    tft.drawLine(350,0,350,320,BLACK);
    tft.fillRect(0,0,345,320,BLACK);
    tft.fillRect(358,88,114,64,BLACK);
    tft.fillRect(360,90,110,60,RED);
    tft.setTextColor(WHITE);
    tft.setFont(&FreeSerifBoldItalic9pt7b);
    tft.setTextSize(1);
    tft.setCursor(5,20);
    tft.print("f(x) = " + Functionstring);
    tft.setCursor(370,130);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setTextSize(2);
    tft.setTextColor(BLACK);
    tft.print("QUIT");
    tft.drawLine(170,0,170,320, WHITE);
    tft.drawLine(0,160,350, 160, WHITE);
    tft.setCursor(325, 175);
    tft.setTextColor(RED);
    tft.setTextSize(1);
    tft.print(sUpperlimit);
    tft.setCursor(2, 175);
    tft.print(sLowerlimit);
    tft.setFont(&FreeSansBold9pt7b);
    TSPoint touch = ts.getPoint();
    pinMode(YP,OUTPUT);
    pinMode(XM,OUTPUT);

    while (true){
        char in_line;
        if (Serial.available()) {
          // read the incoming byte:
          char in_char = Serial.read();

          if (in_char == '\n' || in_char == '\r') {
            process_line();
          }  

          else {
              // add character to buffer, provided that we don't overflow.
              // drop any excess characters.
              if ( buf_len < buf_size-1 ) {
                  buffer[buf_len] = in_char;
                  buf_len++;
                  buffer[buf_len] = 0;
              }
            }
        }
              
        if(touch.z > MINPRESSURE && touch.z < MAXPRESSURE){
            int16_t point_x = map(touch.y, TS_MINX, TS_MAXX,0,tft.width());
            int16_t point_y = map(touch.x, TS_MAXY, TS_MINY,0,tft.height());
            // If you press the QUIT button you go to the title screen
            if (point_x > 10 && point_x <120 && point_y >90 && point_y < 150){
                mainMenu();                
            }
        }       
    }
}

/* * * * * * * * * * *  * * * * * * * * * * * * * * * * *
limitEntry function takes no parameters.

It also does not return any parameters


* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
void limitEntry(){
    //SerialPort Serial("/dev/ttyACM0");
    int ULimitCursorX = 45;
    int ULimitCursorY = 20;
    int LLimitCursorX = 25;
    int LLimitCursorY = 110;
    // Clean up the string global variables.
    String lstring = "";
    String ustring = "";
    sLowerlimit = lstring;
    sUpperlimit = ustring;
    tft.fillScreen(BLACK);
    tft.fillRect(5,5,470,110,WHITE);
    tft.drawFastHLine(5,120,470,WHITE);
    tft.drawFastHLine(5,180,470,WHITE);
    tft.drawFastHLine(5,250,470,WHITE);
    tft.drawFastHLine(5,318,470,WHITE);

    tft.drawFastVLine(5,120,240,WHITE);
    tft.drawFastVLine(96,120,240,WHITE);
    tft.drawFastVLine(192,120,240,WHITE);
    tft.drawFastVLine(288,120,240,WHITE);
    tft.drawFastVLine(384,120,240,WHITE);
    tft.drawFastVLine(475,120,240,WHITE);

    tft.drawLine(40,10,20,110,BLACK);

    // COLUMN 1
    tft.fillRect(10,125,80,50,WHITE);
    tft.setCursor(40,160);
    tft.print("0");
    tft.fillRect(10,185,80,60,WHITE);
    tft.setCursor(40,230);
    tft.print("4");
    tft.fillRect(10,255,80,60,WHITE);
    tft.setCursor(40,300);
    tft.print("8");
    // COLUMN 2
    tft.fillRect(105,125,80,50,WHITE);
    tft.setCursor(135,160);
    tft.print("1");
    tft.fillRect(105,185,80,60,WHITE);
    tft.setCursor(135,230);
    tft.print("5");
    tft.fillRect(105,255,80,60,WHITE);
    tft.setCursor(135,300);
    tft.print("9");
    // COLUMN 3
    tft.fillRect(200,125,80,50,WHITE);
    tft.setCursor(230,160);
    tft.print("2");
    tft.fillRect(200,185,80,60,WHITE);
    tft.setCursor(230,230);
    tft.print("6");
    tft.fillRect(200,255,80,60,WHITE);
    tft.setCursor(230,300);
    tft.print(".");
    // COLUMN 4
    tft.fillRect(295,125,80,50,WHITE);
    tft.setCursor(325,160);
    tft.print("3");
    tft.fillRect(295,185,80,60,WHITE);
    tft.setCursor(325,230);
    tft.print("7");
    tft.fillRect(295,255,80,60,WHITE);
    tft.setCursor(325,300);
    tft.print("pi");
    // COLUMN 5
    tft.fillRect(390,125,80,50,RED);
    tft.setCursor(400,160);
    tft.print("DEL");
    tft.fillRect(390,185,80,60,BLUE);
    tft.setCursor(400,230);
    tft.print("UP");
    tft.fillRect(390,255,80,60,GREEN);
    tft.setCursor(400,300);
    tft.print("GO!");

    tft.setCursor(45,70);
    tft.setFont(&FreeSerifBoldItalic9pt7b);
    tft.print(Functionstring + " dx");
    int go2_flag = 0;
    int next_flag = 0;
    tft.setTextSize(1);
    tft.setCursor(350,110);
    tft.print("Enter lower limit");
    // Inner while loop runs until you enter the lower limit and press "UP" 
    // Outer while loop runs until you enter the lower limit and press "GO"
    while(go2_flag != 1){
        tft.setCursor(ULimitCursorX,ULimitCursorY);
        TSPoint touch = ts.getPoint();
        pinMode(YP,OUTPUT);
        pinMode(XM,OUTPUT);
        if(touch.z > MINPRESSURE && touch.z < MAXPRESSURE){
            int16_t point_x = map(touch.y, TS_MINX, TS_MAXX,0,tft.width());
            int16_t point_y = map(touch.x, TS_MAXY, TS_MINY,0,tft.height());
            if (point_x > 10 && point_x < 90 && point_y > 255 && point_y < 315){
                go2_flag = 1;
                Serial.print("Equation: ");
                Serial.println(Functionstring);
            }
            else if (point_x > 390 && point_x < 470 && point_y > 125 && point_y <175){
                    tft.print("0");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("0");

            }
            else if (point_x > 390 && point_x < 470 && point_y > 185 && point_y <245){
                    tft.print("4");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("4");
            }
            else if (point_x > 390 && point_x < 470 && point_y > 255 && point_y < 315){
                    tft.print("8");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("8");
            }
            else if (point_x > 295 && point_x < 375 && point_y > 125 && point_y <175){
                    tft.print("1");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("1");
            }
            else if (point_x > 295 && point_x < 375 && point_y > 185 && point_y < 245){
                    tft.print("5");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("5");
            }
            else if (point_x > 295 && point_x < 375 && point_y > 255 && point_y < 315){
                    tft.print("9");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("9");
            }
            else if (point_x > 225 && point_x < 280 && point_y > 125 && point_y <175){
                    tft.print("2");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("2");
            }
            else if (point_x > 225 && point_x < 280 && point_y > 185 && point_y <245){
                    tft.print("6");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("6");
            }
            else if (point_x > 225 && point_x < 280 && point_y > 255 && point_y <315){
                    tft.print(".");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat(".");
            }
            else if (point_x > 105 && point_x < 185 && point_y > 125 && point_y <175){
                    tft.print("3");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("3");
            }
            else if (point_x > 105 && point_x < 185 && point_y > 185 && point_y <245){
                    tft.print("7");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("7");
            }
            else if (point_x > 105 && point_x < 185 && point_y > 255 && point_y <315){
                    tft.print("pi");
                    delay(1000);
                    ULimitCursorX += 18;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
                    ustring.concat("pi");
            }
            // Delete button 
            else if (point_x > 390 && point_x < 470 && point_y > 125 && point_y <175){
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(ULimitCursorX,ULimitCursorY);
            }

        }
        while(next_flag != 1){
            // lower limit entry 
            tft.setCursor(LLimitCursorX,LLimitCursorY);
            TSPoint touch = ts.getPoint();
            pinMode(YP,OUTPUT);
            pinMode(XM,OUTPUT);
            if(touch.z > MINPRESSURE && touch.z < MAXPRESSURE){
                int16_t point_x = map(touch.y, TS_MINX, TS_MAXX,0,tft.width());
                int16_t point_y = map(touch.x, TS_MAXY, TS_MINY,0,tft.height());
                // next button pressed, we leave the while loop to go to the upper limit
                if (point_x > 10 && point_x < 90 && point_y > 185 && point_y < 245){
                    next_flag = 1;
                }
                else if (point_x > 390 && point_x < 470 && point_y > 125 && point_y <175){
                    tft.print("0");
                    delay(1000);
                    LLimitCursorX+=9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("0");
                }
                else if (point_x > 390 && point_x < 470 && point_y > 185 && point_y <245){
                    tft.print("4");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("4");
                }
                else if (point_x > 390 && point_x < 470 && point_y > 255 && point_y < 315){
                    tft.print("8");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("8");
                }
                else if (point_x > 295 && point_x < 375 && point_y > 125 && point_y <175){
                    tft.print("1");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("1");
                }
                else if (point_x > 295 && point_x < 375 && point_y > 185 && point_y < 245){
                    tft.print("5");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("5");
                }
                else if (point_x > 295 && point_x < 375 && point_y > 255 && point_y < 315){
                    tft.print("9");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("9");
                }
                else if (point_x > 225 && point_x < 280 && point_y > 125 && point_y <175){
                    tft.print("2");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("2");
                }
                else if (point_x > 225 && point_x < 280 && point_y > 185 && point_y <245){
                    tft.print("6");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("6");
                }
                else if (point_x > 225 && point_x < 280 && point_y > 255 && point_y <315){
                    tft.print(".");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat(".");
                }
                else if (point_x > 105 && point_x < 185 && point_y > 125 && point_y <175){
                    tft.print("3");
                    delay(1000);
                    LLimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("3");
                }
                else if (point_x > 105 && point_x < 185 && point_y > 185 && point_y <245){
                    tft.print("7");
                    delay(1000);
                    ULimitCursorX += 9;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("7");
                }
                else if (point_x > 105 && point_x < 185 && point_y > 255 && point_y <315){
                    tft.print("pi");
                    delay(1000);
                    LLimitCursorX += 18;
                    tft.setCursor(LLimitCursorX,LLimitCursorY);
                    lstring.concat("pi");
                }
                // delete button
                else if (point_x > 390 && point_x < 470 && point_y > 125 && point_y <175){
                        delay(1000);
                        LLimitCursorX += 9;
                        tft.setCursor(LLimitCursorX,LLimitCursorY);
                }
            } 
        }
        tft.setCursor(350,20);
        tft.print("Enter upper limit");
    }
    sLowerlimit = lstring;
    sUpperlimit = ustring;
    Serial.print("UpperL: ");
    Serial.println(sUpperlimit);
    Serial.print("LowerL: ");
    Serial.println(sLowerlimit);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setTextSize(2);
    // Once the GO button is pressed, we go to display the graph
    graphDisplay();
    current_state = Graph;

}


/* * * * * * * * * * *  * * * * * * * * * * * * * * * * *
functionEntry function takes no parameters.

It also does not return any parameters


* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
void functionEntry(){
    char currentChar = 'A';
    String innerstring = "";
    // Clean up the global variable 
    Functionstring = innerstring;
    int cursorX = 100;
    int cursorY = 35;
    tft.fillScreen(BLACK);
    tft.setTextColor(BLACK);
    tft.setFont(&FreeSansBold9pt7b);
    tft.fillRect(18,246,104,64,BLACK);
    // Draw vertical grid lines
    tft.drawLine(2,20,2,320,WHITE);
    tft.drawLine(93,20,93,320,WHITE);
    tft.drawLine(190,20,190,320,WHITE);
    tft.drawLine(285,20,285,320,WHITE);
    tft.drawLine(380,20,380,320,WHITE);
    tft.drawLine(475,20,475,320,WHITE);
    // Draw horizontal grid lines
    tft.drawLine(0,115,480,115,WHITE);
    tft.drawLine(0,175,480,175,WHITE);
    tft.drawLine(0,240,480,240,WHITE);
    tft.drawLine(105,280,480,280,WHITE);

    tft.fillRect(5,245,85,70,GREEN);  
    tft.fillRect(0,5,480,50,WHITE);
    tft.setCursor(10,290);
    tft.print("GO!");
    tft.setCursor(10,35);
    tft.print("f(x) = ");
    // COLUMN 1
    tft.fillRect(5,60,85,50,WHITE);
    tft.fillRect(5,120,85,50,WHITE);
    tft.fillRect(5,180,85,55,WHITE);
    // COLUMN 2
    tft.fillRect(100,60,85,50,WHITE);
    tft.fillRect(100,120,85,50,WHITE);
    tft.fillRect(100,180,85,55,WHITE);
    tft.fillRect(100,245,85,32,WHITE);
    tft.fillRect(100,285,85,32,WHITE);
    // COLUMN 3
    tft.fillRect(195,60,85,50,WHITE);
    tft.fillRect(195,120,85,50,WHITE);
    tft.fillRect(195,180,85,55,WHITE);
    tft.fillRect(195,245,85,32,WHITE);
    tft.fillRect(195,285,85,32,WHITE);
    // COLUMN 4
    tft.fillRect(290,60,85,50,WHITE);
    tft.fillRect(290,120,85,50,WHITE);
    tft.fillRect(290,180,85,55,WHITE);
    tft.fillRect(290,245,85,32,WHITE);
    tft.fillRect(290,285,85,32,WHITE);
    // COLUMN 5
    tft.fillRect(385,60,85,50,WHITE);
    tft.fillRect(385,120,85,50,WHITE);
    tft.fillRect(385,180,85,55,WHITE);
    tft.fillRect(385,245,85,32,WHITE);
    tft.fillRect(385,285,85,32,RED);
    // Printing all the characters on the buttons
    tft.setCursor(40,95);   
    tft.print("0");
    tft.setCursor(40,150);
    tft.print("3");
    tft.setCursor(40,215);
    tft.print("6");
    tft.setCursor(135,95);
    tft.print("1");
    tft.setCursor(135,150);
    tft.print("4");
    tft.setCursor(135,215);
    tft.print("7");
    tft.setCursor(135,270);
    tft.print("(");
    tft.setCursor(140,310);
    tft.print("/");
    tft.setCursor(230,95);
    tft.print("2");
    tft.setCursor(230,150);
    tft.print("5");
    tft.setCursor(230,215);
    tft.print("8");
    tft.setCursor(230,305);
    tft.print("-");
    tft.setCursor(320,150);
    tft.print("^");
    tft.setCursor(320,215);
    tft.print("9");
    tft.setCursor(320,270);
    tft.print(")");
    tft.setCursor(320,313);
    tft.print("*");
    tft.setFont(&FreeSans9pt7b);
    tft.setCursor(230,270);
    tft.print("+");
    tft.setCursor(400,95);
    tft.print("sin");
    tft.setCursor(400,150);
    tft.print("cos");
    tft.setCursor(400,215);
    tft.print("tan");
    tft.setCursor(400,270);
    tft.print("exp");
    tft.setCursor(400,315);
    tft.print("DEL");
    tft.setCursor(320,95);
    tft.setFont(&FreeSerifBoldItalic9pt7b);
    tft.print("x");

    // 0 if GO is not pressed, 1 if pressed
    int go_flag=0;

    tft.setCursor(cursorX,cursorY);

    tft.setFont(&FreeSansBold9pt7b);
    // while the GO button is not pressed...we keep taking input and shifting the cursor
    while (go_flag != 1){
        int lengths = innerstring.length();
        tft.setFont(&FreeSansBold9pt7b);
        tft.setTextColor(BLACK);
        TSPoint touch = ts.getPoint();
        pinMode(YP,OUTPUT);
        pinMode(XM,OUTPUT);
        if(touch.z > MINPRESSURE && touch.z < MAXPRESSURE){
            int16_t point_x = map(touch.y, TS_MINX, TS_MAXX,0,tft.width());
            int16_t point_y = map(touch.x, TS_MAXY, TS_MINY,0,tft.height());
            // x-coords are flipped :(
            if(point_x > 375 && point_x < 475 && point_y > 245 && point_y < 315){
                go_flag = 1;
            }
            else if(point_x > 390 && point_x < 475 && point_y > 60 && point_y < 110){
                tft.print("0");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("0");
                currentChar = '0';
            }
            else if(point_x > 390 && point_x < 475 && point_y > 120 && point_y < 170){
                tft.print("3");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("3");
                currentChar = '3';
            }
            else if(point_x > 390 && point_x < 475 && point_y > 180 && point_y < 235){
                tft.print("6");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("6");
                currentChar = '6';
            }
            else if(point_x > 295 && point_x < 390 && point_y > 60 && point_y < 110){
                tft.print("1");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("1");
                currentChar = '1';
            }
            else if(point_x > 295 && point_x < 390 && point_y > 120 && point_y < 170){
                tft.print("4");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("4");
                currentChar = '4';
            }
            else if(point_x > 295 && point_x < 390 && point_y > 180 && point_y < 235){
                tft.print("7");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("7");
                currentChar = '7';
            }
            else if(point_x > 295 && point_x < 390 && point_y > 245 && point_y < 277){
                tft.print("(");
                delay(1000);
                cursorX+=17;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("(");
                currentChar = '(';
            }
            else if(point_x > 295 && point_x < 390 && point_y > 285 && point_y < 317){
                tft.print("/");
                delay(1000);
                cursorX+=17;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("/");
                currentChar = '/';
            }
            else if(point_x > 200 && point_x < 285 && point_y > 60 && point_y < 110){
                tft.print("2");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("2");
                currentChar = '2';
            }
            else if(point_x > 200 && point_x < 285 && point_y > 120 && point_y < 170){
                tft.print("5");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("5");
                currentChar = '5';
            }
            else if(point_x > 200 && point_x < 285 && point_y > 180 && point_y < 235){
                tft.print("8");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                innerstring.concat("8");
                currentChar = '8';
            }
             else if(point_x > 200 && point_x < 285 && point_y > 245 && point_y < 277){

                tft.print("+");
                delay(1000);
                cursorX+=18;
                tft.setCursor(cursorX,cursorY);
                currentChar = '+';
                innerstring.concat("+");
            }
             else if(point_x > 200 && point_x < 285 && point_y > 285 && point_y < 317){

                tft.print("-");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                currentChar = '-';
                innerstring.concat("-");
            }
            else if(point_x > 105 && point_x < 190 && point_y > 60 && point_y < 110){
                tft.setFont(&FreeSerifBoldItalic9pt7b);
                tft.print("x");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                currentChar = 'x';
                innerstring.concat("x");
            }
            else if(point_x > 105 && point_x < 190 && point_y > 120 && point_y < 170){
                tft.print("^");
                delay(1000);
                cursorX+=17;
                tft.setCursor(cursorX,cursorY);
                currentChar = '^';
                innerstring.concat("^");
            }
            else if(point_x > 105 && point_x < 190 && point_y > 180 && point_y < 235){
                tft.print("9");
                delay(1000);
                cursorX+=20;
                tft.setCursor(cursorX,cursorY);
                currentChar = '9';
                innerstring.concat("9");
            }
            else if(point_x > 105 && point_x < 190 && point_y > 245 && point_y < 277){
                tft.print(")");
                delay(1000);
                cursorX+=17;
                tft.setCursor(cursorX,cursorY);
                currentChar = ')';
                innerstring.concat(")");
            }
            else if(point_x > 105 && point_x < 190 && point_y > 285 && point_y < 317){
                tft.print("*");
                delay(1000);
                cursorX+=17;
                tft.setCursor(cursorX,cursorY);
                currentChar = '*';
                innerstring.concat("*");
            }
            else if(point_x > 10 && point_x < 95 && point_y > 60 && point_y < 110){
                tft.setFont(&FreeSans9pt7b);
                tft.print("sin");
                delay(1000);
                cursorX+=51;
                tft.setCursor(cursorX,cursorY);
                currentChar = 'S';
                innerstring.concat("sin");
            }
            else if(point_x > 10 && point_x < 95 && point_y > 120 && point_y < 170){
                tft.setFont(&FreeSans9pt7b);
                tft.print("cos");
                delay(1000);
                cursorX+=51;
                tft.setCursor(cursorX,cursorY);
                currentChar = 'C';
                innerstring.concat("cos");
            }
            else if(point_x > 10 && point_x < 95 && point_y > 180 && point_y < 235){
                tft.setFont(&FreeSans9pt7b);
                tft.print("tan");
                delay(1000);
                cursorX+=51;
                tft.setCursor(cursorX,cursorY);
                currentChar = 'T';
                innerstring.concat("tan");
            }
            else if(point_x > 10 && point_x < 95 && point_y > 245 && point_y < 277){
                tft.setFont(&FreeSans9pt7b);
                tft.print("exp");
                delay(1000);
                cursorX+=51;
                tft.setCursor(cursorX,cursorY);
                currentChar = 'E';
                innerstring.concat("exp");
            }
            // delete button
            else if(point_x > 10 && point_x < 95 && point_y > 285 && point_y < 317){
                delay(1000);
                // If we have a sin, cos, tan or exp then we delete 3 characters not 1
                if(currentChar=='S' || currentChar=='E' || currentChar=='T' || currentChar=='C') {
                	cursorX-=51;
                	tft.fillRect(cursorX,cursorY-27,59,40,WHITE);
                	tft.setCursor(cursorX,cursorY);
                }
                else {
                	cursorX-=19;
                    // dont work....
                    innerstring[lengths-1] = "\0";
                    Serial.print(innerstring); 
                	tft.fillRect(cursorX,cursorY-27,19,40,WHITE);
                	tft.setCursor(cursorX,cursorY);
                }
            }

        }
    }
    Functionstring = innerstring;
    // Once the go button is pressed we go to enter our limits of integration
    current_state=Limit;
}

/* * * * * * * * * * *  * * * * * * * * * * * * * * * * *
processTouchScreen function takes no parameters.

It also does not return any parameters


* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */ 
void processTouchScreen(int x, int x2, int y, int y2) {

    TSPoint touch = ts.getPoint();

    pinMode(YP,OUTPUT);
    pinMode(XM,OUTPUT);

        if(touch.z > MINPRESSURE && touch.z < MAXPRESSURE){
            int16_t point_x = map(touch.y, TS_MINX, TS_MAXX,0,tft.width());
            int16_t point_y = map(touch.x, TS_MAXY, TS_MINY,0,tft.height());

            // To enter function entry mode

            if (point_x > x && point_x < x2 && point_y > y && point_y < y2){

                if (current_state==Main) {
                    current_state=Function;
                }

                if (current_state==Graph) {
                    current_state=Main;
                }
            }
        }
    

}


int main(){

    setup();
    while (true) {


        if (current_state==Main) {

            mainMenu();

            while (current_state==Main) {
                processTouchScreen(120,360,160,220);
            }
        }

        if (current_state==Function) {
            functionEntry();
        }

        if (current_state == Limit){
            limitEntry();
        }

        if (current_state==Graph) {
            while (current_state==Graph) {
                processTouchScreen(10,120,90,150);
            }
        }
    }


}
