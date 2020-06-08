/*
* (4,2) Correlating Branch Predictor
* 32 entries for branch address
*
*/

class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	4	//m in (m,n)
#define TABLE_BITS	5	//number of bits used for address (2^5 = 32 addresses)
	my_update u;
	branch_info bi;
	unsigned int history;
	unsigned char tab[1<<TABLE_BITS];

	my_predictor (void) : history(0) 
	{ 
		memset (tab, 0, sizeof (tab));
	}

	// Function 
	branch_update *predict (branch_info & b) 
	{
		bi = b;
		// Conditional Branch
		if (b.br_flags & BR_CONDITIONAL) 
		{
			// XOR branch address and history of last m branches to find index of n-bit branch predictor
			u.index = (history << (TABLE_BITS - HISTORY_LENGTH)) ^ (b.address & ((1<<TABLE_BITS)-1));
			// Takes MSB of element of array as the prediction (either 1 or 0)
			u.direction_prediction (tab[u.index] >> 1);
		} 
		// Another other type of branch
		else 
			u.direction_prediction (true);
		u.target_prediction (0);

		return &u;
	}

	// Function updates array of branch predictors (2 bits for each element)
	void update (branch_update *u, bool taken, unsigned int target) 
	{
		// Conditional Branch
		if (bi.br_flags & BR_CONDITIONAL) 
		{
			unsigned char *c = &tab[((my_update*)u)->index];
			// Previous branch was taken
			if (taken) 
			{
				// Increments element at [index] unless element is already 2 (0b11)
				// Since each branch predictor is 2 bits wide, the largest value for each element is 2
				if (*c < 3) 
					(*c)++;
			}
			// Previous branch was not taken
			else 
			{
				// Decrements element at [index] unless element is already 0 (0b00)
				// Since each branch predictor is 2 bits wide, the smallest value for each element is 0
				if (*c > 0) 
					(*c)--;
			}
			
			// Updates branch history
			// Shifts history left 1 bit and appends the state of the most recent branch (1 or 0) to the LSB of history
			history <<= 1;
			history |= taken;
			history &= (1<<HISTORY_LENGTH)-1;
		}
	}
};
