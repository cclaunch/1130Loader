// ============================================================================================
//
//   CONSOLE LOADER FOR IBM 1130
//
// ============================================================================================

// This section contains values to modify to configure this to your own choice of 
// command characters and any changes to Arduino Mega 2560 pin assignments

// Define Arduino pins for the console entry switches
#define CES0  38
#define CES1  39
#define CES2  40
#define CES3  41
#define CES4  42
#define CES5  43
#define CES6  44
#define CES7  45
#define CES8  46
#define CES9  47
#define CES10 48
#define CES11 49
#define CES12 50
#define CES13 51
#define CES14 52
#define CES15 53

// change these depending on whether your implementation sets CES on with high or low on the pins
#define CESON HIGH
#define CESOFF LOW

// Define Arduino pin to activate control of the 1130
#define ACTIVATE 6

// change these depending on whether you take control of buttons with high or low on the pin
#define ACTIVATEON LOW
#define ACTIVATEOFF HIGH

// Define Arduino pins for the Prog Start and Load IAR buttons
#define ProgStartNC 7
#define ProgStartNO 8
#define LoadIAR 9

// change these depending on whether you 'press' the button with high or low on the pin
#define BUTTONON HIGH
#define BUTTONOFF LOW

// change these to select alternate command characters
// the default characters match output of IBM 1130 Simulator at ibm1130.org
#define LOADCHAR '@'
#define ACTIVATECHAR '#'
#define STARTCHAR '='
#define ZEROCHAR 'Z'

// change this to emit a different prompt character to send the next line of file
#define PROMPT '>'

// global variables
int load = 0;
int deact = 0;
int act = 0;
int saved = 0;
int activated = 0;
int zero = 0;
char newword[5] = {0,0,0,0,0};
char saveaddr[5] = {0,0,0,0,0};

// this initializes the program at startup
void setup() {
  pinMode(CES0, OUTPUT);
  digitalWrite(CES0, CESOFF);
  pinMode(CES1, OUTPUT);
  digitalWrite(CES1, CESOFF);
  pinMode(CES2, OUTPUT);
  digitalWrite(CES2, CESOFF);
  pinMode(CES3, OUTPUT);
  digitalWrite(CES3, CESOFF);
  pinMode(CES4, OUTPUT);
  digitalWrite(CES4, CESOFF);
  pinMode(CES5, OUTPUT);
  digitalWrite(CES5, CESOFF);
  pinMode(CES6, OUTPUT);
  digitalWrite(CES6, CESOFF);
  pinMode(CES7, OUTPUT);
  digitalWrite(CES7, CESOFF);
  pinMode(CES8, OUTPUT);
  digitalWrite(CES8, CESOFF);
  pinMode(CES9, OUTPUT);
  digitalWrite(CES9, CESOFF);
  pinMode(CES10, OUTPUT);
  digitalWrite(CES10, CESOFF);
  pinMode(CES11, OUTPUT);
  digitalWrite(CES11, CESOFF);
  pinMode(CES12, OUTPUT);
  digitalWrite(CES12, CESOFF);
  pinMode(CES13, OUTPUT);
  digitalWrite(CES13, CESOFF);
  pinMode(CES14, OUTPUT);
  digitalWrite(CES14, CESOFF);
  pinMode(CES15, OUTPUT);
  digitalWrite(CES15, CESOFF);

  pinMode(ACTIVATE, OUTPUT);
  digitalWrite(ACTIVATE, ACTIVATEOFF); // operator has control of ProgStart until we activate

  pinMode(ProgStartNC, OUTPUT);
  digitalWrite(ProgStartNC, BUTTONOFF);
  pinMode(ProgStartNO, OUTPUT);
  digitalWrite(ProgStartNO, BUTTONOFF);

  pinMode(LoadIAR, OUTPUT);
  digitalWrite(LoadIAR, BUTTONOFF);

  // Start up our link with the terminal user
  Serial.begin(9600);
  Serial.println("IBM 1130 Loader is waiting for activation");

}

// tests an input word of four characters to determine if
// it is a valid hexadecimal value, returning true or false
bool checkword(char * theword) {
  int i;
  for (i = 0; i < 4; i++) {
    if (isHexadecimalDigit(theword[i]) == false) {
      return false;
    }
  }
  return true;
}

