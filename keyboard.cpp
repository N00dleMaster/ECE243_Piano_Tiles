#include <stdio.h>

volatile int *RLEDs = (int *) 0xFF200000; // Red LEDs base address
volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port base address

int keyboard() {
    unsigned char byte1 = 0, byte2 = 0, byte3 = 0;
    int PS2_data, RVALID;

    while (1) {
        PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
        RVALID = (PS2_data & 0x8000); // extract the RVALID field

        if (RVALID) {
            byte1 = byte2;
            byte2 = byte3;
            byte3 = PS2_data & 0xFF;

            if (byte2 == 0xF0) { // PS/2 keyboards send 0xF0 as the break code prefix
                // We are only interested in the make code (when the key is pressed)
                continue; 
            }

            // If the last two bytes are the prefix of a key release (0xF0) followed by
            // one of the make codes for "A", "S", "D", or "F", then we reset byte3 to ignore it
            if (byte1 == 0xF0 && (byte3 == 0x1C || byte3 == 0x1B || byte3 == 0x23 || byte3 == 0x2B)) {
                byte3 = 0; // Reset byte3 to avoid acting on key release
            } else {
                switch (byte3) {
                    case 0x1C: // 'A' key make code
                        *RLEDs = 1; // Action for 'A' key
                        break;
                    case 0x1B: // 'S' key make code
                        *RLEDs = 2; // Action for 'S' key
                        break;
                    case 0x23: // 'D' key make code
                        *RLEDs = 4; // Action for 'D' key
                        break;
                    case 0x2B: // 'F' key make code
                        *RLEDs = 8; // Action for 'F' key
                        break;
                    default:
                        // Handle other keys or ignore
                        *RLEDs = 0; // Turn off LEDs for any other key
                        break;
                }
            }
        }
    }
    return 0; // This will never be reached due to the infinite loop
}

int main() {
    // Call the keyboard handler function - it will never return
    keyboard();
    return 0; // This will never be reached
}
