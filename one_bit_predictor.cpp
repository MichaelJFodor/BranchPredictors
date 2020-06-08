/*
* COEN 313 Branch Predictor Project
* Task 1
*
* One-Bit Branch Predictor
* 32-entry Branch History Table (BHT)
*
*/

class my_update : public branch_update {
public:
	unsigned char state;
	unsigned int index;
	my_update(void) : state(0) {}
};

class my_predictor : public branch_predictor {
public:
	my_update u;
	branch_info bi;
	
	my_predictor (void) {}

	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
			unsigned char c = u.state;
			if(c >> 1 == 0) u.direction_prediction(false);
			else u.direction_prediction(true);
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			unsigned char *c = &((my_update*)u)->state;
			if (taken) {
				if (*c < 1) (*c)++;
			} else {
				if (*c > 0) (*c)--;
			}
		}
	}
};