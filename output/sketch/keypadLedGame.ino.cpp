#include <Arduino.h>
#line 1 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

// Display setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
int a_active, b_active, c_active, d_active, e_active = 100;
// MP3 Player setup
// SoftwareSerial mySoftwareSerial(10, 11); // Old RX, TX
// Using hardware Serial1 pins 18, 19
DFRobotDFPlayerMini myDFPlayer;

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
// Updated pin assignments for keypad
byte rowPins[ROWS] = {A8, A9, A10, A11}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 9, 12, 13};     // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// Add these global variables at the top of the file
bool sequenceActive = false;
char currentSequenceType = '\0';
// unsigned long sequenceStartTime = 0;
char currentInputSequence[20] = "";
int currentInputIndex = 0;
// LED pins - updated
const int GREEN_LEDS[5] = {50, 51, 52, 53, A6};
const int RED_LEDS[5] = {28, 29, 38, 40, 49};

const int SEQUENCE_DISPLAY_TIMEOUT = 60000; // 60 seconds for sequence attempts
bool sequenceStarted = false;
// Start button pin - updated
const int START_BUTTON = 39;

// Switch pins - updated
const int SWITCH_PINS[6] = {22, 23, 24, 25, 26, 27};

// Button pins - updated
const int BUTTON_PINS[6] = {2, 3, 4, 5, 6, 7};

// Potentiometer pins - updated
const int POT_PINS[6] = {A0, A1, A2, A3, A4, A5};

// Jack connection pins - updated
const int JACK_PINS[16] = {30, 31, 32, 33, 34, 35, 36, 37, 41, 42, 43, 44, 45, 46, 47, 48};

// Game state variables
bool gameStarted = false;
bool gameEnded = false;
unsigned long gameStartTime = 0;
unsigned long countdownDuration = 0;
unsigned long penaltyTime = 0;
unsigned long lastDisplayUpdate = 0;
int completedSequences = 0;

// Game configuration
int switchPositions[6] = {0};      // 0 for OFF, 1 for ON
int buttonSequence[6] = {0};       // Stores the correct button sequence (1-6)
int checkbuttonSequence[6] = {0};  // Stores the correct entred button sequence (1-6)
int potValues[6] = {0};            // Target values for potentiometers (0-1023)
int jackConnections[8][2] = {{0}}; // Pairs of jacks that should be connected
char keypadCode[10] = "";          // Keypad code to be entered
bool buttonPressed[6] = {true, true, true, true, true, true};
bool buttonStates[6] = {true, true, true, true, true, true};
// Sequence completion status
bool switchSequenceCompleted = false;
bool buttonSequenceCompleted = false;
bool potSequenceCompleted = false;
bool jackSequenceCompleted = false;
bool keypadSequenceCompleted = false;

// Add these variables to store custom messages
String gameStartMsg = "Game Started! Good luck!";
String sequenceCorrectMsg = "Well done!";
String sequenceWrongMsg = "Try again!";
char gameVictoryMsg[33] = "Victory! All tasks done!";
String gameDefeatMsg = "Game Over! Time's up!";
unsigned long sequenceStartTime = 0;
const unsigned long SEQUENCE_TIMEOUT = 60000; // 60 seconds in milliseconds

#line 89 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void startGame();
#line 130 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void endGame(bool victory);
#line 165 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void updateCountdownDisplay();
#line 202 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void seqTimer();
#line 243 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void processKeypadInput(char key);
#line 367 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void checkKeypadCode(char *enteredCode);
#line 393 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void checkSwitchSequence();
#line 456 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void checkButtonSequence();
#line 504 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void checkKeypadSequence(char *sequence, int length);
#line 542 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void checkPotSequence();
#line 595 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void checkJackConnections();
#line 697 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void showSuccessMessage(String specificMessage);
#line 716 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void showFailureMessage(String specificMessage);
#line 736 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void flashRedLeds();
#line 750 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void applyPenalty();
#line 757 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void saveGameConfig();
#line 824 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void loadGameConfig();
#line 892 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void processSerialCommand();
#line 1133 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void printConfig();
#line 1195 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void setup();
#line 1303 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void button_active();
#line 1332 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void loop();
#line 89 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
void startGame()
{
    // Reset game state
    gameStarted = true;
    gameEnded = false;
    sequenceActive = false;
    gameStartTime = millis();
    completedSequences = 0;

    // Reset sequence completion status
    switchSequenceCompleted = false;
    buttonSequenceCompleted = false;
    potSequenceCompleted = false;
    jackSequenceCompleted = false;
    keypadSequenceCompleted = false;

    // Turn off all LEDs
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(GREEN_LEDS[i], LOW);
        digitalWrite(RED_LEDS[i], LOW);
    }

    // Display game start message
    lcd.clear();
    lcd.print(gameStartMsg);

    // Try to play start audio
    // if (myDFPlayer.available())
    // {
    //   myDFPlayer.play(1); // Assuming 1 is the start audio file
    // }

    // Wait a moment for the message to be visible
    delay(3000);

    // Update display with countdown
    lastDisplayUpdate = millis() - 1000; // Force immediate update
    updateCountdownDisplay();
}

