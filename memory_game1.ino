#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,10,9,8,7);

const int ledPins[] = {2, 3, 4, 5};
const int speaker = 6;
const int userInput = 0;

int level = 0, gameMode = 0;
int lastLevelSeq[50];
bool started = false, gameover = false, soundEnabled = true;

void setup() {
  for (int i = 0; i < 4; i++) pinMode(ledPins[i], OUTPUT);
  pinMode(speaker, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("SIMON SAYS GAME");
  delay(3000);
  for (int i = 0; i < 14; i++) {
    lcd.scrollDisplayLeft(); delay(300);
  }
  lcd.clear(); lcd.print("Loading Game...");
  for (int i = 1; i <= 6; i++) {
    if (i < 5) lightLed(i);
    playTone(i); lightLed(0);
  }
}

void loop() {
  lcd.clear(); lcd.print("To Start");
  lcd.setCursor(0, 1); lcd.print("PRESS ANY BUTTON");
  while (!started) if (getButtonPressed() > 0) { playTone(5); started = true; }

  memset(lastLevelSeq, 0, sizeof(lastLevelSeq));
  delay(500);
  lcd.clear(); lcd.print("Select Game Mode");

  while (gameMode == 0) {
    lcd.setCursor(0, 1); lcd.print("1=P 2=R 4=TGLSND");
    switch (getButtonPressed()) {
      case 1: playTone(5); gameMode = 1; lcd.clear(); lcd.print("Progressive"); lcd.setCursor(0,1); lcd.print("Mode Selected"); break;
      case 2: playTone(5); gameMode = 2; lcd.clear(); lcd.print("Random"); lcd.setCursor(0,1); lcd.print("Mode Selected"); break;
      case 4:
        soundEnabled = !soundEnabled;
        lcd.setCursor(0,1);
        lcd.print(soundEnabled ? "Sound Mode: ON  " : "Sound Mode: OFF ");
        if (soundEnabled) playTone(5);
        delay(1000);
        break;
    }
  }

  delay(1000); level = 1;

  while (!gameover) {
    if (doLevel(level)) {
      lcd.clear(); lcd.print("LEVEL: "); lcd.print(level);
      lcd.setCursor(0,1); lcd.print("Completed......");
      if (soundEnabled) for (int i = 0; i < 3; i++) { playTone(5); delay(50); }
      else delay(125);
      delay(1000); level++;
    } else gameover = true;
  }

  lcd.clear(); lcd.print("WRONG! GAME OVER");
  lcd.setCursor(0,1); lcd.print("SCORE: "); lcd.print(level-1);
  if (soundEnabled) for (int i = 0; i < 5; i++) { playTone(6); delay(50); }
  else delay(125);

  lcd.clear(); lcd.print("Sequence");
  lcd.setCursor(0,1); lcd.print("should have been");
  delay(500);
  for (int s = 0; s < level; s++) playStep(lastLevelSeq[s]);

  lcd.clear(); lcd.print("Your Score: "); lcd.print(level-1);
  lcd.setCursor(0,1); lcd.print("PRESS ANY BUTTON");
  while (gameover) {
    if (getButtonPressed() > 0) {
      level = 0;
      if (soundEnabled) playTone(5);
      gameover = false; started = false; gameMode = 0;
    }
  }
}

int getButtonPressed() {
  int val = analogRead(userInput);
  if (val > 850) return 0;
  if (val < 600) return 1;
  if (val < 700) return 2;
  if (val < 800) return 3;
  return 4;
}

void playStep(int num) {
  lightLed(num);
  if (soundEnabled) playTone(num);
  else delay(100);
  delay(250); lightLed(0);
}

void lightLed(int led) {
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], (led == i + 1));
  if (led == 0) delay(50);
}

void playTone(int tone) {
  int tones[] = {1000, 1250, 1500, 1750, 500, 3000};
  for (long i = 0; i < 125; i++) {
    digitalWrite(speaker, HIGH);
    delayMicroseconds(tones[tone - 1]);
    digitalWrite(speaker, LOW);
    delayMicroseconds(tones[tone - 1]);
  }
}

bool doLevel(int lvl) {
  int steps[lvl], userStep, userLength = 0;
  bool levelPass = false, inProgress = true;
  
  lcd.clear(); lcd.print("Arduino Turn"); lcd.setCursor(0,1); lcd.print("Pay Attention");
  delay(1000);
  randomSeed(analogRead(0) + analogRead(1) + analogRead(2) + analogRead(3) + analogRead(4) + analogRead(5));

  for (int i = 0; i < lvl; i++) {
    steps[i] = (gameMode == 1 && i < lvl - 1) ? lastLevelSeq[i] : random(1, 5);
    lastLevelSeq[i] = steps[i];
  }

  for (int i = 0; i < lvl; i++) playStep(steps[i]);

  lcd.clear(); lcd.print("LEVEL: "); lcd.print(lvl);
  lcd.setCursor(0,1); lcd.print("Your Turn.....");

  while (inProgress) {
    userStep = getButtonPressed();
    if (userStep > 0) {
      userLength++; playStep(userStep);
      if (steps[userLength - 1] != userStep) return false;
      if (userLength == lvl) return true;
    }
  }
  return levelPass;
}

