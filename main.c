
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>


void init_lcd() {
	// LCD Enable (LCDEN) & Low Power Waveform (LCDAB)
	LCDCRA = (1<<LCDEN) | (1<<LCDAB) | (0<<LCDIF) | (0<<LCDIE) | (0<<LCDBL);

	// external asynchronous clock source (LCDCS), 1/3 bias (LCD2B), 1/4 duty (LCDMUX1:0), 25 segments enabled (LCDPM2:0)
	LCDCRB = (1<<LCDCS) | (0<<LCD2B) | (1<<LCDMUX1) | (1<<LCDMUX0) | (1<<LCDPM2) | (1<<LCDPM1) | (1<<LCDPM0);

	// prescaler setting N=16 (LCDPS2:0), clock divider setting D=8 (LCDCD2:0)
	LCDFRR = (0<<LCDPS2) | (0<<LCDPS1) | (0<<LCDPS0) | (1<<LCDCD2) | (1<<LCDCD1) | (1<<LCDCD0);

	// drive time 300 microseconds (LCDDC2:0), contrast control voltage 3.35 V (LCDCC3:0)
	LCDCCR = (0<<LCDDC2) | (0<<LCDDC1) | (0<<LCDDC0) | (1<<LCDCC3) | (1<<LCDCC2) | (1<<LCDCC1) | (1<<LCDCC0);

}

uint16_t digitLookUp(uint8_t c) {

	if(c < 48 || c > 57) {
		return 0;
	}

	uint16_t binary[10];

	binary[0] = 0b0001010101010001; // 0
	binary[1] = 0b0000000100010000; // 1
	binary[2] = 0b0001000111100001; // 2
	binary[3] = 0b0001000110110001; // 3
	binary[4] = 0b0000010110110000; // 4
	binary[5] = 0b0001010010110001; // 5
	binary[6] = 0b0001010011110001; // 6
	binary[7] = 0b0001000100010000; // 7
	binary[8] = 0b0001010111110001; // 8
	binary[9] = 0b0001010110110000; // 9
	binary[10] = '\0';

	return binary[(c - 48)];

	/*
	0 needs A, B, C, D, E, F
		LCDDRx = 0001
		LCDDRx+5 = 0101
		LCDDRx+10 = 0101
		LCDDRx+15 = 0001
	1 needs B, C
		LCDDRx = 0000
		LCDDRx+5 = 0001
		LCDDRx+10 = 0001
		LCDDRx+15 = 0000
	2 needs A, B, L, G, E, D
		LCDDRx = 0001
		LCDDRx+5 = 0001
		LCDDRx+10 = 1110
		LCDDRx+15 = 0001
	3 needs A, B, L, G, C, D
		LCDDRx = 0001
		LCDDRx+5 = 0001
		LCDDRx+10 = 1011
		LCDDRx+15 = 0001
	4 needs F, G, L, B, C
		LCDDRx = 0000
		LCDDRx+5 = 0101
		LCDDRx+10 = 1011
		LCDDRx+15 = 0000
	5 needs A, F, G, L, C, D
		LCDDRx = 0001
		LCDDRx+5 = 0100
		LCDDRx+10 = 1011
		LCDDRx+15 = 0001
	6 needs A, F, G, L, E, C, D
		LCDDRx = 0001
		LCDDRx+5 = 0100
		LCDDRx+10 = 1111
		LCDDRx+15 = 0001
	7 needs A, B, C
		LCDDRx = 0001
		LCDDRx+5 = 0001
		LCDDRx+10 = 0001
		LCDDRx+15 = 0000
	8 needs A, B, C, D, F, E, G, L
		LCDDRx = 0001
		LCDDRx+5 = 0101
		LCDDRx+10 = 1111
		LCDDRx+15 = 0001
	9 needs A, B, C, F, G, L
		LCDDRx = 0001
		LCDDRx+5 = 0101
		LCDDRx+10 = 1011
		LCDDRx+15 = 0000
	*/
}

