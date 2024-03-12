# 1130Loader

This is a small modification to an IBM 1130 that allows users to enter commands from a PC either directly or through a file, entering data words into core. This is version 4 of the design, an important enhancement. 

Manually toggling in programs is a time consuming affair, flipping the 16 Console Entry Switches and pushing the Program Start button to enter each word sequentially. Every change in the area of core to be loaded requires that the address be set in those entry switches and the Load IAR button be pressed. 

When used with a terminal program that will send lines of a text file when it receives the prompt character from the loader, this allows files to be prepared to load into core of the 1130 in a rapid and reliable way. The format of the text file was chosen to match the simh based IBM 1130 Simulator memory dump format, thus programs can be tested on the simulator, dumped to files and loaded onto the physical 1130 system for execution. The format is to place each word as four ASCII characters represening the four hex digits. If the four digit line is prefixed by the command character @, the value is loaded into the 1130 IAR to set the location for subsequent memory words. If the line is prefixed by the command character Z it loads words of 0000 into as many locations as the four hex digits specify. If the command character prefix is =, the four digit value is saved and at the end of the loading, the IAR will be set to this value as the starting point for execution. Finally the command character # entered on a line by itself will toggle the loader to activation or deactivate it. Thus each simulator memory dump file needs to have a # line added at the front and at the end. 

To use the loader, connect a serial connection to the USB connector on the 1130 and open the terminal program. Turn the rotary mode dial on the IBM 1130 to Load and then begin transmitting the file to load memory. When the file is done and the loader is deactivated, turn the rotary mode switch back to Run and push Prog Start to begin executing code. 

Physically the loader consists of an Arduino Mega 2560 and a custom shield that interfaces it to the IBM 1130. One 16 position connector is wired to the 16 console entry switches on the faceplate of the console typewriter. A 6 position connector brings +12V power from the 1130 power supplies and connects wires to the Program Start and Load IAR buttons on the side of the 1130 keyboard. The loader is mounted on the inside of the console typewriter, behind the front panel below the three pushbuttons Tab, Space and Return. The USB connector is mounted elsewhere on the 1130 (TBD). 

The code allows you to reassign Arduino pins, change the polarity of activation of the pins, substitute different command characters and change the prompt character. 

This project still needs the mounting design to be included. 
