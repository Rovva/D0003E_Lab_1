
#include <avr/io.h>


void LCDinit() {
	// LCD Enable (LCDEN) & Low Power Waveform (LCDAB)
	LCDCRA = (1<<LCDEN) | (1<<LCDAB) | (0<<LCDIF) | (0<<LCDIE) | (0<<LCDBL);

	// external asynchronous clock source (LCDCS), 1/3 bias (LCD2B), 1/4 duty (LCDMUX1:0), 25 segments enabled (LCDPM2:0)
	LCDCRB = (1<<LCDCS) | (0<<LCD2B) | (1<<LCDMUX1) | (1<<LCDMUX0) | (1<<LCDPM2) | (1<<LCDPM1) | (1<<LCDPM0);

	// prescaler setting N=16 (LCDPS2:0), clock divider setting D=8 (LCDCD2:0)
	LCDFRR = (0<<LCDPS2) | (0<<LCDPS1) | (0<<LCDPS0) | (1<<LCDCD2) | (1<<LCDCD1) | (1<<LCDCD0);

	// drive time 300 microseconds (LCDDC2:0), contrast control voltage 3.35 V (LCDCC3:0)
	LCDCCR = (0<<LCDDC2) | (0<<LCDDC1) | (0<<LCDDC0) | (1<<LCDCC3) | (1<<LCDCC2) | (1<<LCDCC1) | (1<<LCDCC0);

}

unsigned short digitLookUp(unsigned char c) {
	unsigned long binary = 0;
	if(c == 48) {
		// 0 needs A, B, C, D, F, E, K, P
		/*
			LCDDRx = 1001
			LCDDRx+5 = 0101
			LCDDRx+10 = 0101
			LCDDRx+15 = 0101
		*/
		binary = 0b0001010101010001;
	} else if(c == 49) {
		// 1 needs B, C
		/*
			LCDDRx = 0000
			LCDDRx+5 = 0001
			LCDDRx+10 = 0001
			LCDDRx+15 = 0000
		*/
		binary = 0b0000000100010000;
	} else if(c == 50) {
		// 2 needs A, B, L, G, E, D
		/*
			LCDDRx = 0001
			LCDDRx+5 = 0001
			LCDDRx+10 = 1110
			LCDDRx+15 = 0001
		*/
		binary = 0b0001000111100001;
	} else if(c == 51) {
		// 3 needs A, B, L, G, C, D
		/*
			LCDDRx = 0001
			LCDDRx+5 = 0001
			LCDDRx+10 = 1011
			LCDDRx+15 = 0001
		*/
		binary = 0b0001000110110001;
	} else if(c == 52) {
		// 4 needs F, G, L, B, C
		/*
			LCDDRx = 0000
			LCDDRx+5 = 0101
			LCDDRx+10 = 1011
			LCDDRx+15 = 0000
		*/
		binary = 0b0000010110110000;
	} else if(c == 53) {
		// 5 needs A, F, G, L, C, D
		/*
			LCDDRx = 0001
			LCDDRx+5 = 0100
			LCDDRx+10 = 1011
			LCDDRx+15 = 0001
		*/
		binary = 0b0001010010110001;
	} else if(c == 54) {
		// 6 needs A, F, G, L, E, C, D
		/*
			LCDDRx = 0001
			LCDDRx+5 = 0100
			LCDDRx+10 = 1111
			LCDDRx+15 = 0001
		*/
		binary = 0b0001010011110001;
	} else if(c == 55) {
		// 7 needs A, B, C
		/*
			LCDDRx = 0001
			LCDDRx+5 = 0001
			LCDDRx+10 = 0001
			LCDDRx+15 = 0000
		*/
		binary = 0b0001000100010000;
	} else if(c == 56) {
		// 8 needs A, B, C, D, F, E, G, L
		/*
			LCDDRx = 0001
			LCDDRx+5 = 0101
			LCDDRx+10 = 1111
			LCDDRx+15 = 0001
		*/
		binary = 0b0001010111110001;
	} else if(c == 57) {
		// 9 needs A, B, C, F, G, L
		/*
			LCDDRx = 0001
			LCDDRx+5 = 0101
			LCDDRx+10 = 1011
			LCDDRx+15 = 0000
		*/
		binary = 0b0001010110110000;
	} else {
		binary = 0b0000000000000000;
	}
	return binary;
}