void endGame(bool victory)
{
    gameEnded = true;

    lcd.clear();
    if (victory)
    {
        lcd.print(gameVictoryMsg);
        lcd.setCursor(0, 1);
        lcd.print("All tasks done!");
        // myDFPlayer.play(2); // Victory audio

        // Turn on all green LEDs
        for (int i = 0; i < 5; i++)
        {
            digitalWrite(GREEN_LEDS[i], HIGH);
        }
    }
    else
    {
        lcd.print(gameDefeatMsg);
        lcd.setCursor(0, 1);
        lcd.print("Time's up!      ");
        // myDFPlayer.play(3); // Defeat audio

        // Turn on all red LEDs
        for (int i = 0; i < 5; i++)
        {
            digitalWrite(RED_LEDS[i], HIGH);
        }
    }
    a_active = b_active = c_active = d_active = e_active = 100;
    // b_active = 100;
}

void updateCountdownDisplay()
{
    // Only update display every second to avoid flicker
    if (millis() - lastDisplayUpdate >= 1000)
    {
        lastDisplayUpdate = millis();
        // Serial.println("elapsed time count");
        unsigned long elapsedTime = millis() - gameStartTime;
        if (elapsedTime < countdownDuration || 1)
        { // Serial.println("elapsed time is lesser");
            unsigned long remainingTime = (countdownDuration - elapsedTime) / 1000;

            // Convert to hours, minutes, seconds
            int hours = remainingTime / 3600;
            int minutes = (remainingTime % 3600) / 60;
            int seconds = remainingTime % 60;

            lcd.clear();
            lcd.print("Time remaining:");
            lcd.setCursor(0, 1);

            // Format as hh:mm:ss
            if (hours < 10)
                lcd.print("0");
            lcd.print(hours);
            lcd.print(":");
            if (minutes < 10)
                lcd.print("0");
            lcd.print(minutes);
            lcd.print(":");
            if (seconds < 10)
                lcd.print("0");
            lcd.print(seconds);
        }
    }
}

