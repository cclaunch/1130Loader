// ============================================================================================
//
//   MEMORY LOADER FOR IBM 1130     Version 5
//
// ============================================================================================
//
//  A shield on the Arduino has a state machine and gates that interface with an IBM 1130
//  to cause the CPU to do a cycle steal, which is IBM's term for what is more commonly
//  known as Direct Memory Access (DMA)
//
//  The Arduino sets and address on 15 pins and a data word on another 16 pins, then
//  sets the 'ardreq' pin high to trigger the shield's state machine.
//  
//  The shield requires that the 1130 be in the stopped state, not executing instructions
//  as that simplified the design substantially. It will also reset when the RESET button
//  of the 1130 is pushed (or at power on time)
//
//  The state machine, once triggered, will raise the request line to the CPU asking
//  for a cycle steal to be performed. As the 1130 enters cycle steal mode, the state 
//  machine waits until the machine reaches the X6 clock state, where it drop the
//  cycle steal request. It then waits for CS Level 0 tto drop then it raises the
//  'arddone' pin back to the Arduino. The Arduino should drop its request for the 
//  cycle steal, after which the state machine goes back to idle waiting for the
//  next request.
//
//  Since a cycle steal occurs in one memory cycle of the 1130, either 3.6 or 2.5
//  microseconds, it is extremely fast. The rate at which lines are sent over the 
//  USB cable from the remote PC/Mac/Terminal/etc will be the pacing factor in 
//  how fast memory can be loaded, but at the default 9600 baud it should load
//  around 120 words per second, thus each 4K of memory loads in about half
//  a minute. Each address loading command requires 1/120 of a second but
//  commands to zero a block of data can run much faster.
//
//  The largest possible 1130 configuration (32K) would require
//  four to five minutes to load at the default 9600 baud rate.

//  Boosting the serial port rate on both Arduino and terminal/PC to
//  115,200 baud will increase that speed of loading 4K to a bit over 2 seconds
//  and thus a 32K 1130 configuration would load in under a half minute.
//   

// This section contains values to modify to configure this to your own choice of 
// command characters and any changes to Arduino Mega 2560 pin assignments

#define ardreq    10          // request for a cycle steal sent to the shield
#define arddone   11          // response from shield when a cycle is complete

#define A1        38          // bit 1 of the address to store the data
#define A2        39          // bit 2 of the address to store the data
#define A3        40          // bit 3 of the address to store the data
#define A4        41          // bit 4 of the address to store the data
#define A5        42          // bit 5 of the address to store the data
#define A6        43          // bit 6 of the address to store the data
#define A7        44          // bit 7 of the address to store the data
#define A8        45          // bit 8 of the address to store the data
#define A9        46          // bit 9 of the address to store the data
#define A10       47          // bit 10 of the address to store the data
#define A11       48          // bit 11 of the address to store the data
#define A12       49          // bit 12 of the address to store the data
#define A13       50          // bit 13 of the address to store the data
#define A14       51          // bit 14 of the address to store the data
#define A15       52          // bit 15 of the address to store the data

#define D0        22          // bit 0 of the data value to store
#define D1        23          // bit 1 of the data value to store
#define D2        24          // bit 2 of the data value to store
#define D3        25          // bit 3 of the data value to store
#define D4        26          // bit 4 of the data value to store
#define D5        27          // bit 5 of the data value to store
#define D6        28          // bit 6 of the data value to store
#define D7        29          // bit 7 of the data value to store
#define D8        30          // bit 8 of the data value to store
#define D9        31          // bit 9 of the data value to store
#define D10       32          // bit 10 of the data value to store
#define D11       33          // bit 11 of the data value to store
#define D12       34          // bit 12 of the data value to store
#define D13       35          // bit 13 of the data value to store
#define D14       36          // bit 14 of the data value to store
#define D15       37          // bit 15 of the data value to store

#define BITON     HIGH
#define BITOFF    LOW

