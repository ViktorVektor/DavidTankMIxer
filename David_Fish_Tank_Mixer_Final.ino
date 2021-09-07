#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LowPower.h>

#define SERVO_PWM 5
#define MAX_PULSE_WIDTH 1250
#define MIN_PULSE_WIDTH 250

#define CYCLE_LENGTH 30  // 30 operations for a minute of mixing, since operations take 2s

Servo motor;

#define DISPLAY_SWITCH 3

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define CLK A1
#define DT A0
#define SW 2

int interval = 60; // default 30 minutes
int MIX_DURATION = 3; // default 3 minutes of mixing
int multiplier = 1;
int cycles = CYCLE_LENGTH * MIX_DURATION;

int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;

int pos = 0;

// // For a button interrupt
//void wakeUp()
//{
//  //Handler for pin interrupt
//  detachInterrupt(0);
//}

void setup() {
  // Clock Speed from 16Mhz to 8Mhz
  CLKPR = 0x80;
  CLKPR = 0x01;
  
  //Serial.begin(4800);

  pinMode(SERVO_PWM, OUTPUT);
  pinMode(DISPLAY_SWITCH, OUTPUT);
  pinMode(CLK,INPUT_PULLUP);
  pinMode(DT,INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  
  digitalWrite(DISPLAY_SWITCH, HIGH);

  //set servo to correct position at start
  motor.attach(SERVO_PWM, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  motor.writeMicroseconds(250);
  delay(300);
  motor.detach();
  
  // Read the initial state of CLK of encoder
  lastStateCLK = digitalRead(CLK);

  //attachInterrupt(0, wakeUp, INPUT);
  //delay(1000);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //rotate the screen since orienteation in the enclosure is upside down
  display.setRotation(2);
  
  // clear the buffer, setup the first text 
  display.clearDisplay();
  display.display();
  delay(50);
  
  // time setting function
  setInterval();

  // mix duration setting function
  setMixtime();

  // sleeping time display output
  deviceSleep();

//  display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(10,8);
//  display.print(MIX_DURATION);
//  display.display();
//  delay(2000);

  
  // begind going into sleep mode
  digitalWrite(DISPLAY_SWITCH, LOW);

  // requrested interval time in terms of 4s chunks (so 1 minute is 15x multiplier), 
  //minus 4s offset, interval between mixing considers mixing time.
  multiplier = ((interval - MIX_DURATION) * 15) - 1;
  cycles = CYCLE_LENGTH * MIX_DURATION;
 
  // default interval of 1 minute rest
  if(interval <= 1)
  {
    multiplier = 15; 
  }
}
  
void loop() {
  
  // servo mixing function
  spinCycle();

  for(int looper = 0; looper < multiplier; looper++)
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);  

}

void spinCycle()
{
//  digitalWrite(SERVO_SWITCH, HIGH);
  motor.attach(SERVO_PWM, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);

  // Using a modified 360-degree continuous SG90 Servo
  // Due to some electrical quirk, the servo stops spinning when not changing the pulse width.
  // MIN for CCW mixing, MAX for CW mixing
  for(int y = 0; y < (cycles*20); y++)
  {
      motor.writeMicroseconds(MAX_PULSE_WIDTH);
      delay(20);
      motor.writeMicroseconds(MAX_PULSE_WIDTH-100);
      delay(20);  
  }
//  // Code block for using a normal 180-degree SG90 Servo
//  //1 minute of mixing
//  for(int y = 0; y < cycles; y++)
//  {
//    // 2 second  operation
//    for (int x = MIN_PULSE_WIDTH; x < MAX_PULSE_WIDTH; x+=50)
//    {
//      motor.writeMicroseconds(x);
//      delay(25);
//    }
//    for (int x = MAX_PULSE_WIDTH; x > MIN_PULSE_WIDTH; x-=50)
//    {
//      motor.writeMicroseconds(x);
//      delay(25);
//    }
//  }
  motor.detach();
}
void setMixtime()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,8);
  display.print("Adjust Mix ->");
  display.display();
  boolean selection = true;

  while(selection)
  {
    // Read the current state of CLK
    currentStateCLK = digitalRead(CLK);
    
    // If last and current state of CLK are different, then pulse occurred
    // React to only 1 state change to avoid double count
    if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
  
      // If the DT state is different than the CLK state then
      // the encoder is rotating CCW so decrement
      if (digitalRead(DT) != currentStateCLK) {
        if (MIX_DURATION > 0) // handling negative values
        {
          MIX_DURATION--;
        }
        else
        {
          MIX_DURATION = 0;
        }
        
      } else {
        // Encoder is rotating CW so increment
        MIX_DURATION ++;
      }

      // display the current time setting
      display.clearDisplay();
      display.setCursor(10,2);
      display.setTextSize(1);
      display.print("Mix Interval: ");
      display.setCursor(0,15);
      display.setTextSize(2);
      display.print(MIX_DURATION);
      display.print("m");
      display.display();
    }
    
  
    // Remember last CLK state for encoder
    lastStateCLK = currentStateCLK;
  
    // Read the button state
    int btnState = digitalRead(SW);
  
    //If we detect LOW signal, button is pressed
    if (btnState == LOW) {
      //if 50ms have passed since last LOW pulse, it means that the
      //button has been pressed, released and pressed again
      if (millis() - lastButtonPress > 50) {

        //close the time selection loop
        selection = false;
        
        display.clearDisplay();
        display.setCursor(0,8);
        display.setTextSize(2);
        display.print("Mix Set!");
        display.display();
        delay(500);
      }
  
      // Remember last button press event
      lastButtonPress = millis();
    }
  
    // Put in a slight delay to help debounce the reading
    delay(1);
  }
}

