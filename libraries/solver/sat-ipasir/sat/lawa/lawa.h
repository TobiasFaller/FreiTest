//
// Created by balyo on 24.05.18.
//

#ifndef LAWA_LAWA_H
#define LAWA_LAWA_H

#include <vector>

using namespace std;

#define ISTRUE(lit) (tvalues[abs(lit)]==lit)

struct Clause {
    char flippableLits;
    char satLits;
    vector<int> lits;
};

Clause makeClause(vector<int> lits);
void printClause(const Clause& cls);

class Lawa {

private:
    int numVariables = 0;
    int* tvalues = nullptr;
    vector<Clause> clauses;

    vector<int>* posOccList = nullptr;
    vector<int>* negOccList = nullptr;
    vector<int> unsatClauseIds;

    vector<int> currentAddedClause;
    bool clausesAdded = true;
    vector<int> assumptions;

    int (*terminateCallback)(void* state) = nullptr;
    void* terminateCallbackData = nullptr;

    int computeMakeScore(int lit);
    int computeBreakScore(int lit);
    void flipLiteral(int lit);
    void deallocateMemory();
    void printDebug();
    bool initializeSearch();
    bool unitPropagation();

public:
    void addLiteral(int lit);
    void addAssumption(int lit);
    int getValue(int lit);
    void printModel();
    int search();
    void setupTerminateCallback(void * state, int (*terminate)(void * state));
    ~Lawa();
};

#endif //LAWA_LAWA_H