#define COMMRATE  9600        // can be up to 115200 or more depending on accuracy of sending UART clock

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
long currentaddress;
long outputdata;

// this initializes the program at startup
void setup() {

  pinMode(arddone, INPUT);
  pinMode(ardreq, OUTPUT);
//  digitalWrite(ardreq, LOW);      this is default behavior when pin set to output

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D9, OUTPUT);
  pinMode(D10, OUTPUT);
  pinMode(D11, OUTPUT);
  pinMode(D12, OUTPUT);
  pinMode(D13, OUTPUT);
  pinMode(D14, OUTPUT);
  pinMode(D15, OUTPUT);
  digitalWrite(D0,LOW);
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);
  digitalWrite(D3,LOW);
  digitalWrite(D4,LOW);
  digitalWrite(D5,LOW);
  digitalWrite(D6,LOW);
  digitalWrite(D7,LOW);
  digitalWrite(D8,LOW);
  digitalWrite(D9,LOW);
  digitalWrite(D10,LOW);
  digitalWrite(D11,LOW);
  digitalWrite(D12,LOW);
  digitalWrite(D13,LOW);
  digitalWrite(D14,LOW);
  digitalWrite(D15,LOW);

  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(A8, OUTPUT);
  pinMode(A9, OUTPUT);
  pinMode(A10, OUTPUT);
  pinMode(A11, OUTPUT);
  pinMode(A12, OUTPUT);
  pinMode(A13, OUTPUT);
  pinMode(A14, OUTPUT);
  pinMode(A15, OUTPUT);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
  digitalWrite(A3,LOW);
  digitalWrite(A4,LOW);
  digitalWrite(A5,LOW);
  digitalWrite(A6,LOW);
  digitalWrite(A7,LOW);
  digitalWrite(A8,LOW);
  digitalWrite(A9,LOW);
  digitalWrite(A10,LOW);
  digitalWrite(A11,LOW);
  digitalWrite(A12,LOW);
  digitalWrite(A13,LOW);
  digitalWrite(A14,LOW);
  digitalWrite(A15,LOW);

  // Start up our link with the terminal user
  // this remains active anytime the 1130 is running
  // with no reset upon connection due to capacitor
  // across the reset and ground pins
  Serial.begin(COMMRATE);
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
// of that address or data, returning the value to the main routine
long grabvalue(char* theword) {
  int i;
  char bits;
  long anAddress = 0;
    
  for (i=0; i<4; i++) {
    bits = theword[i];  // grab each hex character from input
    bits -= 48;         // ASCII digits are 48 to 57
    if (bits > 16) {
      bits -= 39;       // upper case characters A-F are 97-102 so 97 - 48 - 39 = 10
    }
    if (i == 0) {
      if (bitRead(bits,3) == 1) {
        anAddress += 32768;
      }
      if (bitRead(bits,2) == 1) {
        anAddress += 16384;
      }
      if (bitRead(bits,1) == 1) {
        anAddress += 8192;
      }
      if (bitRead(bits,0) == 1) {
        anAddress += 4096;
      }
    } else if (i == 1) {
      if (bitRead(bits,3) == 1) {
        anAddress += 2048;
      }
      if (bitRead(bits,2) == 1) {
        anAddress += 1024;
      }
      if (bitRead(bits,1) == 1) {
        anAddress += 512;
      }
      if (bitRead(bits,0) == 1) {
        anAddress += 256;
      }
    } else if (i == 2) {
      if (bitRead(bits,3) == 1) {
        anAddress += 128;
      }
      if (bitRead(bits,2) == 1) {
        anAddress += 64;
      }
      if (bitRead(bits,1) == 1) {
        anAddress += 32;
      }
      if (bitRead(bits,0) == 1) {
        anAddress += 16;
      }
    } else if (i == 3) {
      if (bitRead(bits,3) == 1) {
        anAddress += 8;
      }
      if (bitRead(bits,2) == 1) {
        anAddress += 4;
      }
      if (bitRead(bits,1) == 1) {
        anAddress += 2;
      }
      if (bitRead(bits,0) == 1) {
        anAddress += 1;
      }
    } 
  }
  
  return anAddress;
}

