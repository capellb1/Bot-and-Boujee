#include <Wire.h>
#include <StackArray.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
StackArray <char> stack;
String text[4];
//gyro stuff
extern boolean gyroTurnTimeoutError;
extern boolean calibrate,testMode;
int ldist,rdist,fdist,flameCheck;
int distanceTiers[3];
int turnDirection;
int turnDegrees;
int loops=0; //how many tie we looped
//bluetooth
char command;
String string;
int rightTurns=0;
int leftTurns=0;
String distanceTxt[3]; //debug code
int lSpeed = 255;
int rSpeed = 255;
//Pin Assignments
int E1 = 6; //Speed 1
int M1 = 7; //Direction 1
int E2 = 5; //Speed 2                         
int M2 = 4; //Direction 2

int E3 = 35; //Fan
int M3 = 34;
boolean didtext=false;
boolean sensorDebug = true;
boolean useStack = true;
extern boolean bluetooth=false;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  Serial.println(("\n\n\nStarting up Gyro"));
  setupGyro(2000); // Configure Gyro
  setupLCD();
  delay(1500); //wait for the sensor to be ready   
  stack.setPrinter (Serial); 
  //testCalibrationResults();
}

//Robot code


void readSensors()
{
  int lsum=0;int rsum=0;int fsum=0;
  for(int i=0;i<=2;i++)
  {
    ldist = analogRead(0);
    fdist = analogRead(2);
    rdist = analogRead(1);
    lsum+=ldist;
    rsum+=rdist;
    fsum+=fdist;
    delay(50);
  }
  ldist = lsum/3;
  rdist = rsum/3;
  fdist = fsum/3;
  flameCheck = !(digitalRead(50));
  //Serial.println(String(lsum) + " " + String(fsum) + " " + String(rsum));
  if(sensorDebug)
    Serial.println("LFR Dist: " + String(ldist) + " " + fdist + " " + rdist);
}
int getDirectionToTurn()
{
    //if right is far, then turn right
    if(distanceTiers[2] == 0)
      return 1;
    if(distanceTiers[1] == 1 && distanceTiers[2] == 1) //if front is close and right is close, turn left
      return -1;
    
    return 0;
}
int avgSensor(int port) //take the average over time
{

  int sum=0;
  int testDistance;
  for(int i=0;i<=2;i++)
  {
    testDistance = analogRead(port);
    sum+=testDistance;
    delay(100);
  }
  return sum/3;
}
int confirmDirection(int dir)
{
  Serial.println("Trying to turn in direction " + String(dir));
  if(dir == -1 || dir == 1)
  {
    int sensor = avgSensor(dirToPort(dir));
  }
  return dir;
  return 0;
}
void parseSensors() //parse sensors into tiers
{
  //LEFT   FORWARD   RIGHT
  distanceTiers[0]=0;
  distanceTiers[1]=0;
  distanceTiers[2]=0;  
  if(sensorDebug)
  {
    distanceTxt[0] = "Far";
    distanceTxt[1] = "Far";
    distanceTxt[2] = "Far";
  }
  if (ldist > 370)
  {
    distanceTiers[0] = 1;
    distanceTxt[0] = "Close";
    //Serial.print("LEFT is Close ");
  }
  if (fdist > 300)
  {
    distanceTiers[1] = 1;
    distanceTxt[1] = "Close";
    //Serial.print("FRONT is Close ");
  }

  if (rdist > 370)
  {
    distanceTiers[2] = 1;
    distanceTxt[2] = "Close";
    //Serial.print("RIGHT is Close ");
  }
  if(sensorDebug)
  {
    Serial.println("[" + distanceTxt[0] + " " + distanceTxt[1] + " " + distanceTxt[2] + "]");
  }
}
void realign()
{
  int delta = ldist-rdist;
  if(abs(delta) > 60)
  {
    if(delta<0) //negative, rdist>ldist, lean right
    {
      Serial.println("Lean R");
    }
    if(delta>0)
    {
      Serial.println("Lean L");
    }
  }
}
void loop(){
  //Scan for walls
  
  if(bluetooth)
  {

    if (Serial.available() > 0)
      {string = "";}    
       while(Serial.available() > 0)
        {
         command = ((byte)Serial.read());
         if(command == ':')
         {
      break;
         }
         else
          {
            string += command;
         }
          delay(1);
       } 
       Serial.println(string);
       if(string != 0)
       {
            if(!didtext)
            lcd.clear();
            didtext=true;
       }
        if(string == "r")
        {
          showText("Right");
          Serial.println("BLUETOOTH: Turning Right");
          turnRight();
          gyroTurnTimeoutError = gyroBasedTurns(1, 90);  
          Serial.println("BLUETOOTH: Turn Complete"); 
          stopTurning();
        }   
        if(string =="l")
        {
          showText("Left");
          Serial.println("BLUETOOTH: Turning Left");
          turnLeft();
          gyroTurnTimeoutError = gyroBasedTurns(1, -90); 
          Serial.println("BLUETOOTH: Turn Complete");   
          stopTurning();
        }
         
        if(string =="sr")
        {
          showText("ShortRight");
          Serial.println("BLUETOOTH: Slight Right");
          turnRight();
          gyroTurnTimeoutError = gyroBasedTurns(1, 12); 
          Serial.println("BLUETOOTH: Turn Complete");   
          stopTurning();
        }
         
        if(string =="sl")
        {
          showText("ShortLeft");
          Serial.println("BLUETOOTH: Slight Left");
          turnLeft();
          gyroTurnTimeoutError = gyroBasedTurns(1, -12); 
          Serial.println("BLUETOOTH: Turn Complete");   
          stopTurning();
        }
        if(string =="s")
        {
         showText("Stop");
          Serial.println("BLUETOOTH: Stopping");
          stopTurning();
        }
    
        if(string =="f")
        {
          showText("Forward");
          Serial.println("BLUETOOTH: Forward");
          goForward();
        }
        
        if(string =="b")
        {
          showText("Backward");
          Serial.println("BLUETOOTH: Back");
          goBackwards();
        }
        if ((string.toInt()>=1)&&(string.toInt()<=255))
        {
          showText("Num: " + string);
        }
        string="";command=0;
     }
    
  else
    {
    readSensors();
    parseSensors();
    turnDirection = getDirectionToTurn(); // L = -1 R = 1 0 = Straight
    if(turnDirection != 0) //if its not go straight, then stop to turn
        stopTurning();
    turnDirection = confirmDirection(turnDirection); //confirm that it's not a random noise reading
    turnDegrees=turnDirection*90;
    Serial.println("Turn " + String(turnDegrees) + " deg " + dirToText(turnDirection) + "(" + turnDirection + ")");
    showText("Turn " + String(turnDegrees) + " deg " + dirToText(turnDirection) + "(" + turnDirection + ")");
    LCDPrintLine(0,"[" + distanceTxt[0] + " " + distanceTxt[1] + " " + distanceTxt[2] + "]");
    startTurning(turnDirection);
    gyroTurnTimeoutError = gyroBasedTurns(turnDirection, turnDegrees);   
    if(turnDirection != 0) //stop after turning
        stopTurning();
  
    if (gyroTurnTimeoutError)
    {
      Serial.println("Failed to turn... recalculating"); 
    }
    else 
    {
      Serial.println("Turn complete"); 
      if(turnDirection == 1) //if its a right turn 
      {
        delay(1000); //stop
        //inch forward
        Serial.println("Inching Forward"); 
        goForward();
        delay(1000); //stop
        stopTurning();
      }
    }
    
    loops++;
    if(loops>=10)
    {
      Serial.println("DUMPING STACK\n");
        while (!stack.isEmpty ())
      Serial.print (stack.pop ());
      Serial.print(" ");
      loops=0;
    }
    
    delay(1000);
  }
    
}
void startTurning(int dir)
{
  //Serial.println("Start Turning " + String(dir) + "!");
  if(dir == 1)
    turnRight();
  else if(dir==0)
    goForward();
  else if(dir==-1)
    turnLeft();
}
//********************************************
//Defining Robot's Potential Actions as functions
//Stop, move forward, turn left, turn right
//Call different order of functions depending on the potential
//situations in the maze (3-way fork, left only, etc.)
//********************************************

