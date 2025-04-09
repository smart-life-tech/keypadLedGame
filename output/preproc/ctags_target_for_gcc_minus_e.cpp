# 1 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino"
# 2 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino" 2
# 3 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino" 2
# 4 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino" 2
# 5 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino" 2
# 6 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino" 2
# 7 "c:\\Users\\USER\\Documents\\Arduino\\keypadLedGame\\keypadLedGame.ino" 2

// Display setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

// MP3 Player setup
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(((char*)keys), rowPins, colPins, ROWS, COLS);

// LED pins
const int GREEN_LED = 12;
const int RED_LED = 13;

// Start button pin
const int START_BUTTON = A0;

// Switch pins
const int SWITCH_PINS[6] = {22, 24, 26, 28, 30, 32};

// Button pins (separate from keypad)
const int BUTTON_PINS[6] = {34, 36, 38, 40, 42, 44};

// Potentiometer pins
const int POT_PINS[6] = {A1, A2, A3, A4, A5, A6};

// Jack connection pins (using digital pins to detect connections)
const int JACK_PINS[16] = {46, 47, 48, 49, 50, 51, 52, 53, A7, A8, A9, A10, A11, A12, A13, A14};

// Game state variables
bool gameStarted = false;
bool gameEnded = false;
unsigned long gameStartTime = 0;
unsigned long countdownDuration = 0;
unsigned long penaltyTime = 0;
unsigned long lastDisplayUpdate = 0;
int completedSequences = 0;

// Game configuration
int switchPositions[6] = {0}; // 0 for OFF, 1 for ON
int buttonSequence[6] = {0}; // Stores the correct button sequence (1-6)
int potValues[6] = {0}; // Target values for potentiometers (0-1023)
int jackConnections[8][2] = {{0}}; // Pairs of jacks that should be connected
char keypadCode[10] = ""; // Keypad code to be entered

// Sequence completion status
bool switchSequenceCompleted = false;
bool buttonSequenceCompleted = false;
bool potSequenceCompleted = false;
bool jackSequenceCompleted = false;
bool keypadSequenceCompleted = false;

void setup()
{
    Serial.begin(9600);

    // Initialize display
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.print("Keypad LED Game");
    lcd.setCursor(0, 1);
    lcd.print("Press START");

    // Initialize MP3 player
    mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial))
    {
        lcd.clear();
        lcd.print("MP3 Error!");
        while (true)
            ;
    }
    myDFPlayer.volume(25); // Set volume (0-30)

    // Initialize LEDs
    pinMode(GREEN_LED, 0x1);
    pinMode(RED_LED, 0x1);
    digitalWrite(GREEN_LED, 0x0);
    digitalWrite(RED_LED, 0x0);

    // Initialize start button
    pinMode(START_BUTTON, 0x2);

    // Initialize switches
    for (int i = 0; i < 6; i++)
    {
        pinMode(SWITCH_PINS[i], 0x2);
    }

    // Initialize buttons
    for (int i = 0; i < 6; i++)
    {
        pinMode(BUTTON_PINS[i], 0x2);
    }

    // Jack pins as inputs with pullups
    for (int i = 0; i < 16; i++)
    {
        pinMode(JACK_PINS[i], 0x2);
    }

    // Load game configuration from EEPROM
    loadGameConfig();
}