// this sets up the address and data pins on the shield then raises a request line
// returns once the cycle steal is complete as verified by arddone signal
void store2memory() {

  // first set up address for cycle steal
  if (bitRead(currentaddress,14)== 1) {
    digitalWrite(A1,BITON);
  } else {
    digitalWrite(A1,BITOFF);
  }
  if (bitRead(currentaddress,13)== 1) {
    digitalWrite(A2,BITON);
  } else {
    digitalWrite(A2,BITOFF);
  }
  if (bitRead(currentaddress,12)== 1) {
    digitalWrite(A3,BITON);
  } else {
    digitalWrite(A3,BITOFF);
  }
  if (bitRead(currentaddress,11)== 1) {
    digitalWrite(A4,BITON);
  } else {
    digitalWrite(A4,BITOFF);
  }
  if (bitRead(currentaddress,10)== 1) {
    digitalWrite(A5,BITON);
  } else {
    digitalWrite(A5,BITOFF);
  }
  if (bitRead(currentaddress,9)== 1) {
    digitalWrite(A6,BITON);
  } else {
    digitalWrite(A6,BITOFF);
  }
  if (bitRead(currentaddress,8)== 1) {
    digitalWrite(A7,BITON);
  } else {
    digitalWrite(A7,BITOFF);
  }
  if (bitRead(currentaddress,7)== 1) {
    digitalWrite(A8,BITON);
  } else {
    digitalWrite(A8,BITOFF);
  }
  if (bitRead(currentaddress,6)== 1) {
    digitalWrite(A9,BITON);
  } else {
    digitalWrite(A9,BITOFF);
  }
  if (bitRead(currentaddress,5)== 1) {
    digitalWrite(A10,BITON);
  } else {
    digitalWrite(A10,BITOFF);
  }
  if (bitRead(currentaddress,4)== 1) {
    digitalWrite(A11,BITON);
  } else {
    digitalWrite(A11,BITOFF);
  }
  if (bitRead(currentaddress,3)== 1) {
    digitalWrite(A12,BITON);
  } else {
    digitalWrite(A12,BITOFF);
  }
  if (bitRead(currentaddress,2)== 1) {
    digitalWrite(A13,BITON);
  } else {
    digitalWrite(A13,BITOFF);
  }
  if (bitRead(currentaddress,1)== 1) {
    digitalWrite(A14,BITON);
  } else {
    digitalWrite(A14,BITOFF);
  }
  if (bitRead(currentaddress,0)== 1) {
    digitalWrite(A15,BITON);
  } else {
    digitalWrite(A15,BITOFF);
  }

  // now set up data word for cycle steal
  if (bitRead(outputdata,15) == 1) {
    digitalWrite(D0, BITON); 
  } else {
    digitalWrite(D0, BITOFF);
  }
  if (bitRead(outputdata,14) == 1) {
    digitalWrite(D1, BITON); 
  } else {
    digitalWrite(D1, BITOFF);
  }
  if (bitRead(outputdata,13) == 1) {
    digitalWrite(D2, BITON); 
  } else {
    digitalWrite(D2, BITOFF);
  }
  if (bitRead(outputdata,12) == 1) {
    digitalWrite(D3, BITON); 
  } else {
    digitalWrite(D3, BITOFF);
  }
  if (bitRead(outputdata,11) == 1) {
    digitalWrite(D4, BITON); 
  } else {
    digitalWrite(D4, BITOFF);
  }
  if (bitRead(outputdata,10) == 1) {
    digitalWrite(D5, BITON); 
  } else {
    digitalWrite(D5, BITOFF);
  }
  if (bitRead(outputdata,9) == 1) {
    digitalWrite(D6, BITON); 
  } else {
    digitalWrite(D6, BITOFF);
  }
  if (bitRead(outputdata,8) == 1) {
    digitalWrite(D7, BITON); 
  } else {
    digitalWrite(D7, BITOFF);
  }
  if (bitRead(outputdata,7) == 1) {
    digitalWrite(D8, BITON); 
  } else {
    digitalWrite(D8, BITOFF);
  }
  if (bitRead(outputdata,6) == 1) {
    digitalWrite(D9, BITON); 
  } else {
    digitalWrite(D9, BITOFF);
  }
  if (bitRead(outputdata,5) == 1) {
    digitalWrite(D10, BITON); 
  } else {
    digitalWrite(D10, BITOFF);
  }
  if (bitRead(outputdata,4) == 1) {
    digitalWrite(D11, BITON); 
  } else {
    digitalWrite(D11, BITOFF);
  }
  if (bitRead(outputdata,3) == 1) {
    digitalWrite(D12, BITON); 
  } else {
    digitalWrite(D12, BITOFF);
  }
  if (bitRead(outputdata,2) == 1) {
    digitalWrite(D13, BITON); 
  } else {
    digitalWrite(D13, BITOFF);
  }
  if (bitRead(outputdata,1) == 1) {
    digitalWrite(D14, BITON); 
  } else {
    digitalWrite(D14, BITOFF);
  }
  if (bitRead(outputdata,0) == 1) {
    digitalWrite(D15, BITON); 
  } else {
    digitalWrite(D15, BITOFF);
  }     
 
  //  request cycle steal to be performed by shield
  digitalWrite(ardreq, HIGH);

  // now spin waiting until the operation completes
  while (digitalRead(arddone) == 0);

  // drop request and go away
  digitalWrite(ardreq, LOW);
  return;
}