// function will convert the four ASCII characters to the hexadecimal value
// of each bit of each character, setting the CES bits on for any that are 1
void activateCES(char* theword) {
  int i;
  char bits;
    
  for (i=0; i<4; i++) {
    bits = theword[i];  // grab each hex character from input
    bits -= 48;         // ASCII digits are 48 to 57
    if (bits > 16) {
      bits -= 39;       // upper case characters A-F are 97-102 so 97 - 48 - 39 = 10
    }
    if (i == 0) {
      if (bitRead(bits,3) == 1) {
        digitalWrite(CES0, CESON);
      }
      if (bitRead(bits,2) == 1) {
        digitalWrite(CES1, CESON);
      }
      if (bitRead(bits,1) == 1) {
        digitalWrite(CES2, CESON);
      }
      if (bitRead(bits,0) == 1) {
        digitalWrite(CES3, CESON);
      }
    } else if (i == 1) {
      if (bitRead(bits,3) == 1) {
        digitalWrite(CES4, CESON);
      }
      if (bitRead(bits,2) == 1) {
        digitalWrite(CES5, CESON);
      }
      if (bitRead(bits,1) == 1) {
        digitalWrite(CES6, CESON);
      }
      if (bitRead(bits,0) == 1) {
        digitalWrite(CES7, CESON);
      }
    } else if (i == 2) {
      if (bitRead(bits,3) == 1) {
        digitalWrite(CES8, CESON);
      }
      if (bitRead(bits,2) == 1) {
        digitalWrite(CES9, CESON);
      }
      if (bitRead(bits,1) == 1) {
        digitalWrite(CES10, CESON);
      }
      if (bitRead(bits,0) == 1) {
        digitalWrite(CES11, CESON);
      }
    } else if (i == 3) {
      if (bitRead(bits,3) == 1) {
        digitalWrite(CES12, CESON);
      }
      if (bitRead(bits,2) == 1) {
        digitalWrite(CES13, CESON);
      }
      if (bitRead(bits,1) == 1) {
        digitalWrite(CES14, CESON);
      }
      if (bitRead(bits,0) == 1) {
        digitalWrite(CES15, CESON);
      }
    } 
  }
  
  return;
}

// Routine to set all CES to off
void turnoffCES() {
  digitalWrite(CES15, CESOFF);
  digitalWrite(CES14, CESOFF);
  digitalWrite(CES13, CESOFF);
  digitalWrite(CES12, CESOFF);
  digitalWrite(CES11, CESOFF);
  digitalWrite(CES10, CESOFF);
  digitalWrite(CES9, CESOFF);
  digitalWrite(CES8, CESOFF);
  digitalWrite(CES7, CESOFF);
  digitalWrite(CES6, CESOFF);
  digitalWrite(CES5, CESOFF);
  digitalWrite(CES4, CESOFF);
  digitalWrite(CES3, CESOFF);
  digitalWrite(CES2, CESOFF);
  digitalWrite(CES1, CESOFF);
  digitalWrite(CES0, CESOFF);
  return;
}

