#define CES15 22
#define CES14 23
#define CES13 24
#define CES12 25
#define CES11 26
#define CES10 27
#define CES9  28
#define CES8  29
#define CES7  30
#define CES6  31
#define CES5  32
#define CES4  33
#define CES3  34
#define CES2  35
#define CES1  36
#define CES0  37
#define ACTIVATE 4
#define ProgStart 5
#define LoadIAR 6

int load = 0;
int deact = 0;
int act = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(CES15, OUTPUT);
  pinMode(CES14, OUTPUT);
  pinMode(CES13, OUTPUT);
  pinMode(CES12, OUTPUT);
  pinMode(CES11, OUTPUT);
  pinMode(CES10, OUTPUT);
  pinMode(CES9, OUTPUT);
  pinMode(CES8, OUTPUT);
  pinMode(CES7, OUTPUT);
  pinMode(CES6, OUTPUT);
  pinMode(CES5, OUTPUT);
  pinMode(CES4, OUTPUT);
  pinMode(CES3, OUTPUT);
  pinMode(CES2, OUTPUT);
  pinMode(CES1, OUTPUT);
  pinMode(CES0, OUTPUT);
  pinMode(ACTIVATE, OUTPUT);
  pinMode(ProgStart, OUTPUT);
  pinMode(LoadIAR, OUTPUT);
  digitalWrite(CES15, LOW);
  digitalWrite(CES14, LOW);
  digitalWrite(CES13, LOW);
  digitalWrite(CES12, LOW);
  digitalWrite(CES11, LOW);
  digitalWrite(CES10, LOW);
  digitalWrite(CES9, LOW);
  digitalWrite(CES8, LOW);
  digitalWrite(CES7, LOW);
  digitalWrite(CES6, LOW);
  digitalWrite(CES5, LOW);
  digitalWrite(CES4, LOW);
  digitalWrite(CES3, LOW);
  digitalWrite(CES2, LOW);
  digitalWrite(CES1, LOW);
  digitalWrite(CES0, LOW);
  digitalWrite(ACTIVATE, LOW);
  digitalWrite(ProgStart, LOW);
  digitalWrite(LoadIAR, LOW);
  digitalWrite(ACTIVATE,  LOW);

  // Start up our link with the terminal user
  Serial.begin(9600);
  Serial.println("IBM 1130 Loader is waiting for activation");

}

bool checkword(char * theword) {
  int i;
  for (i = 0; i < 4; i++) {
    if (isHexadecimalDigit(theword[i]) == false) {
      return false;
    }
  }
  return true;
}

void activateRelays(char* theword) {
  int i;
  char bits;
  
  for (i=0; i<4; i++) {
    bits = theword[i];  // grab each hex character from input
    bits -= 48;         // ASCII digits are 48 to 57
    if (bits > 16) {
      bits -= 7;        // lower case characters A-F should be 10 to 15 but are 65-70 so 65 - 48 - 7 = 10
    }
    if (bits > 16) {
      bits -= 32;       // upper case characters A-F are 97-102 so 97 - 48 - 7 - 32 = 10
    }
    if (i == 0) {
      if (bitRead(bits,3) == 1) {
        digitalWrite(CES15, HIGH);
      }
      if (bitRead(bits,2) == 1) {
        digitalWrite(CES14, HIGH);
      }
      if (bitRead(bits,1) == 1) {
        digitalWrite(CES13, HIGH);
      }
      if (bitRead(bits,0) == 1) {
        digitalWrite(CES12, HIGH);
      }
    } else if (i == 1) {
      if (bitRead(bits,3) == 1) {
        digitalWrite(CES11, HIGH);
      }
      if (bitRead(bits,2) == 1) {
        digitalWrite(CES10, HIGH);
      }
      if (bitRead(bits,1) == 1) {
        digitalWrite(CES9, HIGH);
      }
      if (bitRead(bits,0) == 1) {
        digitalWrite(CES8, HIGH);
      }
    } else if (i == 2) {
      if (bitRead(bits,3) == 1) {
        digitalWrite(CES7, HIGH);
      }
      if (bitRead(bits,2) == 1) {
        digitalWrite(CES6, HIGH);
      }
      if (bitRead(bits,1) == 1) {
        digitalWrite(CES5, HIGH);
      }
      if (bitRead(bits,0) == 1) {
        digitalWrite(CES4, HIGH);
      }
    } else if (i == 3) {
      if (bitRead(bits,3) == 1) {
        digitalWrite(CES3, HIGH);
      }
      if (bitRead(bits,2) == 1) {
        digitalWrite(CES2, HIGH);
      }
      if (bitRead(bits,1) == 1) {
        digitalWrite(CES1, HIGH);
      }
      if (bitRead(bits,0) == 1) {
        digitalWrite(CES0, HIGH);
      }
    }
  }
  
  return;
}

