# 1130Loader

This is a small modification to an IBM 1130 that allows users to enter commands from a PC either directly or through a file, entering data words into core. Manually toggling in programs is a time consuming affair, flipping the 16 Console Entry Switches and pushing the Program Start button to enter each word sequentially. Every change in the area of core to be loaded requires that the address be set in those entry switches and the Load IAR button be pressed. 

An Arduino and relay module is installed inside the console of the 1130, wired to switches in the console and 1053, then accessed via USB cable from an external peraonal computer or terminal. The interface is very simple, each command line is four hex digits with optional prefix characters to treat the word as an address instead of data or to enable/disable the control of the 1130. 
