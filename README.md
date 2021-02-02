# HEDCen Expo Group 10-ACTV 2019-2020

*School Chit System using Arduinos*

*Uploaded by Iggy Villa*

**Clarifications:**
1. Business Office Module code is the same as Canteen Module, just edit the code to rename it at 
```c++
64.   lcd.print("Office Module");
```
and change it to
```c++
64.   lcd.print("Canteen Module");
```
2. `moduleCode.ino` and `functions.ino` have to be in the same folder, as `moduleCode.ino` references functions in `functions.ino` such as `readBlock();` and `writeBlock();`.
#### Wiring:

![alt text](https://i.imgur.com/TqUbKWa.png "Wiring Diagram")