// main loop run continuously by the Arduino
void loop() {
  int incomingchar = 0;
  int pointer = 0;
  int times = 1;
  long returnvalue;

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
      if ((incomingchar == ' ') and (pointer == 0)) {
        continue;
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
          Serial.print("IBM 1130 Loader is waiting for activation\nPress Imm Stop to be certain system is stopped.\n");
          Serial.println("# is command to activate the loader.\n");
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
  if (deact == 1) {                             // we shut down the control over the 1130

    // do we have a saved start address? If so, set IAR to that value
    if (saveaddr[0] != 0)  {
      Serial.print("we had a saved address, please load IAR with ");
      Serial.print((int)strtol(saveaddr, NULL, 16), HEX);
      Serial.println(" then hit Prog Start button to execute");
    }

    // proceed with deactivation
    Serial.println("Deactivated - use 1130 as you wish");
    deact = 0;
    activated = 0;
    saveaddr[0] = 0;
    currentaddress = 0;
  }
    
  // we have a valid word, lets load into the 1130 if we are activated
  if (activated == 1) {
        returnvalue = grabvalue(newword);      // don't turn on any CES if storing zeroes
      if (zero == 1) {
        Serial.print("zeroing out ");
        Serial.print(times);
        Serial.print(" (hex ");
        Serial.print(times,HEX);
        Serial.println(") consecutive words");
        returnvalue = 0;
      }
      for (int i = 1; i <= times; i++) {
        if (load == 1) {                        // change the current address to the value from the command line
          Serial.print("next store at "); Serial.println(newword);
          currentaddress = returnvalue;
          load = 0;
        } else {                               // grab the value from the command line and ask the shield to put it in memory
          if (zero == 0) {
            Serial.print(newword);
            Serial.print(" @"); Serial.println(currentaddress, HEX);
          }
          outputdata = returnvalue;
          store2memory();
          currentaddress++;                     // bump the core address to the next sequential location
        }
      }
      zero = 0;
      times = 1;
  }

  // lastly lets see if we need to activate control of the 1130
  if (act == 1) {
    Serial.println("Activated");
    activated = 1;
    saveaddr[0] = 0;
    act = 0;
    currentaddress = 0;              // default core address of 0000 until changed by @ command
  }

}