void writeChar(char ch, int pos) {
	// If pos is less than zero or greater than 5, do nothing
	if(pos < 0 || pos > 5) {
		return;
	}

	uint16_t digitBinary = 0;
	uint8_t nibble_0 = 0, nibble_1 = 0, nibble_2 = 0, nibble_3 = 0, oldValue = 0, mask = 0, increment = 0;
	
	// Fetch the value needed to display number "ch" in LCDDRx
	digitBinary = digitLookUp(ch);

	// Because there are 6 positions on 3 LCDDRx registers
	// we need to adjust "increment" so we do not get wrong
	// memory addresses
	if(pos >= 0 && pos <= 1) {
		increment = 0;
	} else if(pos >= 2 && pos <= 3) {
		increment = 1;
	} else {
		increment = 2;
	}

	// Depending on if the pos is even or odd we adjust the nibbles
	// and mask correctly
	if((pos % 2) == 0) {
		nibble_0 = 0b00001111 & (digitBinary >> 12);
		nibble_1 = 0b00001111 & (digitBinary >> 8);
		nibble_2 = 0b00001111 & (digitBinary >> 4);
		nibble_3 = 0b00001111 & digitBinary;
		// This mask is needed to preserve what is on the right side of
		// LCDDRx
		mask = 0b11110000;
	} else {
		nibble_0 = 0b11110000 & (digitBinary >> 8);
		nibble_1 = 0b11110000 & (digitBinary >> 4);
		nibble_2 = 0b11110000 & digitBinary;
		nibble_3 = 0b11110000 & (digitBinary << 4);
		// Mask needed to preserve what is on the left side of LCDDRx
		mask = 0b00001111;
	}

	// Create a pointer and assign the memory address of LCDDR0
	// (is volatile really needed?)
	volatile uint8_t *LCDDRAddress = &LCDDR0;
	// Increment the pointers memory address with the value calculated earlier
	// This is needed to be able to use LCDDR0, LCDDR1, LCDDR2 etc.
	LCDDRAddress = (LCDDRAddress + increment);
	// Preserve the old value by using a mask
	oldValue = mask & *LCDDRAddress;
	// Add the nibble using OR
	*LCDDRAddress = oldValue | nibble_0;
	// Increase the memory address of the pointer with 5 to be able to
	// use LCDDRx+5
	LCDDRAddress = LCDDRAddress + 5;

	oldValue = mask & *LCDDRAddress;
	*LCDDRAddress = oldValue | nibble_1;
	// Increase with 5 to be able to use LCDDRx+10
	LCDDRAddress = LCDDRAddress + 5;

	oldValue = mask & *LCDDRAddress;
	*LCDDRAddress = oldValue | nibble_2;
	// Increase with 5 to be able to use LCDDRx+15
	LCDDRAddress = LCDDRAddress + 5;

	oldValue = mask & *LCDDRAddress;
	*LCDDRAddress = oldValue | nibble_3;

}

void writeLong(uint32_t value) {
	char array[31];
	uint32_t temp = value;
	// n is needed to show the digits correctly (reverse)
	uint8_t n = 5;
	// For loop using modulus and division to extract all the digits in the long int
	for(uint8_t i = 0; i < 31; i++) {
		array[i] = (temp%10) + '0';
		temp /= 10;
		// if the temp variable is less than 1, break loop
		if(temp < 1) {
			break;
		}
	}
	// For loop to show the 6 least significant digits
 	for(uint8_t i = 0; i <= 5; i++) {
 		writeChar(array[i], n);
		n--;
 	}
}

bool is_prime(long i) {
	uint32_t c;
	// Loop to check if a number is dividable with anything less than half the value of "i"
	for(c = 2; c <= i/2; c++) {
		if(i%c == 0) {
			// Return false as the value of i is not a prime number
			return false;
		}
	}

	if(c == i/2 + 1) {
		// Return true as "i" is a prime number
		return true;
	}

	return false;
}

void primes(uint32_t i) {
	// infinite loop
	for(;;) {
		// Check if the number is a prime number
		if(is_prime(i) == true) {
			// If yes, then write to screen
			writeLong(i);
		}
		i++;
	}
}

