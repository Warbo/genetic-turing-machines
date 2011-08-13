// A Turing Machine implementation using the GNU Multi-Precision Arithmetic
// library (GMP)

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

void in_place_update(
	mpz_t tape,     mp_bitcnt_t *head,
	mpz_t in_tape,  mp_bitcnt_t *in_head,
	mpz_t out_tape, mp_bitcnt_t *out_head,
	unsigned char *state
) {
	// Updates the given Turing Machine in-place. The arguments are pairs that
	// describe a tape (the contents and the head position), with the work tape
	// then the input tape then the output tape. The final argument is the
	// machine's current state.

	// The machine we use here is U(15,2) described by Turlough Neary in "Small
	// universal Turing Machines". We take the halting configuration (state 9,
	// input 0) and replace it with extra states that handle input and output on
	// our extra tapes. The way this works is shown below:
	//
	//                                                                            /--> Input is 0 --> put 0, move left, state 0
	//                                          /--> (15,0) --> Reading input ---{
	//                                         /                                  \--> Input is 1 --> put 1, move left, state 0
	// (9,0) --> put 0, move left, state 15 --{
    //                                         \                                                                 /--> (16,0) --> Write 0 --> put 0, move left, state 0
	//                                          \--> (15,1) --> Writing output --> put 1, move left, state 16 --{
	//                                                                                                           \--> (16,1) --> Write 1 --> put 0, move left, state 0
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
			case 15:
				// This is a new state that allows us to perform I/O. Since we
				// have read a 1, we need to write. We need another state to
				// decide what we need to output.
				write = 1;
				move_right = 0;
				*state = 16;
				break;
			case 16:
				// This is a new state that performs output. We have read a 1,
				// so put that on the output tape.
				mpz_setbit(out_tape, *out_head);
				// Shift the output tape by one cell.
				*out_head = *out_head + 1;
				write = 0;			// Arbitrary
				move_right = 0;		// Arbitrary, but go left for consistency
				*state = 0;			// Arbitrary
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
				// This was the halting state. Instead, we transition to a new
				// state that handles our I/O
				write = 0;
				move_right = 0;
				*state = 15;
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
			case 15:
				// This is a new state which gives us I/O abilities. We treat a
				// 0 as "read from input tape".put 0, move left, state 0
				if (mpz_tstbit(in_tape, *in_head)) {
					// The input tape contains a 1. Write it to the work tape.
					write = 1;
				}
				else {
					// The input tape contains a 0. Write it to the work tape.
					write = 0;
				}
				// Shift our input head 1 cell
				*in_head = *in_head + 1;
				move_right = 0;		// Shift our head left (for consistency)
				*state = 0;			// Arbitrary
				break;
			case 16:
				// This is a new state that performs output. We have read a 0,
				// so put that on the output tape. Since the output tape default
				// is 0, we just skip ahead.
				*out_head = *out_head + 1;
				write = 0;			// Arbitrary
				move_right = 0;		// Arbitrary, but go left for consistency
				*state = 0;			// Arbitrary
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
}

int main() {
	// Create a tape for our Monotone Turing Machine. This initialises to 0.
	mpz_t work_tape, in_tape, out_tape;
	mpz_init(work_tape);
	mpz_init(in_tape);
	mpz_init(out_tape);
	// Define the heads as being at the "start" of the tapes. In our case, this
	// is the least-significant bit, which is 0.
	mp_bitcnt_t work_head = 0;
	mp_bitcnt_t in_head = 0;
	mp_bitcnt_t out_head = 0;

	// We start in state 0, arbitrarily
	char state = 0;

	int i;
	for (i=0; i < 100; i++) {
		in_place_update(work_tape, &work_head, in_tape, &in_head, out_tape, &out_head, &state);
	}
}
