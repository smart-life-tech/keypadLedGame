#line 1 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\README.md"
# Keypad LED Game

This project is an interactive puzzle game using Arduino. Players must solve a series of sequences and codes within a time limit.

## Hardware Requirements

- Arduino Mega (recommended due to the number of I/O pins needed)
- 16x2 LCD Display with I2C interface
- 4x4 Matrix Keypad
- DFPlayer Mini MP3 player module
- SD card for MP3 files
- 6 toggle switches
- 6 push buttons
- 6 potentiometers
- 16 jack connectors
- Green and red LEDs
- Start button
- Resistors, wires, and breadboard/PCB

## Wiring Instructions

### LCD Display
- SDA -> Arduino SDA pin
- SCL -> Arduino SCL pin
- VCC -> 5V
- GND -> GND

### Keypad
- ROW1 -> Arduino pin 9
- ROW2 -> Arduino pin 8
- ROW3 -> Arduino pin 7
- ROW4 -> Arduino pin 6
- COL1 -> Arduino pin 5
- COL2 -> Arduino pin 4
- COL3 -> Arduino pin 3
- COL4 -> Arduino pin 2

### MP3 Player
- RX -> Arduino pin 10
- TX -> Arduino pin 11
- VCC -> 5V
- GND -> GND

### LEDs
- Green LED -> Arduino pin 12 (with appropriate resistor)
- Red LED -> Arduino pin 13 (with appropriate resistor)

### Switches, Buttons, Potentiometers, and Jacks
- Connect as specified in the Arduino code

## Software Setup

### Arduino Setup
1. Install the required libraries:
   - EEPROM (built-in)
   - Wire (built-in)
   - LiquidCrystal_I2C
   - Keypad
   - DFRobotDFPlayerMini
   - SoftwareSerial (built-in)

2. Upload the `keypadLedGame.ino` sketch to your Arduino

### MP3 Files
1. Format an SD card as FAT32
2. Add the following MP3 files to the root directory:
   - 1.mp3: Start game instructions
   - 2.mp3: Victory sound
   - 3.mp3: Defeat sound
   - 4.mp3: Success sound (for correct sequence)
   - 5.mp3: Failure sound (for incorrect sequence)

### PC Configuration Tool
1. Install Python 3.x
2. Install required packages:
   ```
   pip install pyserial tkinter
   ```
3. Run the configuration tool:
   ```
   python keypad_game_configurator.py
   ```

## Game Configuration

Use the PC configuration tool to set:
- Countdown timer duration
- Penalty time for incorrect attempts
- Correct switch positions
- Button sequence
- Potentiometer target values
- Jack connection pairs
- Keypad code

After configuring, send the settings to the Arduino. You can then disconnect the USB cable and the game will run standalone.

## How to Play

1. Press the start button to begin the game
2. Listen to the instructions from the MP3 player
3. Solve the following challenges within the time limit:
   - Set the switches to the correct positions (confirm with *)
   - Press the buttons in the correct sequence (confirm with # before and after)
   - Set the potentiometers to the correct values (confirm with B before and after)
   - Connect the jacks in the correct pairs (confirm with C before and after)
   - Enter the correct code on the keypad (confirm with A before and after)
4. For each correct solution, a green LED will light up
5. For incorrect attempts, a red LED will flash and time will be deducted
6. Complete all challenges before time runs out to win!

## Troubleshooting

- If the LCD doesn't display anything, check the I2C address and wiring
- If MP3 files don't play, ensure the SD card is formatted correctly and files are named properly
- If the configuration tool can't connect to Arduino, check the COM port and ensure no other program is using it