void writeChar(unsigned char letter, unsigned int position) {
	unsigned long digitBinary;
	unsigned char nibble_0 = 0, nibble_1 = 0, nibble_2 = 0, nibble_3 = 0, oldValue = 0;
	
	digitBinary = digitLookUp(letter);
	
	if(position == 0) {
		// Extract nibble from the binary output of binLookup
		//               0001 0101 1011 0000
		//           0000 1111
		nibble_0 = 0b00001111 & (digitBinary >> 12);
		//          0001 0101 1011 0000
		//           0000 1111
		nibble_1 = 0b00001111 & (digitBinary >> 8);
		//     0001 0101 1011 0000
		//           0000 1111
		nibble_2 = 0b00001111 & (digitBinary >> 4);
		//0001 0101 1011 0000
		//           0000 1111
		nibble_3 = 0b00001111 & digitBinary;
				
		// Store the old nibble value for position 1 from the register
		oldValue = 0b11110000 & LCDDR0;
		// Combine the old nibble value for position 1 with nibble value of position 0
		// and set LCDDR0 registers with the new value
		LCDDR0 = oldValue | nibble_0;
		oldValue = 0b11110000 & LCDDR5;
		LCDDR5 = oldValue | nibble_1;
		oldValue = 0b11110000 & LCDDR10;
		LCDDR10 = oldValue | nibble_2;
		oldValue = 0b11110000 & LCDDR15;
		LCDDR15 = oldValue | nibble_3;
	} else if(position == 1) {
		// Because of different position we need to bitshift differently
		//               0001 0101 1011 0000
		//               1111 0000
		nibble_0 = 0b11110000 & (digitBinary >> 8);
		//          0001 0101 1011 0000
		//               1111 0000
		nibble_1 = 0b11110000 & (digitBinary >> 4);
		//     0001 0101 1011 0000
		//               1111 0000
		nibble_2 = 0b11110000 & digitBinary;
		//0001 0101 1011 0000
		//               1111 0000
		nibble_3 = 0b11110000 & (digitBinary << 4);
		
		oldValue = 0b00001111 & LCDDR0;
		LCDDR0 = oldValue | nibble_0;

		oldValue = 0b00001111 & LCDDR5;
		LCDDR5 = oldValue | nibble_1;

		oldValue = 0b00001111 | LCDDR10;
		LCDDR10 = oldValue | nibble_2;

		oldValue = 0b00001111 | LCDDR15;
		LCDDR15 = oldValue | nibble_3;
	} else if(position == 2) {
		nibble_0 = 0b00001111 & (digitBinary >> 12);
		nibble_1 = 0b00001111 & (digitBinary >> 8);
		nibble_2 = 0b00001111 & (digitBinary >> 4);
		nibble_3 = 0b00001111 & digitBinary;

		oldValue = 0b11110000 & LCDDR1;
		LCDDR1 = oldValue | nibble_0;
		oldValue = 0b11110000 & LCDDR6;
		LCDDR6 = oldValue | nibble_1;
		oldValue = 0b11110000 & LCDDR11;
		LCDDR11 = oldValue | nibble_2;
		oldValue = 0b11110000 & LCDDR16;
		LCDDR16 = oldValue | nibble_3;
	} else if(position == 3) {
		nibble_0 = 0b11110000 & (digitBinary >> 8);
		nibble_1 = 0b11110000 & (digitBinary >> 4);
		nibble_2 = 0b11110000 & digitBinary;
		nibble_3 = 0b11110000 & (digitBinary << 4);

		oldValue = 0b00001111 & LCDDR1;
		LCDDR1 = oldValue | nibble_0;
		oldValue = 0b00001111 & LCDDR6;
		LCDDR6 = oldValue | nibble_1;
		oldValue = 0b00001111 & LCDDR11;
		LCDDR11 = oldValue | nibble_2;
		oldValue = 0b00001111 & LCDDR16;
		LCDDR16 = oldValue | nibble_3;
	} else if(position == 4) {
		nibble_0 = 0b00001111 & (digitBinary >> 12);
		nibble_1 = 0b00001111 & (digitBinary >> 8);
		nibble_2 = 0b00001111 & (digitBinary >> 4);
		nibble_3 = 0b00001111 & digitBinary;

		oldValue = 0b11110000 & LCDDR2;
		LCDDR2 = oldValue | nibble_0;
		oldValue = 0b11110000 & LCDDR7;
		LCDDR7 = oldValue | nibble_1;
		oldValue = 0b11110000 & LCDDR12;
		LCDDR12 = oldValue | nibble_2;
		oldValue = 0b11110000 & LCDDR17;
		LCDDR17 = oldValue | nibble_3;
	} else if(position == 5) {
		nibble_0 = 0b11110000 & (digitBinary >> 8);
		nibble_1 = 0b11110000 & (digitBinary >> 4);
		nibble_2 = 0b11110000 & digitBinary;
		nibble_3 = 0b11110000 & (digitBinary << 4);

		oldValue = 0b00001111 & LCDDR2;
		LCDDR2 = oldValue | nibble_0;
		oldValue = 0b00001111 & LCDDR7;
		LCDDR7 = oldValue | nibble_1;
		oldValue = 0b00001111 & LCDDR12;
		LCDDR12 = oldValue | nibble_2;
		oldValue = 0b00001111 & LCDDR17;
		LCDDR17 = oldValue | nibble_3;
	} else {
		return;
	}
}

void writeLong(long value) {
	char array[31];
	long temp = value;
	// n is needed to show the digits correctly (reverse)
	int n = 5;
	// For loop using modulus and division to extract all the digits in the long int
	for(int i = 0; i < 31; i++) {
		array[i] = (temp%10) + '0';
		temp /= 10;
		// if the temp variable is less than 1, break loop
		if(temp < 1) {
			break;
		}
	}
	// For loop to show the 6 least significant digits
 	for(int i = 0; i <= 5; i++) {
 		writeChar(array[i], n);
		n--;
 	}
}

int main(void)
{
	// Setup the clockspeed
	CLKPR  = 0x80;
	CLKPR  = 0x00;
	
	LCDinit();
	writeLong(123456);
    while (1) 
    {
    }
}

