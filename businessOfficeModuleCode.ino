  /*
   * Author: Expo Group 10-ACTV
   * Project Title: Arduino Canteen Chit System
   * Started in January 2020
   * Debugging finished on April 25
   */
  
  #include <Key.h>
  #include <Keypad.h>
  #include <LiquidCrystal_I2C.h>
  #include <SPI.h>
  #include <MFRC522.h>
  #include "pitches.h"
  
  
  #define SS_PIN 10 //slave select pin
  #define RST_PIN 8 //reset pin
  MFRC522 mfrc522(SS_PIN, RST_PIN);
  MFRC522::MIFARE_Key key;
  
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  
  const byte ROWS = 4; //four rows
  const byte COLS = 4; //four columns
  //define the symbols on the buttons of the keypads
  char hexaKeys[ROWS][COLS] = {
    {'1','2','3','+'},
    {'4','5','6','='},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  
  byte rowPins[ROWS] = {12, 11, 7, 6}; //connect to the row pinouts of the keypad
  byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
  
  Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
  
  int snackerBlock = 5;
  bool activate = false;
  bool add = true;
  bool isNegative = false;
  bool isSnacker;
  long firstnum = 0;
  long secondnum = 0; 
  int keyPressed = 0;
  
  void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("Scan a MIFARE Classic Card");
  
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
  
    // 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
  
    //Starting Sequence (LCD)
    lcd.begin();
    lcd.backlight();
    lcd.setCursor (1,0);
    lcd.print("Office Module");
    lcd.setCursor (2,1);
    lcd.print("Version 1.0");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Enter Price:");
    if (add == true) {
      lcd.setCursor(13,0);
      lcd.print("(+)");
    } else {
      lcd.setCursor(13,0);
      lcd.print("(-)");
    }
    lcd.setCursor(0,1);
  }
  
  int block = 4; //Block where we store the read data
  byte point[16]; //Where the balance of the cardholder will be stored first
  byte clear_point[16];
  byte read_point[18]; //Checking if we increased or decreased point
  byte snacker_read_point[18]; //For snacker
  byte snackerConfirm[16];
  
  void loop() {
  
    //Calculator Part
    //1-6 keys adds or subtracts in multiples of 5
    //7-9 keys adds or subtracts 1, 2 or 3 from the balance
  
    static int num = 0;
    static long total = 0; //Balance
    static int col = 0;
    
    char customKey = customKeypad.getKey();
  
    /*
    if (customKey == '+') {
      num++;
      Serial.print(num);
    }
    
    if (customKey && customKey != 'D'){
      lcd.setCursor(col, 1);
      col++;
      lcd.print(customKey);
      int key = customKey;
      
     if (num == 0 && isDigit(key)){ 
        firstnum = (firstnum * 10 + key) - 48;
        Serial.println(firstnum);  
      }
      
     if (num == 1 && isDigit(key)) {
        secondnum = (secondnum * 10 + key) - 48;
        Serial.println(secondnum);
      }
      
    }
    */
    
    if (customKey == '+') {
       if (add == false) {
          add = true;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Addition Mode");
          delay(1000);
          lcd.setCursor(0,0);
          lcd.clear();
          lcd.print("Enter Price:");
          if (add == true) {
            lcd.setCursor(13,0);
            lcd.print("(+)");
          } else {
            lcd.setCursor(13,0);
            lcd.print("(-)");
          }
          lcd.setCursor(0,1);
  
          total = 0;
          
        } else {
          add = false;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Subtraction Mode");
          delay(1000);
          lcd.setCursor(0,0);
          lcd.clear();
          lcd.print("Enter Price:");
          if (add == true) {
            lcd.setCursor(13,0);
            lcd.print("(+)");
          } else {
            lcd.setCursor(13,0);
            lcd.print("(-)");
          }
          lcd.setCursor(0,1);
  
          total = 0;
          
        }
    }
  
    // KEY DIGIT PRESSING
    
    if (isDigit(customKey) && total < 150 && customKey != '0') {
    switch (customKey) {
      case '1':
        total = total + 5;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;
      case '2':
        total = total + 10;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;
      case '3': 
        total = total + 20;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;
      case '4':
        total = total + 30;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;
      case '5':
        total = total + 40;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;
      case '6':
        total = total + 50;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;
      case '7':
        total = total + 1;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;
      case '8':
        total = total + 2;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;
      case '9':
        total = total + 3;
        lcd.setCursor(0,1);
        lcd.print(total);
        break;  
      }
    
      Serial.println(total);
      
    } 
  
    
    if (customKey == '0') {
      
      /*
          50000 - 25
          49975 = new balance
          50000 - 49975 = 25
      */
          
      if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
      }
    
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
      }
    
      Serial.println("Card found.");
      readBlock((block + 1), read_point); //Reading the block
    
      Serial.println("Before increasing the point");
      for (int i = 0; i < 16; i++) {
        Serial.println(read_point[i]);
      }
    
      point[0] = 255;
    
      Serial.print(point[0]);
     
      Serial.print("Increased point");
      for (int i = 0; i < 16; i++) {
        Serial.println(point[i]);
      }
    
      writeBlock(block, point);
    
      readBlock(block, read_point);
    
      Serial.print("read block: ");
      Serial.print(read_point[0]);
    
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Card is now at");
      lcd.setCursor(0,1);
      lcd.print("Snacker level");
      delay(2000);
      
      firstnum = 0;
      col = 0;
      secondnum = 0;
      num = 0;
      total = 0;
      
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    
      lcd.clear();
      lcd.print("Enter Price:");
      lcd.setCursor(0,1);
    
    }
  
  //LCD (+) (-) CHECK
    if (add == true) {
      lcd.setCursor(13,0);
      lcd.print("(+)");
    } else {
      lcd.setCursor(13,0);
      lcd.print("(-)");
    }
    
  
    if (isDigit(customKey) && total >= 150) {
      
      total = 150;
      lcd.setCursor(0,1);
      lcd.print(total);
      
    }
  
    if (total >= 150) {
      
      total = 150;
      lcd.setCursor(0,1);
      lcd.print(total);
      
    }
  
    if (customKey == 'C') {
      
      firstnum = 0;
      col = 0;
      secondnum = 0;
      num = 0;
      total = 0;
      
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.print("Enter Price:");
      lcd.setCursor(0,1);
      
    }
  
  
    if (customKey == '=') {
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Confirm price?");
      lcd.setCursor(0,0);
      lcd.print(total);
      lcd.print(" PHP");  
    }
  
    if (customKey == '*') {
      
       if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
       }
  
       if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
       }
       
       Serial.println("Card found.");
       readBlock(block, read_point);
       
  
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Card Balance: ");
       lcd.setCursor(0,1);
       lcd.print(read_point[0]);
       lcd.print(" PHP");
       
       
       delay(2000);
  
       lcd.setCursor(0,0);
       lcd.clear();
       lcd.print("Enter Price:");
       lcd.setCursor(0,1);
       
       mfrc522.PICC_HaltA();
       mfrc522.PCD_StopCrypto1();
  
       total = 0;
  
    }
  
    // WRITING
    
    if (customKey == 'D') {
       
      if ( ! mfrc522.PICC_IsNewCardPresent()) {
         return;
      }
    
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
      }
    
      Serial.print("OK! I READ A CARD!");
    
      Serial.println("Card found.");
      readBlock(block, read_point); //Reading the block
    
      Serial.println("Before increasing the point");
      for (int i = 0; i < 16; i++) {
        Serial.println(read_point[i]);
      }
    
      point[0] = read_point[0];
    
      
      if (add) {
        
        Serial.println("ADD MODE");
        
        point[0] += total; //adding of value
     
        Serial.println(point[0]);
        
      } else if (!add) {
    
        Serial.println("SUBTRACT MODE");
        
        if (((point[0] - total) > 0)) {
          point[0] = point[0] - total; //subtracting of value
        } else {
          point[0] = point[0];
          isNegative = true;
          Serial.print(point[0]);
        }
        
      }
      
      
      Serial.println("Increased point");
      for (int i = 0; i < 16; i++) {
        Serial.println(point[i]);
      }
    
      writeBlock(block, point);
    
      readBlock(block, read_point);
    
      Serial.print("read block: ");
      Serial.print(read_point[0]);
    
      if (isNegative == false && add) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Success!");
        lcd.setCursor(0,1);
        lcd.print("Bal ");
        lcd.print(point[0] - total);
        lcd.print(" -> ");
        lcd.print(point[0]);
      } else if (!add) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Success!");
        lcd.setCursor(0,1);
        lcd.print("Bal ");
        lcd.print(point[0] + total);
        lcd.print(" -> ");
        lcd.print(read_point[0]);
      } 
      
      if (isNegative == true) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Failed!");
        lcd.setCursor(0,1);
        lcd.print("Not enough bal");
      } 
    
      delay(2000);
    
      /*
       * The PWM signal the tone() function sends interferes with the RFID reader
       * Only remove when you found a way to fix it :)
       * - Iggy
    
      tone(9, NOTE_GS5, 250); 
      delay(250);
      tone(9, NOTE_B5, 250); 
    
      */
    
      firstnum = 0;
      col = 0;
      secondnum = 0;
      num = 0;
      total = 0;
      
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    
      lcd.clear();
      lcd.print("Enter Price:");
      lcd.setCursor(0,1);

      //End of writing 
    }
  
    if (add == true) {
      lcd.setCursor(13,0);
      lcd.print("(+)");
    } else {
      lcd.setCursor(13,0);
      lcd.print("(-)");
    }
    
  }
