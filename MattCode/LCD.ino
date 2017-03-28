/* YourDuino.com Example Software Sketch
 20 character 4 line I2C Display
 Backpack Interface labelled "LCM1602 IIC  A0 A1 A2"
 terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <Wire.h>  // Comes with Arduino IDE
// Get the LCD I2C Library here: 
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.
#include <LiquidCrystal_I2C.h>

/*-----( Declare Constants )-----*/
//none
/*-----( Declare objects )-----*/
// set the LCD address to 0x20 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


/*-----( Declare Variables )-----*/
//none

void setupLCD()   /*----( SETUP: RUNS ONCE )----*/
{

 
  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines and turn on backlight

// ------- Quick 3 blinks of backlight  -------------
  lcd.backlight(); // finish with backlight on  
  
//-------- Write characters on the display ----------------
// NOTE: Cursor Position: CHAR, LINE) start at 0  
  lcd.setCursor(2,0); //Start at character 4 on line 0
  lcd.print("Robot booting up!");
  delay(500);

  lcd.setCursor(0,1);
  bars();
  delay(1000);
  if(bluetooth)
  {
    lcd.print("Connected to HC-06  ");
    delay(1000);  
    lcd.setCursor(0,2);
    lcd.print("Input commands:");  
    lcd.setCursor(0,3);
    delay(500);   
    lcd.print("(l,r,f,b,s)");
    delay(500);
  //lcd.autoscroll(); 
  }
  else
  {
    lcd.print("Robot starting  ");
    delay(1000);
  }
}
void bars()
{
  for(int i=0;i<20;i++)
  {
    lcd.write("*");
    delay(50);
  }
  lcd.setCursor(0,1);
  for(int i=0;i<20;i++)
  {
    lcd.print(" ");
    delay(50);
  }
    lcd.setCursor(0,1);
  lcd.print("  CONFIRMING LINK");
  for(int i=0;i<3;i++)
  {
  delay(250);
  lcd.print(".");
  }
  delay(500);
  lcd.print("                    ");
  lcd.setCursor(0,1);
}
void LCDPrintLine(int line, String newtext)
{
  lcd.setCursor(0,line); //set cursor to the line
  for(int i=0;i<20;i++) //loop and clear the line
  {
    lcd.print(" ");
  }
  lcd.setCursor(0,line);
  lcd.print(newtext); //print the text
}
void showText(String newtext) //vertical scrolling text array
{
  text[0]=text[1];
  text[1]=text[2];
  text[2]=text[3];
  text[3]=newtext;
  for(int i=0;i<=3;i++)
  {
      lcd.setCursor(0,i);
      lcd.print("                    ");
      lcd.setCursor(0,i);
      lcd.print(text[i]);
  }

}

/*
void loop() 
{
  {
    // when characters arrive over the serial port...
    if (Serial.available()) {
      // wait a bit for the entire message to arrive
      delay(100);
      // clear the screen
      lcd.clear();
      // read all the available characters
      while (Serial.available() > 0) {
        // display each character to the LCD
        lcd.write(Serial.read());
      }
    }
  }

}
*/

/* ( THE END ) */
