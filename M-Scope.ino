/*
 * This is a sketch is a replacement Ocillio Scope for a Marantz Model 19
 * It uses a 128x128 1.44 inch ST7735 based LCD
 * 
 * Written by August Zuanich IIII.
 */

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include <TFT_eWidget.h>  // Widget library
MeterWidget volts = MeterWidget(&tft);

#define SerialDebugging false

// color definitions
const int  Display_Color_Black        = 0x0000;
const int  Display_Color_Blue         = 0x001F;
const int  Display_Color_Red          = 0xF800;
const int  Display_Color_Green        = 0x07E0;
const int  Display_Color_Cyan         = 0x07FF;
const int  Display_Color_Magenta      = 0xF81F;
const int  Display_Color_Yellow       = 0xFFE0;
const int  Display_Color_White        = 0xFFFF;

const int persistence = 5000;

// The colors we actually want to use
int Display_LineDot_Color      = Display_Color_Green;
int Display_Background_Color   = Display_Color_Black;

int ySensorPin = A0;
int xSensorPin = A1;
int x=0;
int y=0;
int an0=0;
int an1=0;
int xd[persistence];
int yd[persistence];
int i=persistence-1;
long av = 0;

void setup() {

    #if (SerialDebugging)
    Serial.begin(115200); while (!Serial); Serial.println("Serial output setup.");
    #endif

    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(Display_Background_Color);

    #if (SerialDebugging)
    Serial.println("Display is setup.");
    #endif
    digitalWrite(23,HIGH);
}

void loop()
{
    //
    //read D to A and scale for display
    //
    an0 = analogRead(xSensorPin);
    an1 = analogRead(ySensorPin);
    x = (an0-260)/1.8;
    y = (an1-260)/1.8;

    tft.drawPixel(x,y,Display_LineDot_Color);
    xd[i]=x;
    yd[i]=y;
    i--;
    if (i==0) i = persistence-1;
    tft.drawPixel(xd[i],yd[i],Display_Background_Color);
}
