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
- 5 green and 5 red LEDs
- Start button
- Resistors, wires, and breadboard/PCB

## Wiring Instructions

### LCD Display
- SDA → Arduino pin 20
- SCL → Arduino pin 21
- VCC → 5V
- GND → GND

### Keypad
- ROW1 → Arduino pin A8
- ROW2 → Arduino pin A9
- ROW3 → Arduino pin A10
- ROW4 → Arduino pin A11
- COL1 → Arduino pin 8
- COL2 → Arduino pin 9
- COL3 → Arduino pin 12
- COL4 → Arduino pin 13

### MP3 Player
- RX → Arduino pin 18
- TX → Arduino pin 19
- VCC → 5V
- GND → GND

### LEDs
#### Green LEDs
- GREEN1 → Arduino pin 50 (with appropriate resistor)
- GREEN2 → Arduino pin 51 (with appropriate resistor)
- GREEN3 → Arduino pin 52 (with appropriate resistor)
- GREEN4 → Arduino pin 53 (with appropriate resistor)
- GREEN5 → Arduino pin A6 (with appropriate resistor)

#### Red LEDs
- RED1 → Arduino pin 28 (with appropriate resistor)
- RED2 → Arduino pin 29 (with appropriate resistor)
- RED3 → Arduino pin 37 (with appropriate resistor)
- RED4 → Arduino pin 40 (with appropriate resistor)
- RED5 → Arduino pin 49 (with appropriate resistor)

### Buttons
- BUTTON1 → Arduino pin 2 (to GND)
- BUTTON2 → Arduino pin 3 (to GND)
- BUTTON3 → Arduino pin 4 (to GND)
- BUTTON4 → Arduino pin 5 (to GND)
- BUTTON5 → Arduino pin 6 (to GND)
- BUTTON6 → Arduino pin 7 (to GND)
- START → Arduino pin 39 (to GND)

### Switches
- SWITCH1 → Arduino pin 22 (to GND)
- SWITCH2 → Arduino pin 23 (to GND)
- SWITCH3 → Arduino pin 24 (to GND)
- SWITCH4 → Arduino pin 25 (to GND)
- SWITCH5 → Arduino pin 26 (to GND)
- SWITCH6 → Arduino pin 27 (to GND)

### Potentiometers
- POT1 → Arduino pin A0
- POT2 → Arduino pin A1
- POT3 → Arduino pin A2
- POT4 → Arduino pin A3
- POT5 → Arduino pin A4
- POT6 → Arduino pin A5

### Jack Connectors
- JACK1 → Arduino pin 30 (to GND)
- JACK2 → Arduino pin 31 (to GND)
- JACK3 → Arduino pin 32 (to GND)
- JACK4 → Arduino pin 33 (to GND)
- JACK5 → Arduino pin 34 (to GND)
- JACK6 → Arduino pin 35 (to GND)
- JACK7 → Arduino pin 36 (to GND)
- JACK8 → Arduino pin 37 (to GND)
- JACK9 → Arduino pin 41 (to GND)
- JACK10 → Arduino pin 42 (to GND)
- JACK11 → Arduino pin 43 (to GND)
- JACK12 → Arduino pin 44 (to GND)
- JACK13 → Arduino pin 45 (to GND)
- JACK14 → Arduino pin 46 (to GND)
- JACK15 → Arduino pin 47 (to GND)
- JACK16 → Arduino pin 48 (to GND)

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
   - `1.mp3`: Start game instructions
   - `2.mp3`: Victory sound
   - `3.mp3`: Defeat sound
   - `4.mp3`: Success sound (for correct sequence)
   - `5.mp3`: Failure sound (for incorrect sequence)

### PC Configuration Tool
1. Install Python 3.x
2. Install required packages:
   ```bash
   pip install pyserial tkinter
   ```
3. Run the configuration tool:
   ```bash
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
   - Set the switches to the correct positions (confirm with `*`)
   - Press the buttons in the correct sequence (confirm with `*` before and after)
   - Set the potentiometers to the correct values (confirm with `*` before and after)
   - Connect the jacks in the correct pairs (confirm with `*` before and after)
   - Enter the correct code on the keypad (confirm with `*` before and after)
4. For each correct solution, a green LED will light up
5. For incorrect attempts, a red LED will flash and time will be deducted
6. Complete all challenges before time runs out to win!

## Troubleshooting

- If the LCD doesn't display anything, check the I2C address and wiring
- If MP3 files don't play, ensure the SD card is formatted correctly and files are named properly
- If the configuration tool can't connect to Arduino, check the COM port and ensure no other program is using it

## License

This project is open source and available under the [MIT License](LICENSE).

## Contributing

Contributions to improve the game are welcome. Please feel free to submit a Pull Request.