void loop() {
  int incomingchar = 0;
  char newword[5] = {0,0,0,0,0};
  int pointer = 0;

  Serial.println(">");
  while (1) {
    if (Serial.available() > 0) {
      incomingchar = Serial.read();
      if ((incomingchar == '\n') or (incomingchar == '\r')) {
        if (pointer > 0) {
          Serial.println("dropping partial word");
        }
        pointer = 0;
        load = 0;
        continue;
      }
      if (incomingchar == '@') {
        if ((act == 1) or (deact == 1)) {
          Serial.println("Can't mix load IAR (@) and activation (#) in same word, ignored");
          deact = 0;
          act = 0;
          load = 0;
        }
        load = 1;
        continue;
      }
      if (incomingchar == '#') {
        load = 1;
        if (act == 1) {
          deact = 1;
        } else {
          act = 1;
        }
        continue;
      }
      newword[pointer++] = incomingchar;
      if (pointer > 4) {
        Serial.println("line too long, dropped");
        pointer = 0;
        continue;
      }
      if (pointer == 4) {
        if (checkword(newword)) {
          if (load == 1) {
            Serial.print("setting IAR to:"); Serial.println(newword);
          } else {
            Serial.print("loading:"); Serial.println(newword);
          }
          break;
        } else {
        Serial.println("not a valid hex word, dropped");
        pointer = 0;
        continue;
        }
      }
    }
  }

  // first see if we need to activate control of the 1130
  if (act == 1) {
    digitalWrite(ACTIVATE,  HIGH);
    Serial.println("Activated - device has control of Program Start button");
    act = 0;
  }

  // got a valid word, lets load into the 1130
  activateRelays(newword);
  delay(150);
  if (load == 1) {
    digitalWrite(LoadIAR, HIGH);
    load = 0;
  } else {
    digitalWrite(ProgStart, HIGH);
  }
  delay (500);
  digitalWrite(ProgStart, LOW);
  digitalWrite(LoadIAR, LOW);
  delay (100);
  digitalWrite(CES15, LOW);
  digitalWrite(CES14, LOW);
  digitalWrite(CES13, LOW);
  digitalWrite(CES12, LOW);
  digitalWrite(CES11, LOW);
  digitalWrite(CES10, LOW);
  digitalWrite(CES9, LOW);
  digitalWrite(CES8, LOW);
  digitalWrite(CES7, LOW);
  digitalWrite(CES6, LOW);
  digitalWrite(CES5, LOW);
  digitalWrite(CES4, LOW);
  digitalWrite(CES3, LOW);
  digitalWrite(CES2, LOW);
  digitalWrite(CES1, LOW);
  digitalWrite(CES0, LOW);

  // lastly, see if we need to deactivate control of the 1130
  if (deact == 1) {
    digitalWrite(ACTIVATE,  HIGH);
    Serial.println("Deactivated - Program Start button can be used normally");
    deact = 0;
  }
}
