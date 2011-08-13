// A Turing Machine implementation using the GNU Multi-Precision Arithmetic
// library (GMP)

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

void in_place_update(mpz_t tape, mp_bitcnt_t *head, unsigned char *state) {
	// Updates the given Turing Machine in-place. The first argument is the
	// tape, the second is the position of the read head and the third is the
	// machine's current state.

	// The machine we use here is U(15,2) described by Turlough Neary in "Small
	// universal Turing Machines"

	unsigned short symbol;		// The symbol we've read
	unsigned short write;		// The symbol we need to write
	unsigned short move_right;	// 1 == move head right, 0 == move head left
	int state_int = (int) *state;
	
	// First we branch based the symbol under our head
	if (mpz_tstbit(tape, *head)) {
		// We have a 1
		symbol = 1;
		switch (state_int) {
			case 0:
				write = 0;
				move_right = 1;
				*state = 1;
				break;
			case 1:
				write = 1;
				move_right = 1;
				*state = 2;
				break;
			case 2:
				write = 0;
				move_right = 0;
				*state = 6;
				break;
			case 3:
				write = 0;
				move_right = 0;
				*state = 5;
				break;
			case 4:
				write = 1;
				move_right = 1;
				*state = 0;
				break;
			case 5:
				write = 1;
				move_right = 0;
				*state = 3;
				break;
			case 6:
				write = 0;
				move_right = 0;
				*state = 7;
				break;
			case 7:
				write = 1;
				move_right = 0;
				*state = 8;
				break;
			case 8:
				write = 0;
				move_right = 1;
				*state = 0;
				break;
			case 9:
				write = 0;
				move_right = 0;
				*state = 10;
				break;
			case 10:
				write = 0;
				move_right = 1;
				*state = 11;
				break;
			case 11:
				write = 0;
				move_right = 1;
				*state = 12;
				break;
			case 12:
				write = 0;
				move_right = 0;
				*state = 1;
				break;
			case 13:
				write = 0;
				move_right = 0;
				*state = 2;
				break;
			case 14:
				write = 0;
				move_right = 1;
				*state = 13;
				break;
			default:
				break;
		}
	}
	else {
		// We have a 0
		symbol = 0;
		switch (state_int) {
			case 0:
				write = 1;
				move_right = 1;
				*state = 0;
				break;
			case 1:
				write = 1;
				move_right = 1;
				*state = 0;
				break;
			case 2:
				write = 0;
				move_right = 0;
				*state = 4;
				break;
			case 3:
				write = 1;
				move_right = 0;
				*state = 4;
				break;
			case 4:
				write = 1;
				move_right = 0;
				*state = 3;
				break;
			case 5:
				write = 1;
				move_right = 0;
				*state = 3;
				break;
			case 6:
				write = 1;
				move_right = 0;
				*state = 6;
				break;
			case 7:
				write = 1;
				move_right = 0;
				*state = 6;
				break;
			case 8:
				write = 1;
				move_right = 0;
				*state = 9;
				break;
			case 9:
				// This is a halting state.
				write = 0;
				move_right = 0;
				*state = 0;
				break;
			case 10:
				write = 1;
				move_right = 1;
				*state = 13;
				break;
			case 11:
				write = 1;
				move_right = 1;
				*state = 11;
				break;
			case 12:
				write = 1;
				move_right = 1;
				*state = 11;
				break;
			case 13:
				write = 0;
				move_right = 1;
				*state = 14;
				break;
			case 14:
				write = 1;
				move_right = 1;
				*state = 13;
				break;
			default:
				break;
		}
	}

	// Output
	if (write != symbol) {
		// If the bit we read is different to the one we need to write, flip it
		mpz_combit(tape, *head);
	}

	// Move
	if (move_right) {
		*head = *head+1;
	}
	else {
		if (*head == 0) {
			// Don't fall off the end of the tape!
			exit(0);
		}
		*head = *head - 1;
	}
	gmp_printf("%Zd\n", tape);
	mp_bitcnt_t one_count = mpz_popcount(tape);
	if (one_count > 1000) {
		exit(0);
	}
}

int main() {
	// Create a tape for our Turing Machine. This initialises to 0.
	mpz_t tape;
	mpz_init(tape);
	//mpz_init_set_str(tape, "64", 10);
	// Define the head as being at the "start" of the tape. In our case, this is
	// the least-significant bit, which is 0.
	mp_bitcnt_t head = 0;

	// We start in state 0, arbitrarily
	char state = 0;

	int i;
	for (i=0; i < 100; i++) {
		in_place_update(tape, &head, &state);
	}
}