void init_timer() {
	// Setup prescaler 256, Clock Select (CS12:10)
	TCCR1B = (1<<CS12) | (0<<CS11) | (0<<CS10);
	// Enable timer 1
	TCNT1 = 0;
}

void blink() {
	/* 16-bit max value: 65536
	8 MHz = 8 000 000 Hz
	256 prescaler
	8 000 000 / 256 = 31 250
	65536 / 31250 = 2,097
	Rounded up means 2,1 seconds per overflow

	One second = 65536 / 2,1 = 31207.62

	0 ON
	31207 OFF	current_value <= next_value
	62414 ON	current_value <= next_value
	28085 OFF	current_value >= next_Value
	59292 ON	current_value <= next_value
	24963 OFF	current_value >= next_value
	56170 ON	current_value <= next_value
	21841 OFF	current_value >= next_value
	53048 ON	current_value <= next_value
	18719 OFF	current_value >= next_value
	*/

	uint16_t current_value = TCNT1, next_value = 0, one_second = 31207;
	next_value = current_value + one_second;

	for(;;) {
		current_value = TCNT1;
		if((next_value - current_value) <= 312) {
			LCDDR3 = LCDDR3 ^ 0b00000001;
			next_value = current_value + one_second;
		}
	}
}

void init_button() {
	PORTB = (1<<PB7);
}

void button() {
	bool latch = false;
	uint8_t buttonNow = 0, buttonPrev = 0;

	for(;;) {
		// Read value of PINB7
		buttonNow = (PINB >> 7);
		// If the button state is 0 and the previous state was 1 then change latch state to true
		if(buttonNow == 0 && buttonPrev == 1) {
			if(latch == true) {
				latch = false;
			} else {
				latch = true;
			}
		}

		// Store the new value of buttonNow in buttonPrev
		buttonPrev = buttonNow;
		if(latch == true) {
			// Turn on the LCDDR1 bit 1 if latch is true
			LCDDR1 = 0b00000010;
			// Turn off the LCDDR0 if latch is true
			LCDDR0 = 0b00000000;
		} else {
			LCDDR1 = 0b00000000;
			LCDDR0 = 0b00000100;
		}
	}
}

void primes_part4(uint16_t i) {
	if(is_prime(i) == true) {
		writeLong(i);
	}
}

void blink_part4(uint16_t *current_value, uint16_t *next_value, uint16_t *one_second) {
	//
	if((*next_value - *current_value) <= 3120) {
		LCDDR3 = LCDDR3 ^ 0b00000001;
		*next_value = *current_value + *one_second;
	}
}

void button_part4(uint8_t *buttonPrev, bool *latch) {
	unsigned int buttonNow = (PINB >> 7);
	if(buttonNow == 0 && *buttonPrev == 1) {
		if(*latch == true) {
			*latch = false;
		} else {
			*latch = true;
		}
	}

	*buttonPrev = buttonNow;

	if(*latch == true) {
		LCDDR1 = LCDDR1 | 0b000000010;
		LCDDR0 = LCDDR0 ^ 0b000000100;
	} else {
		LCDDR1 = LCDDR1 ^ 0b000000010;
		LCDDR0 = LCDDR0 | 0b000000100;
	}
}

int main(void)
{
	// Setup the clockspeed
	CLKPR  = 0x80;
	CLKPR  = 0x00;
	
	init_lcd();
	//writeLong(123456);
	//if(is_prime(2) == true) {
	//	writeLong(1);
	//}
	primes(1);
	init_timer();
	//blink();
	init_button();
	//button();


    while (1) {}

	// Part 4
	/*
	//unsigned int buttonPrev = 0, latch_button = 0;
	//unsigned int prev_value = 0, next_value = 0, one_second = 31207;

	bool latch_button = false;
	uint8_t buttonPrev = 0;
	uint16_t current_value = TCNT1, next_value = 0, one_second = 31207;

	next_value = TCNT1 + one_second;

	for(uint32_t i = 26000;; i++) {
		current_value = TCNT1;
		blink_part4(&current_value, &next_value, &one_second);
		button_part4(&buttonPrev, &latch_button);
		primes_part4(i);
	}
	*/
}