void stopTurning()
  {
    digitalWrite(M1,LOW);   
    digitalWrite(M2,LOW);       
    analogWrite(E1, LOW);   //PWM Speed Control
    analogWrite(E2, LOW);   //PWM Speed Control
    if(useStack)
    stack.push('*'); 
  }

void goForward()
  {
    digitalWrite(M1,HIGH);   
    digitalWrite(M2,HIGH);       
    analogWrite(E1, lSpeed);   //PWM Speed Control
    analogWrite(E2, rSpeed);   //PWM Speed Control
    if(useStack)
    stack.push('F'); 
  }

void turnRight()  //90 DEGREES CLOCKWISE
  {
    digitalWrite(M1,HIGH);   
    digitalWrite(M2,LOW);       
    analogWrite(E1, lSpeed);   //PWM Speed Control
    analogWrite(E2, rSpeed);   //PWM Speed Control 
    if(useStack)
    stack.push('R');   
    rightTurns++;  
  }

void turnLeft()  //90 DEGREES COUNTER-CLOCKWISE
  {
    digitalWrite(M1,LOW);   
    digitalWrite(M2,HIGH);       
    analogWrite(E1, lSpeed);   //PWM Speed Control
    analogWrite(E2, rSpeed);   //PWM Speed Control
    if(useStack) 
    stack.push('L');     
    leftTurns++;
  }

void goBackwards()
  {
    digitalWrite(M1,LOW);   
    digitalWrite(M2,LOW);       
    analogWrite(E1, lSpeed);   //PWM Speed Control
    analogWrite(E2, rSpeed);   //PWM Speed Control
    //stack.push('B'); 
  }
