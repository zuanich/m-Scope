/*
 * This is a sketch is a replacement Ocillio Scope for a Marantz Model 19
 * It uses a round 240 x 240 LCD
 * 
 * Written by August Zuanich IIII.
 */

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

// color definitions
const int  Display_Color_Black        = 0x0000;
const int  Display_Color_Blue         = 0x001F;
const int  Display_Color_Red          = 0xF800;
const int  Display_Color_Green        = 0x07E0;
const int  Display_Color_Cyan         = 0x07FF;
const int  Display_Color_Magenta      = 0xF81F;
const int  Display_Color_Yellow       = 0xFFE0;
const int  Display_Color_White        = 0xFFFF;

const int persistence = 1000; // Tested 100 for M-Scope, still need more testing.

const float pi = 3.141592654;
const float piIncrement = 10;

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
int i=1;
int j=0;
int i2=2;
int j2=1;
float piCount = pi;
int minX = 32767;
int minY = 32767;
int maxX = -32768;
int maxY = -32768;


void setup() {

    Serial.begin(115200);

    tft.begin();
    tft.setRotation(2);
    tft.fillScreen(Display_Background_Color);
    Serial.println("Display is setup.");

    digitalWrite(23,HIGH);

    xd[1] = -1;
}

void loop()
{
    an0 = analogRead(xSensorPin);
    an1 = analogRead(ySensorPin);

    //****Scale fit all on display****
    x = an0 / 4.2625;
    y = an1 / 4.2625;
    
    //****Origional Scope Factors****
    //read D to A and scale for display
    //D to A Range 0 to 1023
    //CRT Scale: X: 758-294 Range:464
    //CRT Scale: Y: 715-264 Range:451
    //
//    x = (an0-294) * 0.5172413793103448;
//    y = (an1-264) * 0.5321507760532151;

    //****Used for calibration****
    if (an0 > maxX){
      maxX = an0;
    }

    if (an0 < minX){
      minX = an0;
    }

    if (an1 > maxY){
      maxY = an1;
    }

    if (an1 < minY){
      minY = an1;
    }

    //****used to generate a circle for testing****
//    x = int((sin(piCount)+1) * 80) + 40;
//    y = int((cos(piCount)+1) * 80) + 40;
//    piCount += (pi/piIncrement);
    
    if (xd[1] != -1) tft.drawLine(x,y,xd[j],yd[j],Display_LineDot_Color);
    tft.drawLine(xd[i2],yd[i2],xd[j2],yd[j2],Display_Background_Color);
    xd[i]=x;
    yd[i]=y;

    if (i == persistence-1) i = -1;
    if (j == persistence-1) j = -1;
    if (i2 == persistence-1) i2 = -1;
    if (j2 == persistence-1) j2 = -1;
    i++;
    j++;
    i2++;
    j2++;
}

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
      //
      // Print out max an values collected from calibration routine
      //
      Serial.print("Max X:");
      Serial.println(maxX);

      Serial.print("Min X:");
      Serial.println(minX);

      Serial.print("Max Y:");
      Serial.println(maxY);

      Serial.print("Min Y:");
      Serial.println(minY);
}
