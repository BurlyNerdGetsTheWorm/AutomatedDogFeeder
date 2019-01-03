//Last Edit: 2/10/18

#include <Stepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
//Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Assign I2C addresses to the motor shields
Adafruit_MotorShield AFMSbottom(0x61); // Rightmost jumper closed
Adafruit_MotorShield AFMStop(0x60); // Default address, no jumpers

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor ports #1 & #2 (M3 and M4)
Adafruit_StepperMotor *LetztyMotor = AFMSbottom.getStepper(200, 1);
Adafruit_StepperMotor *BeanMotor = AFMSbottom.getStepper(200, 2);
Adafruit_StepperMotor *SammichMotor = AFMStop.getStepper(200, 1);
//Adafruit_StepperMotor *FosterMotor = AFMStop.getStepper(200, 2);

int piezoPin = 2;
int SkipPin = 4;
int FeedPin = 5;
int WaitingPin = 6;
int WitheringPin = 7;
boolean ButtonPress = false;
unsigned long DelayDuration = 43170000; //This is where delay time is set. During testing it will be 30 seconds (30000), final should be 12 hours (43200000) or 11 hours, 59 minutes, and 30 seconds (43170000) to account for the time to dispense and initially reset the arduino at the end of normal feeding time, say, 8pm




void setup()
{
AFMS.begin(); // create with the default frequency 1.6KHz
//AFMS.begin(1000); // OR with a different frequency, say 1KHz

BeanMotor->setSpeed(150); // 150 rpm
LetztyMotor->setSpeed(150); // 150 rpm
SammichMotor->setSpeed(150); // 150 rpm

pinMode(FeedPin, INPUT_PULLUP);
pinMode(SkipPin, INPUT_PULLUP);
pinMode(WaitingPin, OUTPUT);
pinMode(WitheringPin, OUTPUT);
}

//declare dispense function
void Dispense()
{
//*Tone needs 2 arguments, but can take three
//1) Pin#
//2) Frequency - this is in hertz (cycles per second) which determines the pitch of the noise made
//3) Duration - how long the tone plays
tone(piezoPin, 1000, 600);
delay(2000);

float BeanPosition = 0.0;
float LetztyPosition = 0.0;
float SAmmichPosition = 0.0;
//set increments according to a fraction of Bean rotations out of Letzty rotations. This is where difference in amount of food is determined!
float BeanIncrement = 17.0/100.0; // 1/6 speed (since his food comes out faster and needs less)
float LetztyIncrement = 100.0/100.0; // Full speed
float SammichIncrement = 17.0/100.0; // 1/6 speed (since her food comes out faster and needs less)

//don't forget to set the 'less than' value of i to whatever dispenses the correct amount for Letzty, then adjust the percentage for Bean and Sammich. This is where the total number of increments is set. E.g. Letzty will turn a total of 60 full increments, while Bean will theoretically move 15 increments. 60 is about 9 rotations at 100%, so at 25%, Bean's dispenser would only rotate 2.25 times(ish).
for(int i=0; i<40; i++)
{

BeanPosition += BeanIncrement;
    LetztyPosition += LetztyIncrement;
        SammichPosition += SammichIncrement;

    BeanMotor->step(BeanPosition, FORWARD, SINGLE);
    LetztyMotor->step(LetztyPosition, FORWARD, SINGLE);
    SammichMotor->step(SammichPosition, FORWARD, SINGLE);
  }
// stop rotation and turn off holding torque.
BeanMotor->release();
LetztyMotor->release();
SammichMotor->release();
}

//declare button check function
void checkButtons()
{
  int  FeedButtonState = digitalRead(FeedPin);
  int  SkipButtonState = digitalRead(SkipPin);

  if(FeedButtonState == LOW)
  {
    delay(100);
    ButtonPress = true;
    Dispense();
  }

  if(SkipButtonState == LOW)
  {

    tone(piezoPin, 150, 300);
    delay(100);
    ButtonPress = true;
  }
}



//declare delay function
void myDelay()
{
  unsigned long StartOfDelay = millis();
  unsigned long WitheringHour = DelayDuration * .75;

  //Subtract current total duration from duration at start of delay. Keep running the check buttons function while the difference is less than the delay duration
  while (millis() - StartOfDelay <=DelayDuration) {
    checkButtons();  // check the buttons

  //Controls LEDs only during the last 3 hours
  if (millis() - StartOfDelay > WitheringHour) {
  switch (ButtonPress) {
case false:
digitalWrite(WitheringPin, HIGH);
digitalWrite(WaitingPin, LOW);
break;
case true:
digitalWrite(WitheringPin, LOW);
digitalWrite(WaitingPin, HIGH);
break;
default:
digitalWrite(WitheringPin, HIGH);
digitalWrite(WaitingPin, HIGH);
break;
}

    }
  }
}

void loop()
{
// your loop code here
ButtonPress = false;
digitalWrite(WitheringPin, LOW);
digitalWrite(WaitingPin, HIGH);
myDelay(); // call a new delay function that both delays and checks the button states.

if (ButtonPress == false)
{
Dispense();
}
}
