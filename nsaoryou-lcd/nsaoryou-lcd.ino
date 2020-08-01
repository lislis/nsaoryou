/*
 * NSA or you?
 * Hardware device for the Dining Cryptographers Problem
 *
 */

#include <EnableInterrupt.h>
#include <LiquidCrystal.h>

#define startMode 0
#define coinMode 1
#define personalSecret 2
#define publicAnnounce 3

#define modeBtn 8
#define inputBtn 7
#define resultBtn 10

#define DEBOUNCE_TIME 200

int mode;

bool coin = 0;

bool personalTotal = 0;
bool temporarySecret = 0;
int personalCounter = 0;

bool publicTotal = 0;
bool temporaryPublic = 0;
int publicCounter = 0;

long debounce_time_mode = 0;
long current_time_mode = 0;
long debounce_time_result = 0;
long current_time_result = 0;
long debounce_time_input = 0;
long current_time_input = 0;


const int rs = 11, en = 12, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*
 * Each mode change will reset all variables needed for the entered mode.
 * Each mode should be isolated and modes should not share information.
 * The operating human acts as memory.
 */
void changeMode() {
  current_time_mode = millis();
  if ((current_time_mode - debounce_time_mode) > DEBOUNCE_TIME) {
    mode++;
    if (mode > 4) {
      mode = 0;
    }
    switch (mode) {
      case startMode:
        Serial.println("NSA or YOU? start");
        break;
      case coinMode:
        coin = 0;
        Serial.println("Coin toss");
        lcd.setCursor(0,0);
        lcd.print("Coin toss?      ");
        break;
      case personalSecret:
        personalTotal = 0;
        personalCounter = 0;
        temporarySecret = 0;
        Serial.println("Personal secret");
        break;
      case publicAnnounce:
        publicTotal = 0;
        temporaryPublic = 0;
        publicCounter = 0;
        Serial.println("Public Mode \nPublicly announce secrets and find out if the NSA paid!");
        Serial.println("Press (I) to change input secret. Press (R) to get the result.");
        Serial.println("The counter tells you how many secrets you have announced.");
        break;
    }
  }
  debounce_time_mode = current_time_mode;
}

void setInput() {
  current_time_input = millis();
  if ((current_time_input - debounce_time_input) > DEBOUNCE_TIME) {
    switch (mode) {
      case startMode:
        break;

      case coinMode:
        break;

      case personalSecret:
        if (personalCounter == 0) {
          Serial.print("Your 1st secret: ");
        } else if (personalCounter == 1) {
          Serial.print("Your 2nd secret: ");
        }
        temporarySecret = !temporarySecret;
        Serial.println(temporarySecret);
        break;

      case publicAnnounce:
        if (publicCounter == 0) {
          Serial.print("1st public annoucement: ");
        } else if (publicCounter == 1) {
          Serial.print("2nd public announcement: ");
        } else {
          Serial.print(publicCounter + 1);
          Serial.print("th public announcement: ");
        }
        temporaryPublic = !temporaryPublic;
        Serial.println(temporaryPublic);
        break;
    }
  }
  debounce_time_input = current_time_input;
}

void getModeResult() {
  current_time_result = millis();
  if ((current_time_result - debounce_time_result) > DEBOUNCE_TIME) {
    switch (mode) {
      case startMode:
        break;

      case coinMode:
        Serial.print("Tossed a coin. You got ");
        coin = random(2);
        Serial.println(coin);
        break;

      case personalSecret:
        if (personalCounter == 0) {
          Serial.print("Set 1st secret: ");
          personalTotal = temporarySecret;
        } else if (personalCounter == 1) {
          Serial.print("Set 2nd secret: ");
          personalTotal = personalTotal ^ temporarySecret;
        }
        Serial.println(temporarySecret);
        temporarySecret = 0;
        personalCounter++;

        if (personalCounter >= 2) {
          Serial.print("Please announce ");
          Serial.print(personalTotal);
          Serial.println(" if you did not pay.");

          Serial.print("Please announce ");
          Serial.print(!personalTotal);
          Serial.println(" if you _did_ pay.");
        }
        break;

      case publicAnnounce:
        if (publicCounter == 0) {
          Serial.print("Set 1st public: ");
          publicTotal = temporaryPublic;
        } else if (publicCounter == 1) {
          Serial.print("Set 2nd public: ");
          publicTotal = publicTotal ^ temporaryPublic;
        } else {
          Serial.print((publicCounter + 1));
          Serial.print("th public: ");
          publicTotal = publicTotal ^ temporaryPublic;
        }
        Serial.println(temporaryPublic);
        temporaryPublic = 0;
        publicCounter++;

        if (publicCounter >= 3) {
          Serial.println("\n");
          if (publicTotal) {
            Serial.println("One of you paid");
          } else {
            Serial.println("The NSA paid");
          }
        }
        break;
    }
  }
  debounce_time_result = current_time_result;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  randomSeed(analogRead(0));

  pinMode(modeBtn, INPUT_PULLUP);
  pinMode(inputBtn, INPUT_PULLUP);
  pinMode(resultBtn, INPUT_PULLUP);

  enableInterrupt(modeBtn, changeMode, RISING);
  enableInterrupt(resultBtn, getModeResult, RISING);
  enableInterrupt(inputBtn, setInput, RISING);

  mode = 0;
  Serial.println("NSA or you?");
  Serial.println("Startmode");
}

void loop() {
  lcd.clear();
  switch (mode) {
      case startMode:
        lcd.setCursor(0,0);
        lcd.print("NSA or YOU?     ");
        break;
      case coinMode:
        lcd.setCursor(0,0);
        lcd.print("Coin toss?   (R)");
        lcd.setCursor(0,1);
        lcd.print(coin);
        break;
      case personalSecret:
        lcd.setCursor(0,0);
        lcd.print("Personal secret ");
        if (personalCounter == 0) {
          lcd.setCursor(0,1);
          lcd.print(temporarySecret);
          lcd.print(" (I)");
        } else if (personalCounter == 1) {
          lcd.setCursor(0,1);
          lcd.print(personalTotal);
          lcd.print(" ^ ");
          lcd.print(temporarySecret);
          lcd.print(" (I) -> (R)");
        }
        if (personalCounter >= 2) {
          lcd.setCursor(0,1);
          lcd.print("=> ");
          lcd.print(personalTotal);
          lcd.print(" (or ");
          lcd.print(!personalTotal);
          lcd.print(")  ");
        }
        break;
      case publicAnnounce:
        lcd.setCursor(0,0);
        lcd.print("Public announce");
        if (publicCounter == 0) {
          lcd.setCursor(0,1);
          lcd.print(temporaryPublic);
          lcd.print(" (I)");
        } else if (publicCounter == 1) {
          lcd.setCursor(0,1);
          lcd.print(publicTotal);
          lcd.print(" ^ ");
          lcd.print(temporaryPublic);
          lcd.print(" (I)");
        } else {
          lcd.setCursor(0,1);
          lcd.print(publicTotal);
          lcd.print(" ^ ");
          lcd.print(temporaryPublic);
          lcd.print(" (I) -> (R)");
        }

        if (publicCounter >= 3) {
          if (publicTotal) {
            lcd.setCursor(0,1);
            lcd.print("YOU");
          } else {
            lcd.setCursor(0,1);
            lcd.print("NSA");
          }
        }
        break;
    }  
    delay(20);
}
