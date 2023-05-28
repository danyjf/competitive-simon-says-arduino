int dataPin = 2;
int latchPin = 3;
int clockPin = 4;

int p1Buttons[] = {A3, A2, 7, 8};
byte p1Leds[] = {
  B00000001, // red
  B00000010, // green
  B00000100, // blue
  B00001000  // yellow
};

int p2Buttons[] = {A1, A0, 12, 13};
byte p2Leds[] = {
  B00010000, // red
  B00100000, // green
  B01000000, // blue
  B10000000  // yellow
};

int buzzerPin = 6;

int buzzerLedTones[] = {1046.5, 1174.66, 1318.51, 1396.91}; 
int buzzerWinTones[] = {525.63, 625.08, 701.63, 743.35, 701.63, 625.08, 525.63};
int buzzerMarbleTones[] = {440.0, 466.16, 493.88, 523.25};

int currentPlayer = 0;

int sequence[100] = {0};
int length = 0;

bool lost = false;

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
  
  Serial.begin(9600);
}

void loop() 
{
  lost = false;
  getFirstPlay();

  while (true)
  {
    delay(500);
    playSequence();
    
    // check whether it is the player 1 or player 2 turn
    // and assign the variables accordingly
    int* buttons = p1Buttons;
    byte* leds = p1Leds;
    if (currentPlayer == 2)
    {
      buttons = p2Buttons;
      leds = p2Leds;
    }

    // player does the sequence
    for (int i = 0; i < length; i++)
    {
      int pressedButton = waitForButtonPress(buttons);
      
      if (pressedButton != sequence[i])
      {
        lose();
        break;
      }
      
      blinkLed(leds, pressedButton, 100);

      while (true)
      {
        if (digitalRead(buttons[pressedButton]) == LOW)
        {
          break;
        }
      }
    }
    
    if (lost)
    {
      break;
    }

    // player sets the next button in the sequence
    int pressedButton = waitForButtonPress(buttons);
    blinkLed(leds, pressedButton, 100);
    sequence[length] = pressedButton;
    length += 1;

    if (currentPlayer == 2)
    {
      currentPlayer = 1;
	  }
	  else
	  {
	    currentPlayer = 2;
	  }
  }
}

void lose()
{
  Serial.println(currentPlayer + " lost!");
  lost = true;
  length = 0;
  currentPlayer = 0;
  
  playWinMusic();

  delay(1000);
}

int waitForButtonPress(int buttons[])
{
  while (true)
  {
  	int pressedButton = getPressedButton(buttons);
    if (pressedButton != -1)
    {
      return pressedButton;
    }
  }
}

void playSequence()
{
  byte* leds = p2Leds;
  if (currentPlayer == 1)
  {
  	leds = p1Leds;
  }
  
  for (int i = 0; i < length; i++)
  {    
    blinkLed(leds, sequence[i], 400);
    
    delay(100);
  }
}

// Get the first play, this will decide who is the 
// first player and who is the second
void getFirstPlay()
{
  int pressedButton = -1;
  while (true)
  {
    pressedButton = getPressedButton(p1Buttons);
    if (pressedButton != -1)
    {
      currentPlayer = 2;
      blinkLed(p1Leds, pressedButton, 100);
      break;
    }

    pressedButton = getPressedButton(p2Buttons);
    if (pressedButton != -1)
    {
      currentPlayer = 1;
      blinkLed(p2Leds, pressedButton, 100);
      break;
    }
  }
  
  sequence[length] = pressedButton;
  length += 1;
}

int getPressedButton(int buttons[])
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

void blinkLed(byte leds[], int index, int delayMilli)
{
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

void playWinMusic()
{ 
  for (int i = 0; i < 7; i++)
  {
  	tone(buzzerPin, buzzerWinTones[i]);
  	delay(200);
  	noTone(buzzerPin);
  }
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
