#include <stdio.h>
    
volatile int *RLEDs = (int *) 0x0FF200000;


int keyboard(){
	unsigned char byte3 = 0;
  	volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address
	int PS2_data, RVALID;

	while (1) {
		byte3 = -1;
		PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if (RVALID != 0){
			// byte1 = byte2;
			// byte2 = byte3;
			byte3 = PS2_data & 0xFF;
			switch (byte3) {
				case 0x74:
					*RLEDs = 1;
					return 1;
				case 0x6B:
					*RLEDs = 2;
					return 2;
				case 0x75:
					*RLEDs = 3;
					return 3;						
			}
		}
	}
}