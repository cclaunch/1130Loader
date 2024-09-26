# 1130Loader

This is a minor enhancement to an IBM 1130 that allows users to enter commands from a PC either directly or through a file, entering data words into core. This will be version 5 of the design, a significant change. 

Manually toggling in programs is a time consuming affair, flipping the 16 Console Entry Switches and pushing the Program Start button to enter each word sequentially. Every change in the area of core to be loaded requires that the address be set in those entry switches and the Load IAR button be pressed.

This device utilizes the cycle steal capability of the 1130, where peripheral controllers can read or write locations in memory by causing the CPU to pause while a memory cycle is used for the peripheral, then the CPU instruction processing continues. This is very similar to DMA on a microprocessor. 

When used with a terminal program that will send lines of a text file when it receives the prompt character from the loader, this allows files to be prepared to load into core of the 1130 in a rapid and reliable way. The format of the text file was chosen to match the simh based IBM 1130 Simulator memory dump format, thus programs can be tested on the simulator, dumped to files and loaded onto the physical 1130 system for execution. The format is to place each word as four ASCII characters represening the four hex digits. If the four digit line is prefixed by the command character @, the value is loaded into the 1130 IAR to set the location for subsequent memory words. If the line is prefixed by the command character Z it loads words of 0000 into as many locations as the four hex digits specify. If the command character prefix is =, the four digit value is saved and at the end of the loading, the IAR will be set to this value as the starting point for execution. Finally the command character # entered on a line by itself will toggle the loader to activation or deactivate it. Thus each simulator memory dump file needs to have a # line added at the front and at the end. 

To use the loader, connect a serial connection to the USB connector on the 1130 and open the terminal program. The 1130 must be stopped for this to runction, otherwise nothing special is needed. 

Physically the loader consists of an Arduino Mega 2560 and a custom shield that interfaces it to the IBM 1130. This is mounted on the logic gate B, connected to header boards on the gate which are then connected to individual backplane (board) pins using wirewrap. 

The code allows you to substitute different command characters and change the prompt character. 
