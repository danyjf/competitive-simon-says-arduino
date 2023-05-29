#include <Servo.h>

Servo servoMarble;
int servoMarblePin = 11;

int sensorPin = 5;

// multiplexer pins
int dataPin = 2;
int latchPin = 3;
int clockPin = 4;

// player 1 buttons
int p1Buttons[] = {A3, A2, 7, 8};
// player 1 LEDs
byte p1Leds[] = {
  B00000001, // red
  B00000010, // green
  B00000100, // blue
  B00001000  // yellow
};

// player 2 buttons
int p2Buttons[] = {A1, A0, 12, 13};
// player 2 LEDs
byte p2Leds[] = {
  B00010000, // red
  B00100000, // green
  B01000000, // blue
  B10000000  // yellow
};

int buzzerPin = 6;

float buzzerLedTones[] = {1046.5, 1174.66, 1318.51, 1396.91}; 
float buzzerWinTones[] = {525.63, 625.08, 701.63, 743.35, 701.63, 625.08, 525.63};
float buzzerMarbleTones[] = {440.0, 466.16, 493.88, 523.25};

int currentPlayer = 0;

// sequence with a max size of 100
int sequence[100] = {0};
// current sequence length
int length = 0;

void setup() 
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  
  pinMode(buzzerPin, OUTPUT);
  
  for (int i = 0; i < 4; i++)
  {
  	pinMode(p1Buttons[i], INPUT);
  }
  for (int i = 0; i < 4; i++)
  {
  	pinMode(p2Buttons[i], INPUT);
  }

  servoMarble.write(90);
  servoMarble.attach(servoMarblePin);

  pinMode(sensorPin, INPUT);

  Serial.begin(9600);
}

void loop() 
{
  waitForFirstPlayer();
  
  while (true)
  {
    // delay between player turns
    delay(500);
    nextPlayer();
    
  	playSequence();
    
    bool correct = verifyInputSequence();
    if (!correct)
    {
      lose();
      break;
    }
    
    addButtonToSequence();
  }
}

void waitForFirstPlayer()
{
  int pressedButton = -1;
  while (true)
  {
    pressedButton = checkButtons(p1Buttons);
    if (pressedButton != -1)
    {
      currentPlayer = 1;
      break;
    }
    
    pressedButton = checkButtons(p2Buttons);
    if (pressedButton != -1)
    {
      currentPlayer = 2;
      break;
    }
  }
  
  sequence[length] = pressedButton;
  length++;
  
  blinkLed(pressedButton, 100);
}

int checkButtons(int buttons[])
{
  for (int i = 0; i < 4; i++)
  {
    if (digitalRead(buttons[i]) == HIGH)
    {
		return i;
    }
  }
  return -1;
}

void blinkLed(int index, int delayMilli)
{
  byte* leds = p2Leds;
  if (currentPlayer == 1)
  {
  	leds = p1Leds;
  }
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, leds[index]);
  digitalWrite(latchPin, HIGH);
  
  tone(buzzerPin, buzzerLedTones[index]);
  delay(delayMilli);
  noTone(buzzerPin);
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
}

void nextPlayer() 
{
  if (currentPlayer == 1)
  {
    currentPlayer = 2;
  }
  else
  {
  	currentPlayer = 1;
  }
}

void playSequence()
{
  for (int i = 0; i < length; i++)
  {    
    blinkLed(sequence[i], 400);
    
    delay(100);
  }
}

bool verifyInputSequence() 
{
  int* buttons = p2Buttons;
  if (currentPlayer == 1)
  {
    buttons = p1Buttons;
  }
  
  for (int i = 0; i < length; i++)
  {
    while (true)
    {
      int pressedButton = checkButtons(buttons);
      if (pressedButton != -1)
      {
        if (pressedButton != sequence[i])
        {
          return false;
        }
        
        blinkLed(pressedButton, 100);
        
        while (true)
        {
          if (digitalRead(buttons[pressedButton]) == LOW)
          {
            break;
          }
        }
        
        break;
      }
    }
  }
  
  return true;
}

void lose()
{
  playWinMusic();

  nextPlayer();
  
  dispenseMarble();
  playMarbleMusic();
  
  waitForMarble();
  dispenseChocolate();

  delay(1000);
  length = 0;
  currentPlayer = 0; 
}

void addButtonToSequence()
{
  int* buttons = p2Buttons;
  if (currentPlayer == 1)
  {
    buttons = p1Buttons;
  }
  
  while (true)
  {
    int pressedButton = checkButtons(buttons);
    if (pressedButton != -1)
    {
      blinkLed(pressedButton, 100);
      sequence[length] = pressedButton;
      length++;
      break;
    }
  }
}

void playWinMusic()
{ 
  for (int i = 0; i < 7; i++)
  {
  	tone(buzzerPin, buzzerWinTones[i]);
  	delay(200);
  	noTone(buzzerPin);
  }
}

void dispenseMarble()
{
  int rotation = 0;
  if (currentPlayer == 2)
  {
    rotation = 180;
  }

  servoMarble.write(rotation);
  delay(1000);
  servoMarble.write(90);
}

void waitForMarble()
{
  while (true)
  {
    if (digitalRead(sensorPin) == 0)
    {
      break;
    }
  }
}

void dispenseChocolate()
{

}

void playMarbleMusic() 
{
  for (int i = 0; i < 4; i++)
  {
  	tone(buzzerPin, buzzerMarbleTones[i]);
    if (i == 3){
      delay(600);
    }
    else
    {
    	delay(200);
    }
  	noTone(buzzerPin);
  }
}
0