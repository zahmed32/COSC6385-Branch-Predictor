// my_predictor.h
// This file contains a sample gshare_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.

class gshare_update : public branch_update {
public:
	unsigned int index;
};

class gshare_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	15
#define TABLE_BITS	15
	gshare_update u;
	branch_info bi;
	unsigned int history;
	unsigned char tab[1<<TABLE_BITS];

	gshare_predictor (void) : history(0) { 
		memset (tab, 0, sizeof (tab));
	}

	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
			u.index = 
				  (history << (TABLE_BITS - HISTORY_LENGTH)) 
				^ (b.address & ((1<<TABLE_BITS)-1));
			u.direction_prediction (tab[u.index] >> 1);
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			unsigned char *c = &tab[((gshare_update*)u)->index];
			if (taken) {
				if (*c < 3) (*c)++;
			} else {
				if (*c > 0) (*c)--;
			}
			history <<= 1;
			history |= taken;
			history &= (1<<HISTORY_LENGTH)-1;
		}
	}
};

//
// Pentium M hybrid branch predictors
// This class implements a simple hybrid branch predictor based on the Pentium M branch outcome prediction units. 
// Instead of implementing the complete Pentium M branch outcome predictors, the class below implements a hybrid 
// predictor that combines a bimodal predictor and a global predictor. 
class pm_update : public branch_update {
public:
        unsigned int index;
		unsigned int global_row;
    	unsigned int global_tag;
		int global_way;
		
};

struct global_entry {
    bool valid;
    unsigned int tag;
    unsigned char counter;
};

class pm_predictor : public branch_predictor {
public:
        pm_update u;
		unsigned char bimodal[4096];  //This creates 4,096 counter slots, numbered 0 through 4,095
		bool is_conditional;
		unsigned int history;
		global_entry global_table[512][2];
		int replace_way[512];

        pm_predictor (void) {
			history = 0;
			for (int i = 0; i < 4096; i++) {
        	bimodal[i] = 1;  //set  counter to 1
    }

		for (int row = 0; row < 512; row++) {
			replace_way[row] = 0;
    		for (int way = 0; way < 2; way++) {
				global_table[row][way].valid = false;
				global_table[row][way].tag = 0;
				global_table[row][way].counter = 1;
    }
}
        }

        branch_update *predict (branch_info & b) {
			
		is_conditional = (b.br_flags & BR_CONDITIONAL) != 0;

		if (is_conditional) {
			u.index = b.address % 4096;
			unsigned int mixed = (b.address ^ history) & 32767u;

			u.global_row = (mixed >> 6) & 511u;
			u.global_tag = mixed & 63u;
			u.global_way = -1;

			for (int way = 0; way < 2; way++) {
				if (global_table[u.global_row][way].valid &&
					global_table[u.global_row][way].tag == u.global_tag) {
					u.global_way = way;
					break;
				}
			}
			bool guess_taken = bimodal[u.index] >= 2;

			if (u.global_way != -1) {
				guess_taken =
					global_table[u.global_row][u.global_way].counter >= 2;
			}

			u.direction_prediction(guess_taken);
		} else {
			u.direction_prediction(true);
		}

		u.target_prediction(0);
		return &u;

        }

        void update (branch_update *u, bool taken, unsigned int target) {
				if (!is_conditional) {
						return;
					}
			    unsigned int slot = this->u.index; //retrieve slot

				if (taken) {
					if (bimodal[slot] < 3) {
						bimodal[slot]++;   //increase counter if decision is taken
					}
				} else {
					if (bimodal[slot] > 0) {
						bimodal[slot]--;  // decrease counter if decision is not taken
					}
				}

				int row = this->u.global_row;
				int way = this->u.global_way;

				if (way != -1) {
					// Hit: train the existing counter.
					if (taken) {
						if (global_table[row][way].counter < 3) {
							global_table[row][way].counter++;
						}
					} else {
						if (global_table[row][way].counter > 0) {
							global_table[row][way].counter--;
						}
					}
				} else {
					// Miss: use an empty entry if possible.
					if (!global_table[row][0].valid) {
						way = 0;
					} else if (!global_table[row][1].valid) {
						way = 1;
					} else {
						way = replace_way[row];
					}

					global_table[row][way].valid = true;
					global_table[row][way].tag = this->u.global_tag;
					global_table[row][way].counter = taken ? 2 : 1;
				}

				// We just used this entry, so replace the other one next.
				replace_way[row] = 1 - way;

				history = ((history << 1) | (taken ? 1u : 0u)) & 16383u;
        }

};

//
// Complete Pentium M branch predictors for extra credit
// This class implements the complete Pentium M branch prediction units. 
// It implements both branch target prediction and branch outcome predicton. 
class cpm_update : public branch_update {
public:
        unsigned int index;
};

class cpm_predictor : public branch_predictor {
public:
        cpm_update u;

        cpm_predictor (void) {
        }

        branch_update *predict (branch_info & b) {
            u.direction_prediction (true);
            u.target_prediction (0);
            return &u;
        }

        void update (branch_update *u, bool taken, unsigned int target) {
        }

};