void seqTimer()
{
    // If a sequence is active, check for timeout
    if (millis() - sequenceStartTime > SEQUENCE_TIMEOUT)
    {
        sequenceActive = false;
        lcd.clear();
        lcd.print("Time's up!");
        // lcd.setCursor(0, 1);
        // lcd.print(sequenceWrongMsg);

        // Flash red LEDs
        flashRedLeds();

        // Try to play failure sound
        // if (myDFPlayer.available())
        // {
        //   myDFPlayer.play(5);
        // }

        // Apply penalty
        applyPenalty();

        // Force countdown display update
        delay(2000);
        lastDisplayUpdate = millis() - 1000;
        return;
    }

    // Display remaining time (update every second)
    static unsigned long lastTimeDisplay = 0;
    if (millis() - lastTimeDisplay >= 1000)
    {
        lastTimeDisplay = millis();
        int remainingSeconds = (SEQUENCE_TIMEOUT - (millis() - sequenceStartTime)) / 1000;
        lcd.setCursor(14, 0);
        if (remainingSeconds < 10)
            lcd.print("0");
        lcd.print(remainingSeconds);
    }
}
void processKeypadInput(char key)
{
    Serial.print("Key pressed: ");
    Serial.println(key);
    // If no sequence is active, check if a sequence selection key was pressed
    if (!sequenceActive)
    {
        Serial.print("Key pressed seq not active: ");
        Serial.println(key);
        Serial.print("state: ");
        Serial.println(b_active);
        if ((key == 'A' && a_active == 100) || (key == 'B' && b_active == 100) || (key == 'C' && c_active == 100) || (key == 'D' && d_active == 100) || (key == '*' && e_active == 100))
        {
            sequenceActive = true;
            currentSequenceType = key;
            sequenceStartTime = millis();
            currentInputIndex = 0;
            currentInputSequence[0] = '\0';
            Serial.println(a_active);
            lcd.clear();
            switch (key)
            {
            case 'A':
                lcd.print("Switch seq.");
                lcd.setCursor(0, 1);
                lcd.print("Set switches...");
                // a = false;
                break;
            case 'B':
                lcd.print("Button seq.");
                lcd.setCursor(0, 1);
                lcd.print("Press buttons...");
                for (int i = 0; i < 6; i++)
                    checkbuttonSequence[i] = 0;
                // b = false;
                break;
            case 'C':
                lcd.print("Pots.");
                lcd.setCursor(0, 1);
                lcd.print("Adjust pots...");
                // c = false;
                break;
            case 'D':
                lcd.print("Jack conn.");
                lcd.setCursor(0, 1);
                lcd.print("Connect jacks...");
                // d = false;
                break;
            case '*':
                lcd.print("Keypad code");
                lcd.setCursor(0, 1);
                lcd.print("Enter code: ");
                // e = false;
                break;
            }

            // // Try to play a sound to indicate sequence started
            // if (myDFPlayer.available())
            // {
            //   myDFPlayer.play(6); // Assuming 6 is a "sequence start" sound
            // }
        }
        return;
    }

    // Check if # is pressed to complete the sequence
    if (key == '#')
    {
        sequenceActive = false;

        // Evaluate the sequence based on what's being tested
        switch (currentSequenceType)
        {
        case 'A':
            if (a_active == 100)
                checkSwitchSequence();
            break;
        case 'B':
            if (b_active == 100)
                checkButtonSequence();
            break;
        case 'C':
            if (c_active == 100)
                checkPotSequence();
            break;
        case 'D':
            if (d_active == 100)
                checkJackConnections();
            break;
        case '*':
            if (e_active == 100)
            {
                // For keypad code, evaluate the entered sequence
                currentInputSequence[currentInputIndex] = '\0';
                checkKeypadCode(currentInputSequence);
            }
            break;
        }

        // Force countdown display update after showing success/failure message
        delay(2000);
        lastDisplayUpdate = millis() - 1000;
        Serial.print("Key type pressed seq active v2: ");
        Serial.println(currentSequenceType);
        return;
    }

    // For keypad code entry, record the key presses
    if (currentSequenceType == '*' && key != '#' && key != '*')
    {
        if (currentInputIndex < 19)
        { // Leave room for null terminator
            currentInputSequence[currentInputIndex++] = key;
            currentInputSequence[currentInputIndex] = '\0';

            // Show the entered sequence on the LCD
            lcd.setCursor(0, 1);
            lcd.print("Enter code: ");
            lcd.print(currentInputSequence);
        }
    }
}

