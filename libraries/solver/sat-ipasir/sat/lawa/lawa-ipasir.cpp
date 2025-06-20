//
// Created by balyo on 24.05.18.
//

#include "lawa.h"
extern "C" {
#include "ipasir.h"
}


const char * ipasir_signature () {
    return "Lawa - Lazy walksat local search solver version 0.1";
}

void * ipasir_init () {
    return new Lawa();
}

void ipasir_release (void * solver) {
    delete (Lawa*)solver;
}

void ipasir_add (void * solver, int lit_or_zero) {
    Lawa* lawa = (Lawa*)solver;
    lawa->addLiteral(lit_or_zero);
}

void ipasir_assume (void * solver, int lit) {
    Lawa* lawa = (Lawa*)solver;
    lawa->addAssumption(lit);
}

int ipasir_solve (void * solver) {
    Lawa* lawa = (Lawa*)solver;
    return lawa->search();
}

int ipasir_val (void * solver, int lit) {
    Lawa* lawa = (Lawa*)solver;
    return lawa->getValue(lit);
}

int ipasir_failed (void * solver, int lit) {
    // Lawa does not solve unsat formulas
    return 0;
}

void ipasir_set_terminate (void * solver, void * state, int (*terminate)(void * state)) {
    Lawa* lawa = (Lawa*)solver;
    lawa->setupTerminateCallback(state, terminate);
}

void ipasir_set_learn (void * solver, void * state, int max_length, void (*learn)(void * state, int * clause)) {
    // Does nothing, Lawa does not learn clauses
}
