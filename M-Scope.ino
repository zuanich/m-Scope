/*
 ┏━┓┏━┓━━━━━┏━━━┓┏━━━┓┏━━━┓┏━━━┓┏━━━┓
 ┃┃┗┛┃┃━━━━━┃┏━┓┃┃┏━┓┃┃┏━┓┃┃┏━┓┃┃┏━━┛
 ┃┏┓┏┓┃━━━━━┃┗━━┓┃┃━┗┛┃┃━┃┃┃┗━┛┃┃┗━━┓
 ┃┃┃┃┃┃┏━━━┓┗━━┓┃┃┃━┏┓┃┃━┃┃┃┏━━┛┃┏━━┛
 ┃┃┃┃┃┃┗━━━┛┃┗━┛┃┃┗━┛┃┃┗━┛┃┃┃━━━┃┗━━┓
 ┗┛┗┛┗┛━━━━━┗━━━┛┗━━━┛┗━━━┛┗┛━━━┗━━━┛
 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * This is a sketch is a replacement Ocillio Scope for a Marantz Model 19
 * It uses a round 240 x 240 LCD
 * 
 * Written by August Zuanich IIII.

   Notes:
   Brown wire Pin K on Scope circut board is Horizontal
   White wire Pin E on Scope circut board is Vertical

   Voltaged for Max and Min on Scope Display
   Horizontal: 151.4v Max 56.3v Min
   Vertical: 148.7v Max 49.1 Min

   ****Origional Scope Factors****
   read D to A and scale for display
   D to A Range 0 to 1023
   CRT Scale: X: 758-294 Range:464
   CRT Scale: Y: 715-264 Range:451

 */

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#include <SPI.h>
#include <SD.h>

#define BUTTONPIN 20

// color definitions
const int  Display_Color_Black        = 0x0000;
const int  Display_Color_Blue         = 0x001F;
const int  Display_Color_Red          = 0xF800;
const int  Display_Color_Green        = 0x07E0;
const int  Display_Color_Cyan         = 0x07FF;
const int  Display_Color_Magenta      = 0xF81F;
const int  Display_Color_Yellow       = 0xFFE0;
const int  Display_Color_White        = 0xFFFF;

const int persistence = 100; // Tested 100 for M-Scope, still need more testing.

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
int minX = 32767;
int minY = 32767;
int maxX = -32768;
int maxY = -32768;
boolean scaleToFit = false;

String sampFileName = "SAMPLES.BIN";
String recFileName = "SAMPREC.BIN";
File myFile;
byte hDataValue[2];
byte vDataValue[2];
int numSamples = 0;
int sdChipSelect = 15;
boolean sdCardValid = true;

boolean buttonNotChecked = true;

/*
 ___  ___  _____  _   _  ___ 
/ __|| __||_   _|| | | || _ \
\__ \| _|   | |  | |_| ||  _/
|___/|___|  |_|   \___/ |_|    
*/
void setup() {

    Serial.begin(115200);
     while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
    }
    Serial.println("Serial port is setup");

    tft.begin();
    tft.setRotation(2);
    tft.fillScreen(Display_Background_Color);
    Serial.println("Display is setup.");

    digitalWrite(23,HIGH);

    xd[1] = -1;

    Serial.print("Initializing SD card...");
    // see if the card is present and can be initialized:
    if (!SD.begin(sdChipSelect)) {
      Serial.println("card failed, or not present");
      sdCardValid = false;
    }

    if (sdCardValid)
    {
      Serial.println("card initialized.");

      myFile = SD.open(sampFileName, FILE_READ);
      if (myFile)
      {
         numSamples = myFile.size();

         Serial.print("Opened SD card sample file ");
         Serial.println(sampFileName);
         Serial.print("Number of samples in sample file:");
         Serial.print(numSamples / 8);
         Serial.print(" (");
         Serial.print(numSamples);
         Serial.println(" bytes)");
      }
      else
      {
         Serial.print("Failed to open SD card sample file ");
         Serial.println(sampFileName);
         sdCardValid = false;
      }
      recFile = SD.open(sampFileName, FILE_WRITE);

    }

    pinMode(BUTTONPIN, INPUT);
}
/*
 _      ___    ___   ___ 
| |    / _ \  / _ \ | _ \
| |__ | (_) || (_) ||  _/
|____| \___/  \___/ |_|   
*/                  
void loop()
{
    while(!digitalRead(BUTTONPIN))
    {
      if (buttonNotChecked)
      {
        if (scaleToFit)
        {
          scaleToFit = false;
          buttonNotChecked = false;
        }
        else
        {
          scaleToFit = true;
          buttonNotChecked = false;
        }
      }
    }

    buttonNotChecked = true;

    if (sdCardValid)
    {
      myFile.read(hDataValue,2);
      myFile.read(vDataValue,2);
      if (myFile.available() == 0) myFile.seek(0);

      an0 = hDataValue[0] + (hDataValue[1] * 256);
      an1 = vDataValue[0] + (vDataValue[1] * 256);
    }
    else
    {    
      an0 = analogRead(xSensorPin);
      an1 = analogRead(ySensorPin);
    }

    if (scaleToFit)
    {
      //****Scale fit all on display****
      x = an0 / 4.2625;
      y = an1 / 4.2625;
    }
    else
    {
      x = (an0-294) * 0.5172413793103448;
      y = (an1-264) * 0.5321507760532151;
    }
    
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