// main loop run continuously by the Arduino
void loop() {
  int incomingchar = 0;
  int pointer = 0;
  int times = 1;

  // first loop will collect inut from the serial connection to 
  // detect command characters and to assemble the four character
  // hex number that is an address, data or count
  Serial.println(PROMPT);
  while (1) {
    if (Serial.available() > 0) {
      incomingchar = Serial.read();
      if (isAlpha(incomingchar)) incomingchar = toupper(incomingchar);
      if ((incomingchar == '\n') or (incomingchar == '\r')) {
        if ((act == 1) or (deact) == 1) {      // activation character handled immediately, don't collect four characters
          pointer = 0;
          load = 0;
          break;                               // drop out of input routine and process the command request
        }
        if (pointer == 0) {
          Serial.println(PROMPT);
          zero = 0;
          load = 0;
          saved = 0;
          continue;                            // we reset and go back to collect another line of input from serial
        }
        if ((activated == 0) and (pointer > 0)) {
          Serial.println("ignoring command because loader is not activated");
          saved = 0;
          pointer = 0;
          zero = 0;
          load = 0;
          Serial.println(PROMPT);
          continue;                            // we reset and go back to collect another line of input from serial
        } else if ((pointer > 0) and (pointer != 4)) {
          Serial.print("dropping partial word or extraneous characters ");
          for (int i = 0; i <= pointer; i++) {
            Serial.write(newword[i]);
          }
          zero = 0;
          Serial.println("");
        }
        if (pointer == 4) {
          pointer = 0;
          break;                               // drop out input routine and process the command request
        }
        Serial.println(PROMPT);
        pointer = 0;
        load = 0;
        continue;                                 // end input loop and go execute
      }
      // turn on flags when we encounter the command characters
      if (incomingchar == ACTIVATECHAR) {      // request to activate or deactivate our control over the 1130
        if (activated == 1) {
          deact = 1;
          act = 0;
        } else {
          act = 1;
          deact = 0;
        }
        continue;                              // mark command and continue collecting the four characters
      }

      if (incomingchar == ZEROCHAR) {          // store NNNN words of zero in memory
        zero = 1;
        continue;                              // mark command and continue collecting the four characters
      }
      if (incomingchar == LOADCHAR) {          // load the IAR with value AAAA
        load = 1;
        continue;                              // mark command and continue collecting the four characters
      }
      if (incomingchar == STARTCHAR) {         // save the start address AAAA of this code
        saved = 1;
        continue;                              // mark command and continue collecting the four characters
      }

      // accumulate characters up to count of four then act on it if we are activated
      if ((act == 1) or (deact == 1)) {
         Serial.print("Activate character must be on a line of its own, ignoring ");
         Serial.write(incomingchar);
         Serial.println("");
         continue;                             // didn't save char, go back to read input
      }
      if (pointer < 4) {
        newword[pointer] = incomingchar;
      }
      pointer++;
      if (pointer > 4) {
        Serial.print("ignoring excess character ");
        Serial.write(incomingchar);
        Serial.println("");
      }
      if (pointer == 4) {
        if (activated == 0) {
          Serial.println("ignoring word because loader is not activated");
          saved = 0;
          pointer = 0;
          zero = 0;
          load = 0;
          continue;                            // we reset and go back to collect another line of input from serial
        } 
        if (checkword(newword)) {       // verify that XXXX is valid hexadecimal
          if (zero == 1) {                     // document request to zero out 0xNNNN words from this point onward
            times = (int)strtol(newword, NULL, 16);
            if (times < 1) {
              Serial.println("number of words to zero must range from 1 to 32,767");
              times = 1;
              zero = 0;
              pointer = 0;
              load = 0;
              Serial.println(PROMPT);
              continue;                        // we reset and go back to collect another line of input from serial
            }
            Serial.print("zeroing out ");
            Serial.print(times);
            Serial.println(" consecutive words");
          } else if (load == 1) {              // document we will load the IAR with this value
            Serial.print("setting IAR to "); Serial.println(newword);
          } else if (saved == 1) {             // document we are saving the start IAR of this file
            saved = 0;
            Serial.print("When done loading, start executing this code at "); Serial.println(newword);
            saveaddr[0] = newword[0];
            saveaddr[1] = newword[1];
            saveaddr[2] = newword[2];
            saveaddr[3] = newword[3];            
            pointer = 0;
            load = 0;
            continue;                           // we reset and go back to collect another line of input from serial
          } else {                              // document that we will store DDDD in memory
            Serial.print("loading "); Serial.println(newword);
          }
          continue;                             // go back to finish collecting line
        } else {
        Serial.println("not a valid hex word, dropped");
        pointer = 0;
        Serial.println(PROMPT);
        continue;                               // we reset and go back to collect another line of input from serial
        }
      }
    }
  }

  // execution section of the loop, once we collected a command request and a hex data value

  // first see if we need to deactivate control of the 1130
  if (deact == 1) {                             // we shut down the control over the 1130 -- the PROG START button works for operator

    // do we have a saved start address? If so, set IAR to that value
    if (saveaddr[0] != 0)  {
      Serial.print("we had a saved address, loading IAR with ");
      Serial.println((int)strtol(saveaddr, NULL, 16), HEX);
      activateCES(saveaddr);
      delay(150);
      digitalWrite(LoadIAR, BUTTONON);
      delay (500);
      digitalWrite(LoadIAR, BUTTONOFF);
      delay (100);
      turnoffCES();
    }

    // proceed with deactivation
    digitalWrite(ACTIVATE,  ACTIVATEOFF);
    digitalWrite(ProgStartNC, BUTTONOFF);
    Serial.println("Deactivated - Program Start button can be used normally");
    deact = 0;
    activated = 0;
    saveaddr[0] = 0;
  }
    
  // we have a valid word, lets load into the 1130 if we are activated
  if (activated == 1) {
      if (zero == 0) activateCES(newword);      // don't turn on any CES if storing zeroes
      for (int i = 1; i <= times; i++) {
        delay(150);
        if (load == 1) {                        // push LOAD IAR button to set the CES value in IAR
          digitalWrite(LoadIAR, BUTTONON);
          load = 0;
        } else {                                // push PROG START button to set the CES into memory at IAR address
          digitalWrite(ProgStartNC, BUTTONOFF);
          digitalWrite(ProgStartNO, BUTTONON);
        }
        delay (500);
        // release the PROG START and LOAD IAR buttons
        digitalWrite(ProgStartNO, BUTTONOFF);
        digitalWrite(ProgStartNC, BUTTONON);
        digitalWrite(LoadIAR, BUTTONOFF);
        delay (100);
        if (zero == 0) turnoffCES();            // if we may have set CES on, turn them all off
      }
      zero = 0;
      times = 1;
  }

  // lastly lets see if we need to activate control of the 1130
  if (act == 1) {
    digitalWrite(ProgStartNC, BUTTONON);        // this signal HIGH when button is not pushed
    digitalWrite(ProgStartNO, BUTTONOFF);       // this signal is LOW when button is not pushed
    digitalWrite(ACTIVATE,  ACTIVATEON);        // removes power from PROG START button to let us control 1130
    Serial.println("Activated - device has control of Program Start button");
    activated = 1;
    saveaddr[0] = 0;
    act = 0;
  }

}