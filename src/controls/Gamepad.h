#ifndef GAMEPAD_WRAPPER
#define GAMEPAD_WRAPPER
#include "Adafruit_seesaw.h"

#define BUTTON_UP 6
#define BUTTON_DOWN 1 //2
#define BUTTON_LEFT 2 //5
#define BUTTON_RIGHT 5 //1
#define BUTTON_SELECT 0
#define BUTTON_START 16
const uint32_t GAMEPAD_BUTTON_MASK = (1UL << BUTTON_UP) | (1UL << BUTTON_DOWN) | (1UL << BUTTON_START) |
                       (1UL << BUTTON_LEFT) | (1UL << BUTTON_RIGHT) | (1UL << BUTTON_SELECT);




class GamepadWrapper
{
    Adafruit_seesaw ss;
    // From -1 to 1, with 0 as centre
    int dead_zone = 10;
    int delta = 5;
    int address = 0x50;
public:
    int last_x = 0, last_y = 0;
    float current_x, current_y;
    bool LEFT_PRESSED, RIGHT_PRESSED, DOWN_PRESSED, UP_PRESSED, SELECT_PRESSED, START_PRESSED = false;
    bool LEFT_TRIGGERED, RIGHT_TRIGGERED, DOWN_TRIGGERED, UP_TRIGGERED, SELECT_TRIGGERED, START_TRIGGERED = false;
    bool HAS_MOVED, X_DISPLACED, Y_DISPLACED;

    GamepadWrapper(int d_zone=10, int delt=5, int addr=0x50) : 
        ss(&Wire1), dead_zone(d_zone), delta(delt), address(addr)
    {
        //init();
    }

    void init()
    {
        Serial.println("Gamepad Initialising");
        if (!ss.begin(address))
        {
            Serial.println("ERROR! seesaw not found on 0x50");
            //while (1) delay(1);
        }
        Serial.println("Gamepad started");
        uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
        if (version != 5743)
        {
            Serial.print("Wrong firmware loaded? ");
            Serial.println(version);
            //while (1) delay(10);
        }
        ss.pinModeBulk(GAMEPAD_BUTTON_MASK, INPUT_PULLUP);
        //ss.setGPIOInterrupts(GAMEPAD_BUTTON_MASK, 1);
    }

    void update()
    {
        LEFT_TRIGGERED = false;
        RIGHT_TRIGGERED = false; 
        DOWN_TRIGGERED = false;
        UP_TRIGGERED = false;
        SELECT_TRIGGERED = false;
        START_TRIGGERED = false;
        HAS_MOVED = false;
        X_DISPLACED = false;
        Y_DISPLACED = false;
        // Joystick read and update
        int x = 1023 - ss.analogRead(14);
        int y = 1023 - ss.analogRead(15);
        current_x = (((float)x) / 512 ) - 1;
        current_y = (((float)y) / 512 ) - 1;
        if( (abs(x - last_x) > delta)  ||  (abs(y - last_y) > delta) ) {
            last_x = x;
            last_y = y;
            HAS_MOVED = true;
        } else { HAS_MOVED = false;}
        X_DISPLACED = (abs(x - 512) > dead_zone);
        Y_DISPLACED = (abs(y - 512) > dead_zone);

        // Buttons read and update
        uint32_t buttons = ss.digitalReadBulk(GAMEPAD_BUTTON_MASK);

        if (! (buttons & (1UL << BUTTON_LEFT))) {
            if(!LEFT_PRESSED) LEFT_TRIGGERED = true;
            LEFT_PRESSED = true;
        } else LEFT_PRESSED = false;
        if (! (buttons & (1UL << BUTTON_RIGHT))) {
            if(!RIGHT_PRESSED) RIGHT_TRIGGERED = true;
            RIGHT_PRESSED = true;
        } else RIGHT_PRESSED = false;
        if (! (buttons & (1UL << BUTTON_UP))) {
            if(!UP_PRESSED) UP_TRIGGERED = true;
            UP_PRESSED = true;
        } else UP_PRESSED = false;
        if (! (buttons & (1UL << BUTTON_DOWN))) {
            if(!DOWN_PRESSED) DOWN_TRIGGERED = true;
            DOWN_PRESSED = true;
        } else DOWN_PRESSED = false;
    }

    void printStatus() {
        Serial.print(">>>> X: "); Serial.print(last_x); Serial.print(", Y: "); Serial.print(last_y);
        Serial.print(" A:"); Serial.print(LEFT_PRESSED); 
        Serial.print(" B:"); Serial.print(RIGHT_PRESSED); 
        Serial.print(" X:"); Serial.print(UP_PRESSED); 
        Serial.print(" Y:"); Serial.print(DOWN_PRESSED); 
        Serial.print(" A+:"); Serial.print(LEFT_TRIGGERED); 
        Serial.print(" B_:"); Serial.print(RIGHT_TRIGGERED); 
        Serial.println();
    }

};


#endif