// New function to check keypad code
void checkKeypadCode(char *enteredCode)
{
    // Check against the correct code
    if (strcmp(enteredCode, keypadCode) == 0)
    {
        keypadSequenceCompleted = true;
        completedSequences++;
        showSuccessMessage("Keypad code correct!");

        // Light up a green LED based on completed sequences
        if (completedSequences <= 5)
        {
            digitalWrite(GREEN_LEDS[completedSequences - 1], HIGH);
        }
        e_active = 0;
        // myDFPlayer.play(4); // Success audio
    }
    else
    {
        showFailureMessage("Keypad code wrong!");
        flashRedLeds();
        // myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
}

void checkSwitchSequence()
{
    bool correct = true;

    // Check if all switches are in the correct position
    for (int i = 0; i < 6; i++)
    {
        int switchState = digitalRead(SWITCH_PINS[i]) == LOW ? 1 : 0;
        Serial.print("Switch ");
        Serial.print(i);
        Serial.print(" state: ");
        Serial.println(switchState);
        if (switchState != switchPositions[i])
        {
            correct = false;
            break;
        }
    }

    lcd.clear();
    if (correct)
    {
        switchSequenceCompleted = true;
        completedSequences++;

        // Light up a green LED based on completed sequences
        if (completedSequences <= 5)
        {
            digitalWrite(GREEN_LEDS[completedSequences - 1], HIGH);
            // Debug output
            Serial.print("Turning on GREEN LED ");
            Serial.println(completedSequences - 1);
        }

        lcd.print("Switches correct!");
        lcd.setCursor(0, 1);
        // lcd.print(sequenceCorrectMsg);

        // Try to play success sound
        // if (myDFPlayer.available())
        // {
        //   myDFPlayer.play(4);
        // }
        a_active = 0;
    }
    else
    {
        lcd.print("Switches wrong!");
        lcd.setCursor(0, 1);
        // lcd.print(sequenceWrongMsg);

        flashRedLeds();

        // Try to play failure sound
        // if (myDFPlayer.available())
        // {
        //   myDFPlayer.play(5);
        // }

        applyPenalty();
    }
}

void checkButtonSequence()
{
    // Implementation that doesn't require parameters
    // This would need to directly check the button states
    bool correct = true;
    // Check if buttons match the expected sequence
    for (int i = 0; i < 6; i++)
    {
       Serial.print(" saved button seq.");
       Serial.println(buttonSequence[i]);
    }

    // Check if buttons match the expected sequence
    for (int i = 0; i < 6; i++)
    {
        // int buttonState = digitalRead(BUTTON_PINS[i]) == LOW ? 1 : 0;
        if (checkbuttonSequence[i] != (buttonSequence[i]))
        {
            correct = false;
            break;
        }
    }

    if (correct)
    {
        buttonSequenceCompleted = true;
        completedSequences++;
        showSuccessMessage("Button seq correct!");

        // Light up a green LED based on completed sequences
        if (completedSequences <= 5)
        {
            digitalWrite(GREEN_LEDS[completedSequences - 1], HIGH);
        }

        // myDFPlayer.play(4); // Success audio
        Serial.println("button right");
        b_active = 0;
    }
    else
    {
        showFailureMessage("Button seq wrong!");
        flashRedLeds();
        // myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
}

void checkKeypadSequence(char *sequence, int length)
{
    // Extract the keypad code from the input (remove the A markers)
    char codeInput[10] = "";
    int codeInputIndex = 0;

    for (int i = 1; i < length - 1; i++)
    {
        codeInput[codeInputIndex++] = sequence[i];
    }
    codeInput[codeInputIndex] = '\0';

    // Check against the correct code
    if (strcmp(codeInput, keypadCode) == 0)
    {
        keypadSequenceCompleted = true;
        completedSequences++;
        showSuccessMessage("Keypad code correct!");

        // Light up a green LED based on completed sequences
        if (completedSequences <= 5)
        {
            digitalWrite(GREEN_LEDS[completedSequences - 1], HIGH);
        }

        // myDFPlayer.play(4); // Success audio
        Serial.println("key right ");
        e_active = 0;
    }
    else
    {
        showFailureMessage("Keypad code wrong!");
        flashRedLeds();
        // myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
}

void checkPotSequence()
{
    bool correct = true;
    Serial.print("Checking pot sequence...");

    // Check if all potentiometers are at the correct values (with some tolerance)
    const int TOLERANCE = 5; // Tolerance for potentiometer readings

    for (int i = 0; i < 6; i++)
    {
        int potReading = (analogRead(POT_PINS[i]) / 10);
        if (abs(potReading - potValues[i]) > TOLERANCE)
        {
            correct = false;
            break;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        int potReading = analogRead(POT_PINS[i]) / 10;
        Serial.print("pot reading : ");
        Serial.print(potReading);
        Serial.print(" || saved reading :");
        Serial.println(potValues[i]);
    }

    Serial.println("Potentiometer check complete. analysing");
    if (correct)
    {
        potSequenceCompleted = true;
        completedSequences++;
        showSuccessMessage("Pots. OK!");

        // Light up a green LED based on completed sequences
        if (completedSequences <= 5)
        {
            digitalWrite(GREEN_LEDS[completedSequences - 1], HIGH);
        }

        // myDFPlayer.play(4); // Success
        c_active = 0;
        Serial.println("pot right seq");
    }
    else
    {
        showFailureMessage("Pots. wrong!");
        flashRedLeds();
        // myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
    Serial.println("Potentiometer check complete.");
}

void checkJackConnections()
{
    bool correct = true;
    Serial.print("Checking jack connections...");
    Serial.println();
    // Create a matrix to track which jacks are connected
    bool jackMatrix[16][16] = {{false}};

    // First, detect all connections
    for (int i = 0; i < 16; i++)
    {
        // Set current pin as OUTPUT LOW temporarily
        pinMode(JACK_PINS[i], OUTPUT);
        digitalWrite(JACK_PINS[i], LOW);

        // Check which other pins detect the LOW signal
        for (int j = 0; j < 16; j++)
        {
            if (i != j)
            {
                if (digitalRead(JACK_PINS[j]) == LOW)
                {
                    jackMatrix[i][j] = true;
                }
            }
        }

        // Reset pin back to INPUT_PULLUP
        pinMode(JACK_PINS[i], INPUT_PULLUP);
    }

    // Check if the detected connections match the required connections
    for (int i = 0; i < 8; i++)
    {
        int jack1 = jackConnections[i][0];
        int jack2 = jackConnections[i][1];

        if (jack1 >= 0 && jack2 >= 0)
        { // Valid connection pair
            if (!jackMatrix[jack1][jack2] || !jackMatrix[jack2][jack1])
            {
                correct = false;
                break;
            }
        }
    }

    // Also check that there are no extra connections
    for (int i = 0; i < 16; i++)
    {
        for (int j = i + 1; j < 16; j++)
        {
            // If this connection exists but isn't in our required list
            if (jackMatrix[i][j] || jackMatrix[j][i])
            {
                bool isRequiredConnection = false;
                for (int k = 0; k < 8; k++)
                {
                    if ((jackConnections[k][0] == i && jackConnections[k][1] == j) ||
                        (jackConnections[k][0] == j && jackConnections[k][1] == i))
                    {
                        isRequiredConnection = true;
                        break;
                    }
                }
                if (!isRequiredConnection)
                {
                    correct = false;
                    break;
                }
            }
        }
        if (!correct)
            break;
    }

    if (correct)
    {
        jackSequenceCompleted = true;
        completedSequences++;
        showSuccessMessage("Jack conn. OK!");

        // Light up a green LED based on completed sequences
        if (completedSequences <= 5)
        {
            digitalWrite(GREEN_LEDS[completedSequences - 1], HIGH);
        }

        // myDFPlayer.play(4); // Success audio
        Serial.println("Jack connections OK!");
        d_active = 0;
    }
    else
    {
        showFailureMessage("Jack seq. wrong!");
        Serial.print("jack check failed");
        flashRedLeds();
        // myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
}

void showSuccessMessage(String specificMessage)
{
    lcd.clear();
    lcd.print(specificMessage);
    // lcd.setCursor(0, 1);
    // lcd.print(sequenceCorrectMsg);

    // Play success audio
    // myDFPlayer.play(4);

    // Wait for audio to start

    // Keep message visible for a few seconds
    delay(2000);

    // Force update in next cycle to show countdown
    lastDisplayUpdate = millis() - 1000;
}

void showFailureMessage(String specificMessage)
{
    lcd.clear();
    lcd.print(specificMessage);
    // lcd.setCursor(0, 1);
    // lcd.print(sequenceWrongMsg);

    // Play failure audio
    // myDFPlayer.play(5);

    // Wait for audio to start
    delay(500);

    // Keep message visible for a few seconds
    delay(2000);

    // Force update in next cycle to show countdown
    lastDisplayUpdate = millis() - 1000;
}

void flashRedLeds()
{
    // Flash all red LEDs
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(RED_LEDS[i], HIGH);
    }
    delay(500);
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(RED_LEDS[i], LOW);
    }
}

void applyPenalty()
{
    // Add penalty time to the game clock
    countdownDuration -= penaltyTime;
}

// EEPROM functions for storing/loading game configuration
void saveGameConfig()
{
    int addr = 0;

    // Save countdown duration (4 bytes)
    EEPROM.put(addr, countdownDuration);
    addr += sizeof(countdownDuration);

    // Save penalty time (4 bytes)
    EEPROM.put(addr, penaltyTime);
    addr += sizeof(penaltyTime);

    // Save switch positions (6 bytes)
    for (int i = 0; i < 6; i++)
    {
        EEPROM.write(addr++, switchPositions[i]);
    }

    // Save button sequence (6 bytes)
    for (int i = 0; i < 6; i++)
    {
        EEPROM.write(addr++, buttonSequence[i]);
    }

    // Save potentiometer values (12 bytes - 2 bytes per value)
    for (int i = 0; i < 6; i++)
    {
        EEPROM.put(addr, potValues[i]);
        addr += sizeof(int);
    }

    // Save jack connections (16 bytes - 2 bytes per connection)
    for (int i = 0; i < 8; i++)
    {
        EEPROM.write(addr++, jackConnections[i][0]);
        EEPROM.write(addr++, jackConnections[i][1]);
    }

    // Save keypad code (10 bytes)
    for (int i = 0; i < 10; i++)
    {
        EEPROM.write(addr++, keypadCode[i]);
    }

    // // Save custom messages
    // for (int i = 0; i < 33; i++)
    // {
    //     EEPROM.write(addr++, gameStartMsg[i]);
    // }
    // for (int i = 0; i < 33; i++)
    // {
    //     EEPROM.write(addr++, sequenceCorrectMsg[i]);
    // }
    // for (int i = 0; i < 33; i++)
    // {
    //     EEPROM.write(addr++, sequenceWrongMsg[i]);
    // }
    // for (int i = 0; i < 33; i++)
    // {
    //     EEPROM.write(addr++, gameVictoryMsg[i]);
    // }
    // for (int i = 0; i < 33; i++)
    // {
    //     EEPROM.write(addr++, gameDefeatMsg[i]);
    // }
}

void loadGameConfig()
{
    int addr = 0;

    // Load countdown duration
    EEPROM.get(addr, countdownDuration);
    addr += sizeof(countdownDuration);

    // Load penalty time
    EEPROM.get(addr, penaltyTime);
    addr += sizeof(penaltyTime);

    // Load switch positions
    for (int i = 0; i < 6; i++)
    {
        switchPositions[i] = EEPROM.read(addr++);
    }

    // Load button sequence
    for (int i = 0; i < 6; i++)
    {
        buttonSequence[i] = EEPROM.read(addr++);
    }

    // Load potentiometer values
    for (int i = 0; i < 6; i++)
    {
        EEPROM.get(addr, potValues[i]);
        addr += sizeof(int);
    }

    // Load jack connections
    for (int i = 0; i < 8; i++)
    {
        jackConnections[i][0] = EEPROM.read(addr++);
        jackConnections[i][1] = EEPROM.read(addr++);
    }

    // Load keypad code
    for (int i = 0; i < 10; i++)
    {
        keypadCode[i] = EEPROM.read(addr++);
    }

    // Load custom messages
    for (int i = 0; i < 33; i++)
    {
        gameStartMsg[i] = EEPROM.read(addr++);
    }
    for (int i = 0; i < 33; i++)
    {
        sequenceCorrectMsg[i] = EEPROM.read(addr++);
    }
    for (int i = 0; i < 33; i++)
    {
        sequenceWrongMsg[i] = EEPROM.read(addr++);
    }
    for (int i = 0; i < 33; i++)
    {
        gameVictoryMsg[i] = EEPROM.read(addr++);
    }
    for (int i = 0; i < 33; i++)
    {
        // gameDefeatMsg[i] = EEPROM.read(addr++);
    }
}

// Process commands received from the PC
void processSerialCommand()
{
    String command = Serial.readStringUntil('\n');
    delay(500);

    if (command.startsWith("SET:"))
    {
        // Parse the command
        command = command.substring(4); // Remove "SET:"

        if (command.startsWith("COUNTDOWN:"))
        {
            countdownDuration = command.substring(10).toInt() * 1000; // Convert to milliseconds
            Serial.println("OK:COUNTDOWN");
        }
        else if (command.startsWith("PENALTY:"))
        {
            penaltyTime = command.substring(8).toInt() * 1000; // Convert to milliseconds
            Serial.println("OK:PENALTY");
        }
        else if (command.startsWith("SWITCHES:"))
        {
            String values = command.substring(9);
            for (int i = 0; i < 6; i++)
            {
                if (i < values.length())
                {
                    switchPositions[i] = values.charAt(i) == '1' ? 1 : 0;
                }
            }
            Serial.println("OK:SWITCHES");
        }
        else if (command.startsWith("BUTTONS:"))
        {
            String values = command.substring(8);
            for (int i = 0; i < 6; i++)
            {
                if (i < values.length())
                {
                    buttonSequence[i] = values.charAt(i) - '0';
                }
            }
            Serial.println("OK:BUTTONS");
        }
        else if (command.startsWith("POTS:"))
        {
            String values = command.substring(5);
            int index = 0;
            int valueStart = 0;

            for (int i = 0; i < values.length(); i++)
            {
                if (values.charAt(i) == ',' || i == values.length() - 1)
                {
                    String valueStr;
                    if (i == values.length() - 1)
                    {
                        valueStr = values.substring(valueStart);
                    }
                    else
                    {
                        valueStr = values.substring(valueStart, i);
                    }

                    if (index < 6)
                    {
                        potValues[index] = valueStr.toInt();
                    }

                    valueStart = i + 1;
                    index++;
                }
            }
            Serial.println("OK:POTS");
        }
        else if (command.startsWith("JACKS:"))
        {
            String values = command.substring(6);
            int pairIndex = 0;
            int valueStart = 0;
            int jackIndex = 0;

            for (int i = 0; i < values.length(); i++)
            {
                if (values.charAt(i) == ',' || i == values.length() - 1)
                {
                    String valueStr;
                    if (i == values.length() - 1)
                    {
                        valueStr = values.substring(valueStart);
                    }
                    else
                    {
                        valueStr = values.substring(valueStart, i);
                    }

                    if (pairIndex < 8 && jackIndex < 2)
                    {
                        jackConnections[pairIndex][jackIndex] = valueStr.toInt();
                        jackIndex++;
                        if (jackIndex == 2)
                        {
                            jackIndex = 0;
                            pairIndex++;
                        }
                    }

                    valueStart = i + 1;
                }
            }
            Serial.println("OK:JACKS");
        }
        else if (command.startsWith("KEYCODE:"))
        {
            String code = command.substring(8);
            code.toCharArray(keypadCode, 10);
            Serial.println("OK:KEYCODE");
        }
        else if (command == "SAVE")
        {
            saveGameConfig();
            Serial.println("OK:SAVE");
        }
        else if (command.startsWith("MSG:"))
        {
            command = command.substring(4); // Remove "MSG:"

            if (command.startsWith("START:"))
            {
                String msg = command.substring(6);
                // msg.toCharArray(gameStartMsg, 33);
                Serial.println("OK:MSG:START");
            }
            else if (command.startsWith("CORRECT:"))
            {
                String msg = command.substring(8);
                // msg.toCharArray(sequenceCorrectMsg, 33);
                Serial.println("OK:MSG:CORRECT");
            }
            else if (command.startsWith("WRONG:"))
            {
                String msg = command.substring(6);
                // msg.toCharArray(sequenceWrongMsg, 33);
                Serial.println("OK:MSG:WRONG");
            }
            else if (command.startsWith("VICTORY:"))
            {
                String msg = command.substring(8);
                // msg.toCharArray(gameVictoryMsg, 33);
                Serial.println("OK:MSG:VICTORY");
            }
            else if (command.startsWith("DEFEAT:"))
            {
                String msg = command.substring(7);
                // msg.toCharArray(gameDefeatMsg, 33);
                Serial.println("OK:MSG:DEFEAT");
            }
        }
    }
    else if (command == "GET:CONFIG")
    {
        // Send the current configuration back to the PC
        Serial.print("CONFIG:COUNTDOWN:");
        Serial.println(countdownDuration / 1000);

        Serial.print("CONFIG:PENALTY:");
        Serial.println(penaltyTime / 1000);

        Serial.print("CONFIG:SWITCHES:");
        for (int i = 0; i < 6; i++)
        {
            Serial.print(switchPositions[i]);
        }
        Serial.println();

        Serial.print("CONFIG:BUTTONS:");
        for (int i = 0; i < 6; i++)
        {
            Serial.print(buttonSequence[i]);
        }
        Serial.println();

        Serial.print("CONFIG:POTS:");
        for (int i = 0; i < 6; i++)
        {
            Serial.print(potValues[i]);
            if (i < 5)
                Serial.print(",");
        }
        Serial.println();

        Serial.print("CONFIG:JACKS:");
        for (int i = 0; i < 8; i++)
        {
            Serial.print(jackConnections[i][0]);
            Serial.print(",");
            Serial.print(jackConnections[i][1]);
            if (i < 7)
                Serial.print(",");
        }
        Serial.println();

        Serial.print("CONFIG:KEYCODE:");
        Serial.println(keypadCode);

        Serial.print("CONFIG:MSG:START:");
        Serial.println(gameStartMsg);

        Serial.print("CONFIG:MSG:CORRECT:");
        Serial.println(sequenceCorrectMsg);

        Serial.print("CONFIG:MSG:WRONG:");
        Serial.println(sequenceWrongMsg);

        Serial.print("CONFIG:MSG:VICTORY:");
        Serial.println(gameVictoryMsg);

        Serial.print("CONFIG:MSG:DEFEAT:");
        Serial.println(gameDefeatMsg);
    }
    else if (command == "RESET")
    {
        // Reset the game
        gameStarted = false;
        gameEnded = false;

        // Turn off all LEDs
        for (int i = 0; i < 5; i++)
        {
            digitalWrite(GREEN_LEDS[i], LOW);
            digitalWrite(RED_LEDS[i], LOW);
        }

        lcd.clear();
        lcd.print("Keypad LED Game");
        lcd.setCursor(0, 1);
        lcd.print("Press START");
        Serial.println("OK:RESET");
    }
}

void printConfig()
{
    Serial.println("---saved  Configuration ---");
    delay(50);

    Serial.print("Countdown: ");
    Serial.println(countdownDuration / 1000);
    delay(50);

    Serial.print("Penalty: ");
    Serial.println(penaltyTime / 1000);
    delay(50);

    Serial.print("Switches: ");
    for (int i = 0; i < 6; i++)
    {
        Serial.print(switchPositions[i]);
    }
    Serial.println();
    delay(50);
    Serial.print("Buttons: ");
    for (int i = 0; i < 6; i++)
    {
        Serial.print(buttonSequence[i]);
    }
    Serial.println();
    delay(50);
    Serial.print("Pots: ");
    for (int i = 0; i < 6; i++)
    {
        Serial.print(potValues[i]);
        if (i < 5)
        {
            Serial.print(", ");
        }
    }
    Serial.println();
    delay(50);
    Serial.print("Jacks: ");
    for (int i = 0; i < 8; i++)
    {
        Serial.print(jackConnections[i][0]);
        Serial.print(", ");
        Serial.print(jackConnections[i][1]);
        if (i < 7 || i % 2 == 1)
        {
            Serial.print(", ");
        }
        Serial.println();
    }
    delay(50);
    Serial.print("Keypad Code: ");
    Serial.println(keypadCode);
    Serial.print("Start Message: ");
    Serial.println(gameStartMsg);
    Serial.print("Correct Message: ");
    Serial.println(sequenceCorrectMsg);
    Serial.print("Wrong Message: ");
    Serial.println(sequenceWrongMsg);
    Serial.println("now connect the GUI  configurator");
}
// Function prototypes
void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);

    // Initialize display
    Wire.begin();
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.print("Keypad LED Game");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    delay(1000);

    // Initialize MP3 player with better error handling
    int retryCount = 0;
    bool dfPlayerInitialized = false;

    while (!dfPlayerInitialized && retryCount < 3)
    {
        if (myDFPlayer.begin(Serial1))
        {
            dfPlayerInitialized = true;
            myDFPlayer.volume(25); // Set volume (0-30)

            // Test if SD card is present by trying to get number of files
            if (myDFPlayer.readState() != 0)
            {
                lcd.clear();
                lcd.print("SD Card Error!");
                lcd.setCursor(0, 1);
                lcd.print("Check SD Card");
                delay(2000);
            }
        }
        else
        {
            retryCount++;
            delay(500);
        }
    }

    if (!dfPlayerInitialized)
    {
        lcd.clear();
        lcd.print("MP3 Player Error");
        lcd.setCursor(0, 1);
        lcd.print("Game will continue");
        delay(2000);
    }

    // Initialize LEDs with explicit testing
    lcd.clear();
    lcd.print("Testing LEDs...");

    // Test all LEDs
    for (int i = 0; i < 5; i++)
    {
        pinMode(GREEN_LEDS[i], OUTPUT);
        pinMode(RED_LEDS[i], OUTPUT);

        // Turn on and off each LED to test
        digitalWrite(GREEN_LEDS[i], HIGH);
        delay(200);
        digitalWrite(GREEN_LEDS[i], LOW);

        digitalWrite(RED_LEDS[i], HIGH);
        delay(200);
        digitalWrite(RED_LEDS[i], LOW);
    }

    // Initialize start button
    pinMode(START_BUTTON, INPUT_PULLUP);

    // Initialize switches
    for (int i = 0; i < 6; i++)
    {
        pinMode(SWITCH_PINS[i], INPUT_PULLUP);
    }

    // Initialize buttons
    for (int i = 0; i < 6; i++)
    {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    }

    // Jack pins as inputs with pullups
    for (int i = 0; i < 16; i++)
    {
        pinMode(JACK_PINS[i], INPUT_PULLUP);
    }

    // Load game configuration from EEPROM and print it to serial monitor
    loadGameConfig();
    printConfig();

    lcd.clear();
    lcd.print("Keypad LED Game");
    lcd.setCursor(0, 1);
    lcd.print("Press START");
    // a_active, b, c, d, e = true;
    a_active = 100;
    b_active = 100;
    c_active = 100;
    d_active = 100;
    e_active = 100;
}
void button_active()
{
    for (int i = 0; i < 6; i++)
    {
        // Read button state and store in array
        buttonStates[i] = digitalRead(BUTTON_PINS[i]);
        // buttonPressed[i] = buttonState;
        if (!buttonStates[i]) // button is pressed
        {
            if (buttonPressed[i])
            {
                delay(200);
                checkbuttonSequence[i] = !buttonStates[i];
                // Example debug output
                Serial.print("Button  ");
                Serial.print(i + 1);
                Serial.print(" pressed : ");
                Serial.println(checkbuttonSequence[i]);
                buttonPressed[i] = false;
                break;
            }
        }
        else
        {
            buttonPressed[i] = true;
        }
    }
}

void loop()
{
    // Check for configuration commands from PC
    if (Serial.available() > 0)
    {
        processSerialCommand();
    }
    if (currentSequenceType == 'B')
        button_active();
    // Check if start button is pressed
    if (!gameStarted && !gameEnded && digitalRead(START_BUTTON) == LOW)
    {
        startGame();
        delay(300); // Debounce
    }

    // Game logic only runs if the game has started and not ended
    if (gameStarted && !gameEnded)
    {
        // Serial.println("Update countdown display if no sequence is active");
        if (!sequenceActive)
        {
            updateCountdownDisplay();
            // Serial.println("no sequence is active");
        }
        else
        {
            seqTimer();
            // Serial.println("Update countdown display ");
        }

        // Check if time is up
        if (millis() - gameStartTime >= countdownDuration)
        {
            endGame(false);
        }

        // Process keypad input
        char key = keypad.getKey();
        if (key)
        {
            processKeypadInput(key);
        }

        // Check if all sequences are completed
        if (switchSequenceCompleted &&
            buttonSequenceCompleted &&
            potSequenceCompleted &&
            jackSequenceCompleted &&
            keypadSequenceCompleted)
        {
            endGame(true);
        }
    }
}
