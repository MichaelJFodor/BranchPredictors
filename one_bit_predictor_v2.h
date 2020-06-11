// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

class my_update : public branch_update {
public:
    unsigned int index;
};

class my_predictor : public branch_predictor {
public:
//TABLE_BITS can be changed to 5,6,7,8 to implement 32,64,128,256 entry BHT
#define TABLE_BITS    5
    my_update u;
    branch_info bi;
    unsigned int history;
    bool tab[1 << TABLE_BITS];

    my_predictor(void) : history(0) {
        memset(tab, 0, sizeof(tab));
    }

    branch_update *predict(branch_info &b) {
        bi = b;
        if (b.br_flags & BR_CONDITIONAL) {
            //following statement is used to calculate [6:2] bits of the branch address
            u.index = ((b.address >> 2) & ((1 << TABLE_BITS) - 1));
            u.direction_prediction(tab[u.index]);

        } else {
            u.direction_prediction(true);
        }
        u.target_prediction(0);
        return &u;
    }

    void update(branch_update *u, bool taken, unsigned int target) {
        if (bi.br_flags & BR_CONDITIONAL) {
            bool *c = &tab[((my_update *) u)->index];
            *c = taken;
        }
    }
};
