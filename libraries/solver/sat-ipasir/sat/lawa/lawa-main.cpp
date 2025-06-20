//
// Created by balyo on 24.05.18.
//

/**
* Lawa - Lazy Walksat Local search
* Author: Tomas Balyo (tomas.balyo@kit.edu)
* Karlsruhe Institure of Technology
*/
#include <cstdlib>
#include <cstdio>

#include "dimacs-parsing.h"

int main(int argc, char** argv) {
    printf("c This is lawa - lazy walksat local search satisfiability solver\n");
    printf("c USAGE: ./lawa <cnf-formula-in-dimacs-format> [assumptions...]\n");
    Lawa lawa;
    if (!loadSatProblem(argv[1], lawa)) {
        printf("ERROR: problem not loaded\n");
        return 1;
    }
    for (int i = 2; i < argc; i++) {
        lawa.addAssumption(atoi(argv[i]));
    }
    srand(2018);
    if (lawa.search() == 10) {
        printf("s SATISFIABLE\nv ");
        lawa.printModel();
    } else {
        printf("s UNSATISFIABLE\n");
    }
    return 0;
}
