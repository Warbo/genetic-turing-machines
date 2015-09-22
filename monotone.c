// A Monotone Turing Machine implementation using the GNU Multi-Precision
// Arithmetic library (GMP)

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })

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
				// 0 as "read from input tape".
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
			*head = *head + 1;
		}
		*head = *head - 1;
	}
	//gmp_printf("%Zd\n", tape);
}

void mutate_once(mpz_t tape) {
	// Takes a tape (a binary number) and causes a mutation at some point on the
	// tape. The bit to be flipped is determined by counting coin tosses until a
	// tails appears.
	mp_bitcnt_t to_flip = 0;
	while (rand() % 2) {
		to_flip++;
	}

	// We have the bit that needs flipping. Now flip it:
	mpz_combit(tape, to_flip);
}

void run(mpz_t tape, mpz_t goal, unsigned long int goal_bits, unsigned int steps, mp_bitcnt_t *score, mpz_t *result) {
	// Builds a monotone machine out of the given work tape, and runs it until
	// it either generates the goal or performs the given number of steps.

	// Create blank input and output tapes for our Monotone Turing Machine, and
	// a copy of the work tape
	mpz_t work_tape, in_tape, out_tape;
	mpz_init(work_tape);
	mpz_init(in_tape);
	mpz_init(out_tape);
	mpz_set(work_tape, tape);

	// Define the heads as being at the "start" of the tapes. In our case, this
	// is the least-significant bit, which is 0.
	mp_bitcnt_t work_head = 0;
	mp_bitcnt_t in_head = 0;
	mp_bitcnt_t out_head = 0;

	// We start in state 0, arbitrarily
	char state = 0;

	unsigned int step = 0;
	*score = goal_bits;
	while ((work_head < goal_bits) && (step < steps)) {
		in_place_update(work_tape, &work_head, in_tape, &in_head, out_tape, &out_head, &state);
		*score = min(*score, mpz_hamdist(out_tape, goal));
		step++;
	}

	// Clean up
	if (result > 0) {
		mpz_set(*result, out_tape);
	}
	mpz_clear(work_tape);
	mpz_clear(in_tape);
	mpz_clear(out_tape);
}

int main() {
	// Entry point. First we define our goal:
	mpz_t goal;		// This is the output we want to generate
	mpz_init(goal);
	mpz_set_ui(goal, (unsigned long int) 12345);
	// Now we've got a goal number, we need to work out how many bits it
	// contains (so that we can stop our machines when appropriate)
	mpz_t temp;		// This will store 2^goal_bits
	mpz_init(temp);
	unsigned long int goal_bits = 0;		// This will store how many bits goal needs
	// Keep increasing goal_bits until temp becomes bigger than goal
	while (mpz_cmp(temp, goal) <= 0) {
		// Set temp to be 2^goal_bits, and increment goal_bits
		mpz_ui_pow_ui(temp, (unsigned long int) 2, ++goal_bits);
	}
	mpz_clear(temp);
	// Our result is now one too many
	goal_bits--;

	// Now we define simulation parameters
	unsigned int cutoff = 1000;		// Kill machines after this many steps
	unsigned int attempts = 10;		// The number of machines to try
	unsigned int population_size = 100;		// The number of machines to keep around

	// Next we set up our population:
	mpz_t* population = (mpz_t*) malloc(population_size * sizeof(mpz_t));		// Store the work tapes
	mp_bitcnt_t* scores = (mp_bitcnt_t*) malloc(population_size * sizeof(mp_bitcnt_t));		// Store the best scores
	int* above_average = (int*) malloc(population_size * sizeof(int));		// Which are keepers
	int* below_average = (int*) malloc(population_size * sizeof(int));		// Which are losers
	int above_average_n = 0;
	int below_average_n = 0;
	int average;
	srand(time(0)); // Seed our random number generator
	// Set up our arrays
	unsigned int i;
	for (i=0; i < population_size; i++) {
		mpz_init(population[i]);
		above_average[i] = -1;
		below_average[i] = -1;
	}

	unsigned int attempt = 0;
	mp_bitcnt_t min_score = goal_bits;
	mp_bitcnt_t max_score = 0;
	while (attempt < attempts) {
		// Run all of the machines in the population and get their scores
		min_score = goal_bits;
		max_score = 0;
		for (i=0; i < population_size; i++) {
			printf(",");
			run(population[i], goal, goal_bits, cutoff, &scores[i], (mpz_t*) 0);
			min_score = min(min_score, scores[i]);
			max_score = max(max_score, scores[i]);
		}

		// Find the average score (rounded down to be conservative)
		average = (max_score - min_score) / 2;

		// Categorise our results
		below_average_n = 0;
		above_average_n = 0;
		for (i=0; i < population_size; i++) {
			if (scores[i] < average) {
				// Mark this for dropping
				below_average[below_average_n] = i;
				below_average_n++;
			}
			else {
				// Mark this for keeping
				above_average[above_average_n] = i;
				above_average_n++;
			}
		}

		if (attempt < (attempts-1)) {
			// Now replace those to be dropped with clones of those to be kept
			for (i=0; i < below_average_n; i++) {
				mpz_set(population[below_average[i]], population[above_average[rand() % above_average_n]]);
			}

			// Now mutate our population
			for (i=0; i < population_size; i++) {
				mutate_once(population[i]);
			}
		}
		
		attempt++;
		printf(".");
	}

	// Our simulation is over. Let's run the best.
	mpz_t result;
	mpz_init(result);
	for (i=0; i < population_size; i++) {
		if (scores[i] == min_score) {
			run(population[i], goal, goal_bits, cutoff, &scores[i], &result);
			break;
		}
	}
		
	printf("%Zd\n",result);
	
}
