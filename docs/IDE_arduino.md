# Downloading
https://www.arduino.cc/en/software/

I have downloaded the the IDE rev 1.8.19

# Configuration
You can test your installation by uploading the blink exemple to verify the driver installation

## libraries
### LCD
https://github.com/hasenradball/LCD-I2C
Par défaut, l'adresse I2C de l'afficheur est 0x27
https://github.com/johnrickman/LiquidCrystal_I2C

### Liaison série
SoftwareSerial

### Player MP3
https://github.com/DFRobot/DFRobotDFPlayerMini
---

## Câblage DFPlayer Mini avec Arduino Nano :

| DFPlayer Mini | Arduino Nano |
| ------------- | ------------ |
| VCC           | 5V           |
| GND           | GND          |
| TX            | D10          |
| RX            | D11          |

### RTC
https://github.com/adafruit/RTClib
https://github.com/adafruit/Adafruit_BusIO
Par défaut, l'adresse I2C de l'afficheur est 0x68

# Divers
### 🔹 Pour une carte **Arduino UNO / Nano / Mega** (AVR core) :

#### ➤ Le fichier `Arduino.h` est ici :

- **Windows :**  
	    `C:\Users\VotreNom\AppData\Local\Arduino15\packages\arduino\hardware\avr\<version>\cores\arduino\Arduino.h`

