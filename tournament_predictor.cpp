// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.
#include <iostream>
class my_update : public branch_update {
public:
	unsigned int gshare_index;
	unsigned int local_index;
};

class my_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH 15
#define TABLE_BITS 15
	my_update u;
	branch_info bi;
	unsigned int history;
	unsigned char pred_decider;
	
	unsigned char gshare_pred;
	unsigned char local_pred;
	unsigned char gshare_tab[1<<TABLE_BITS];
	unsigned char local_tab[1<<TABLE_BITS];

	my_predictor (void) : history(0) { 
		memset(gshare_tab, 0, sizeof(gshare_tab));
		memset(local_tab, 0, sizeof(local_tab));
	}

	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {		
		        // Get gshare index
			u.gshare_index = 
				(history << (TABLE_BITS - HISTORY_LENGTH))
				^ (b.address & ((1<<TABLE_BITS)-1));
			// Get gshare prediction from table
			gshare_pred = gshare_tab[u.gshare_index] >> 1;	
			
			// Get local index (similar to gshare, but no history XORing)
			u.local_index = (b.address & ((1<<TABLE_BITS)-1));

			// Get local prediction from table
			local_pred = local_tab[u.local_index] >> 1;

			// Use tournament counter to decide which prediction to use
			if(pred_decider >> 1)
				u.direction_prediction(gshare_pred);
			else 
				u.direction_prediction(local_pred);
			

		} else {
			u.direction_prediction (true);
		}

		u.target_prediction (0);
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			unsigned char *c = &gshare_tab[((my_update*)u)->gshare_index];
			if (taken) {
				if (*c < 3) { 
					(*c)++;	
				}
			} else {
				if (*c > 0) {
				       	(*c)--;
				}
			}
			history <<= 1;
			history |= taken;
			history &= (1<<HISTORY_LENGTH) - 1;

			c = &local_tab[((my_update*)u)->local_index];
			if (taken) {
				if (*c < 3) { 
					(*c)++;	
				}
			} else {
				if (*c > 0) {
				       	(*c)--;
				}
			}

			if(taken != (bool)local_pred && taken == (bool)gshare_pred){
				if(pred_decider < 3){
					pred_decider++;
				}
			}
			else if(taken == (bool) local_pred && taken != (bool)gshare_pred){
				if(pred_decider > 0){
					pred_decider--;
				}
			}

		}
	}
};