void loop()
{
    // Check for configuration commands from PC
    if (Serial.available() > 0)
    {
        processSerialCommand();
    }

    // Check if start button is pressed
    if (!gameStarted && !gameEnded && digitalRead(START_BUTTON) == 0x0)
    {
        startGame();
    }

    // Game logic only runs if the game has started and not ended
    if (gameStarted && !gameEnded)
    {
        // Update countdown display
        updateCountdownDisplay();

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

void startGame()
{
    gameStarted = true;
    gameStartTime = millis();
    completedSequences = 0;

    // Reset sequence completion status
    switchSequenceCompleted = false;
    buttonSequenceCompleted = false;
    potSequenceCompleted = false;
    jackSequenceCompleted = false;
    keypadSequenceCompleted = false;

    // Display game start message
    lcd.clear();
    lcd.print("Game Started!");
    lcd.setCursor(0, 1);
    lcd.print("Good luck!");

    // Play start audio
    myDFPlayer.play(1); // Assuming 1 is the start audio file

    // Wait for audio to finish (approximate)
    delay(5000); // Adjust based on your audio length

    // Update display with countdown
    lastDisplayUpdate = millis();
}

void endGame(bool victory)
{
    gameEnded = true;

    lcd.clear();
    if (victory)
    {
        lcd.print("Victory!");
        lcd.setCursor(0, 1);
        lcd.print("All tasks done!");
        myDFPlayer.play(2); // Victory audio
    }
    else
    {
        lcd.print("Game Over!");
        lcd.setCursor(0, 1);
        lcd.print("Time's up!");
        myDFPlayer.play(3); // Defeat audio
    }

    // Turn off red LED, keep green LEDs on if any sequences were completed
    digitalWrite(RED_LED, 0x0);
}

void updateCountdownDisplay()
{
    // Only update display every second to avoid flicker
    if (millis() - lastDisplayUpdate >= 1000)
    {
        lastDisplayUpdate = millis();

        unsigned long elapsedTime = millis() - gameStartTime;
        if (elapsedTime < countdownDuration)
        {
            unsigned long remainingTime = (countdownDuration - elapsedTime) / 1000;

            lcd.clear();
            lcd.print("Time remaining:");
            lcd.setCursor(0, 1);
            lcd.print(remainingTime);
            lcd.print(" seconds");
        }
    }
}

void processKeypadInput(char key)
{
    static char currentSequence[20] = "";
    static int sequenceIndex = 0;

    // Add key to current sequence
    if (sequenceIndex < 19)
    { // Leave room for null terminator
        currentSequence[sequenceIndex++] = key;
        currentSequence[sequenceIndex] = '\0';
    }

    // Check for sequence confirmation keys
    if (key == '*' && !switchSequenceCompleted)
    {
        checkSwitchSequence();
        sequenceIndex = 0;
        currentSequence[0] = '\0';
    }
    else if (key == '#' || key == 'A' || key == 'B' || key == 'C')
    {
        // These keys might be part of a sequence, so we wait for more input
        if (sequenceIndex >= 3)
        {
            // Check if we have a complete sequence with start and end markers
            if (currentSequence[0] == '#' && currentSequence[sequenceIndex - 1] == '#' && !buttonSequenceCompleted)
            {
                checkButtonSequence(currentSequence, sequenceIndex);
                sequenceIndex = 0;
                currentSequence[0] = '\0';
            }
            else if (currentSequence[0] == 'A' && currentSequence[sequenceIndex - 1] == 'A' && !keypadSequenceCompleted)
            {
                checkKeypadSequence(currentSequence, sequenceIndex);
                sequenceIndex = 0;
                currentSequence[0] = '\0';
            }
            else if (currentSequence[0] == 'B' && currentSequence[sequenceIndex - 1] == 'B' && !potSequenceCompleted)
            {
                checkPotSequence();
                sequenceIndex = 0;
                currentSequence[0] = '\0';
            }
            else if (currentSequence[0] == 'C' && currentSequence[sequenceIndex - 1] == 'C' && !jackSequenceCompleted)
            {
                checkJackConnections();
                sequenceIndex = 0;
                currentSequence[0] = '\0';
            }
        }
    }
}

void checkSwitchSequence()
{
    bool correct = true;

    // Check if all switches are in the correct position
    for (int i = 0; i < 6; i++)
    {
        int switchState = digitalRead(SWITCH_PINS[i]) == 0x0 ? 1 : 0;
        if (switchState != switchPositions[i])
        {
            correct = false;
            break;
        }
    }

    if (correct)
    {
        switchSequenceCompleted = true;
        completedSequences++;
        showSuccessMessage("Switches correct!");
        digitalWrite(GREEN_LED, 0x1);
        myDFPlayer.play(4); // Success audio
    }
    else
    {
        showFailureMessage("Switches wrong!");
        flashRedLed();
        myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
}

void checkButtonSequence(char *sequence, int length)
{
    // Extract the button sequence from the input (remove the # markers)
    char buttonInput[10] = "";
    int buttonInputIndex = 0;

    for (int i = 1; i < length - 1; i++)
    {
        buttonInput[buttonInputIndex++] = sequence[i];
    }
    buttonInput[buttonInputIndex] = '\0';

    // Convert to integers and check against the correct sequence
    bool correct = true;
    for (int i = 0; i < 6; i++)
    {
        int buttonNum = buttonInput[i] - '0';
        if (buttonNum != buttonSequence[i])
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
        digitalWrite(GREEN_LED, 0x1);
        myDFPlayer.play(4); // Success audio
    }
    else
    {
        showFailureMessage("Button seq wrong!");
        flashRedLed();
        myDFPlayer.play(5); // Failure audio
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
        digitalWrite(GREEN_LED, 0x1);
        myDFPlayer.play(4); // Success audio
    }
    else
    {
        showFailureMessage("Keypad code wrong!");
        flashRedLed();
        myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
}

void checkPotSequence()
{
    bool correct = true;

    // Check if all potentiometers are at the correct values (with some tolerance)
    const int TOLERANCE = 50; // Tolerance for potentiometer readings

    for (int i = 0; i < 6; i++)
    {
        int potReading = analogRead(POT_PINS[i]);
        if (((potReading - potValues[i])>0?(potReading - potValues[i]):-(potReading - potValues[i])) > TOLERANCE)
        {
            correct = false;
            break;
        }
    }

    if (correct)
    {
        potSequenceCompleted = true;
        completedSequences++;
        showSuccessMessage("Potentiometers OK!");
        digitalWrite(GREEN_LED, 0x1);
        myDFPlayer.play(4); // Success audio
    }
    else
    {
        showFailureMessage("Potentiometers wrong!");
        flashRedLed();
        myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
}

void checkJackConnections()
{
    bool correct = true;

    // Create a matrix to track which jacks are connected
    bool jackMatrix[16][16] = {{false}};

    // First, detect all connections
    for (int i = 0; i < 16; i++)
    {
        // Set current pin as OUTPUT LOW temporarily
        pinMode(JACK_PINS[i], 0x1);
        digitalWrite(JACK_PINS[i], 0x0);

        // Check which other pins detect the LOW signal
        for (int j = 0; j < 16; j++)
        {
            if (i != j)
            {
                if (digitalRead(JACK_PINS[j]) == 0x0)
                {
                    jackMatrix[i][j] = true;
                }
            }
        }

        // Reset pin back to INPUT_PULLUP
        pinMode(JACK_PINS[i], 0x2);
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
        showSuccessMessage("Jack connections OK!");
        digitalWrite(GREEN_LED, 0x1);
        myDFPlayer.play(4); // Success audio
    }
    else
    {
        showFailureMessage("Jack connections wrong!");
        flashRedLed();
        myDFPlayer.play(5); // Failure audio
        applyPenalty();
    }
}

void showSuccessMessage(const char *message)
{
    lcd.clear();
    lcd.print(message);
    lcd.setCursor(0, 1);
    lcd.print("Well done!");

    // Message will be visible for 10 seconds
    lastDisplayUpdate = millis() - 990; // Force update in next cycle
}

void showFailureMessage(const char *message)
{
    lcd.clear();
    lcd.print(message);
    lcd.setCursor(0, 1);
    lcd.print("Try again!");

    // Message will be visible for 10 seconds
    lastDisplayUpdate = millis() - 990; // Force update in next cycle
}

void flashRedLed()
{
    digitalWrite(RED_LED, 0x1);
    delay(500);
    digitalWrite(RED_LED, 0x0);
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
}

// Process commands received from the PC
void processSerialCommand()
{
    String command = Serial.readStringUntil('\n');

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
    }
    else if (command == "RESET")
    {
        // Reset the game
        gameStarted = false;
        gameEnded = false;
        lcd.clear();
        lcd.print("Keypad LED Game");
        lcd.setCursor(0, 1);
        lcd.print("Press START");
        digitalWrite(GREEN_LED, 0x0);
        digitalWrite(RED_LED, 0x0);
        Serial.println("OK:RESET");
    }
}
