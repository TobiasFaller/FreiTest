/**
* Lawa - Lazy Walksat Local search
* Author: Tomas Balyo (tomas.balyo@kit.edu)
* Karlsruhe Institure of Technology
*/

#include "lawa.h"
#include <cstdio>
#include <cstdlib>
#include <unordered_set>

Clause makeClause(vector<int> lits) {
    Clause c;
    c.lits = lits;
    return c;
}

void printClause(const Clause& cls) {
    for (int li = 0; li < cls.lits.size(); li++) {
        printf("%d ", cls.lits[li]);
    }
    printf("0 \n");
}

int Lawa::computeMakeScore(int lit) {
    int score = 0;
    vector<int>& occList = lit > 0 ? posOccList[lit] : negOccList[-lit];
    for (int cid : occList) {
        if (clauses[cid].satLits == 0) {
            score++;
        }
    }
    return score;
}

int Lawa::computeBreakScore(int lit) {
    int score = 0;
    vector<int>& occList = lit > 0 ? negOccList[lit] : posOccList[-lit];
    for (int cid : occList) {
        if (clauses[cid].satLits == 1) {
            score++;
        }
    }
    return score;
}

void Lawa::flipLiteral(int lit) {
    int var = abs(lit);
    vector<int>& decOccList = lit > 0 ? negOccList[var] : posOccList[var];
    vector<int>& incOccList = lit > 0 ? posOccList[var] : negOccList[var];

    for (int cid : decOccList) {
        clauses[cid].satLits--;
        if (clauses[cid].satLits == 0) {
            unsatClauseIds.push_back(cid);
        }
    }
    for (int cid : incOccList) {
        clauses[cid].satLits++;
    }

    tvalues[var] = lit;
}

void Lawa::deallocateMemory() {
    if (tvalues != nullptr) {
        delete[] tvalues;
        delete[] posOccList;
        delete[] negOccList;
    }
}

void Lawa::addLiteral(int lit) {
    int var = abs(lit);
    if (var > numVariables) {
        numVariables = var;
    }
    if (lit == 0) {
        clauses.push_back(makeClause(currentAddedClause));
        currentAddedClause.clear();
    } else {
        currentAddedClause.push_back(lit);
    }
    clausesAdded = true;
}

void Lawa::addAssumption(int lit) {
    assumptions.push_back(lit);
}

int Lawa::getValue(int lit) {
    return tvalues[abs(lit)];
}

void Lawa::printModel() {
    for (int var = 1; var <= numVariables; var++) {
        printf("%d ", tvalues[var]);
    }
    printf("\n");
}

void Lawa::printDebug() {
    for (int ci = 0; ci < clauses.size(); ci++) {
        printf("%d: ",ci);
        printClause(clauses[ci]);
    }
    printModel();
}

Lawa::~Lawa() {
    deallocateMemory();
}

bool Lawa::initializeSearch() {
    if (clausesAdded) {
        deallocateMemory();
        tvalues = new int[numVariables+1];
        posOccList = new vector<int>[1+numVariables];
        negOccList = new vector<int>[1+numVariables];

        // compute occurrence lists
        for (int ci = 0; ci < clauses.size(); ci++) {
            for (int li = 0; li < clauses[ci].lits.size(); li++) {
                int lit = clauses[ci].lits[li];
                int var = abs(lit);
                if (lit > 0) {
                    posOccList[var].push_back(ci);
                } else {
                    negOccList[var].push_back(ci);
                }
            }
        }
    }
    clausesAdded = false;

    for (int var = 1; var <= numVariables; var++) {
        tvalues[var] = rand()%2 == 0 ? var : -var;
    }

    unordered_set<int> lockedVariables;
    for (int lit : assumptions) {
        int var = abs(lit);
        tvalues[var] = lit;
        lockedVariables.insert(var);
    }
    assumptions.clear();

    for (int ci = 0; ci < clauses.size(); ci++) {
        Clause& cls = clauses[ci];

        vector<int> lockedLits;
        vector<int> freeLits;
        for (int lit : cls.lits) {
            if (lockedVariables.count(abs(lit))) {
                lockedLits.push_back(lit);
            } else {
                freeLits.push_back(lit);
            }
        }
        cls.flippableLits = freeLits.size();
        cls.lits = freeLits;
        cls.lits.insert(cls.lits.end(), lockedLits.begin(), lockedLits.end());

        int satLits = 0;
        for (int lit : cls.lits) {
            if (ISTRUE(lit))
                satLits++;
        }
        if (cls.flippableLits == 0 && satLits == 0) {
            // unsatisfiable formula
            return false;
        }
        if (satLits == 0) {
            unsatClauseIds.push_back(ci);
        }
        cls.satLits = satLits;
    }
    return true;
}

bool Lawa::unitPropagation() {
    //TODO
}

int Lawa::search() {
    if (!initializeSearch()) {
        return 20;
    }
    unsigned long round = 0;
    while (unsatClauseIds.size() > 0) {
        if (terminateCallback != nullptr) {
            if (terminateCallback(terminateCallbackData)) {
                return 0;
            }
        }
        round++;
        //printf("c round %lu, unsat clauses: %lu\n", round, unsatClauseIds.size());
        int usidid = rand() % unsatClauseIds.size();
        int usid = unsatClauseIds[usidid];
        unsatClauseIds[usidid] = unsatClauseIds.back();
        unsatClauseIds.pop_back();
        Clause& cls = clauses[usid];
        if (cls.satLits > 0) {
            continue;
        }
        //printf("c will satisfy clause ");
        //printClause(cls);

        if (cls.flippableLits == 1) {
            flipLiteral(cls.lits[0]);
            continue;
        }

        int id1 = rand() % cls.flippableLits;
        int id2 = rand() % cls.flippableLits;
        while(id1 == id2) {
            id1 = rand() % cls.flippableLits;
            id2 = rand() % cls.flippableLits;
        }
        int lit1 = cls.lits[id1];
        int lit2 = cls.lits[id2];

        int score1 = computeMakeScore(lit1) - computeBreakScore(lit1);// - flippedCount[abs(lit1)];
        int score2 = computeMakeScore(lit2) - computeBreakScore(lit2);// - flippedCount[abs(lit2)];

        if (score1 == score2) {
            //printf("c flipping %d and %d, their score: %d\n", lit1, lit2, score1);
            flipLiteral(lit1);
            flipLiteral(lit2);
        } else {
            int toFlip = score1 > score2 ? lit1 : lit2;
            //printf("c flipping %d, its score is max (%d, %d)\n", toFlip, score1, score2);
            flipLiteral(toFlip);
        }
    }
    printf("c searched finished after %lu rounds.\n", round);
    return 10;
}

void Lawa::setupTerminateCallback(void * state, int (*terminate)(void * state)) {
    terminateCallback = terminate;
    terminateCallbackData = state;
}