void setInterval()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,8);
  display.print("Adjust Sleep ->");
  display.display();
  boolean selection = true;

  while(selection)
  {
    // Read the current state of CLK
    currentStateCLK = digitalRead(CLK);
    
    // If last and current state of CLK are different, then pulse occurred
    // React to only 1 state change to avoid double count
    if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
  
      // If the DT state is different than the CLK state then
      // the encoder is rotating CCW so decrement
      if (digitalRead(DT) != currentStateCLK) {
        if (interval > 0) // handling negative values
        {
          interval --;
        }
        else
        {
          interval = 0;
        }
        
      } else {
        // Encoder is rotating CW so increment
        interval ++;
      }

      // display the current time setting
      display.clearDisplay();
      display.setCursor(10,2);
      display.setTextSize(1);
      display.print("Time Interval: ");
      display.setCursor(0,15);
      display.setTextSize(2);
      display.print(interval);
      display.print("m");
      display.display();
    }
    
  
    // Remember last CLK state for encoder
    lastStateCLK = currentStateCLK;
  
    // Read the button state
    int btnState = digitalRead(SW);
  
    //If we detect LOW signal, button is pressed
    if (btnState == LOW) {
      //if 50ms have passed since last LOW pulse, it means that the
      //button has been pressed, released and pressed again
      if (millis() - lastButtonPress > 50) {

        //close the time selection loop
        selection = false;
        
        display.clearDisplay();
        display.setCursor(0,8);
        display.setTextSize(2);
        display.print("Sleep Set!");
        display.display();
        delay(500);
        
      }
  
      // Remember last button press event
      lastButtonPress = millis();
    }
  
    // Put in a slight delay to help debounce the reading
    delay(1);
  }

}

void deviceSleep()
{
  display.clearDisplay();
  display.setCursor(0,2);
  display.setTextSize(1);
  display.print("Sleep Time: ");
  display.print(interval);
  display.print("m");
  display.setCursor(0,12);
  display.print("Mix Time: ");
  display.print(MIX_DURATION);
  display.print("m");
  display.display();
  delay(1500);
}
