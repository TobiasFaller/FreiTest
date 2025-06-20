//
// Created by balyo on 24.05.18.
//

#ifndef LAWA_DIMACS_PARSING_H
#define LAWA_DIMACS_PARSING_H

#include "lawa.h"
#include <cctype>

int readNextNumber(FILE* f, int c) {
    while (!isdigit(c)) {
        c = fgetc(f);
    }
    int num = 0;
    while (isdigit(c)) {
        num = num*10 + (c-'0');
        c = fgetc(f);
    }
    return num;
}

void readLine(FILE* f) {
    int c = fgetc(f);
    while(c != '\n') {
        c = fgetc(f);
    }
}

bool loadSatProblem(const char* filename, Lawa& lawa) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return false;
    }
    int c = 0;
    bool neg = false;
    int clauseInd = 0;
    vector<int> tmpClause;
    while (c != EOF) {
        c = fgetc(f);

        // comment line
        if (c == 'c') {
            readLine(f);
            continue;
        }
        // problem lines
        if (c == 'p') {
            continue;
        }
        // whitespace
        if (isspace(c)) {
            continue;
        }
        // negative
        if (c == '-') {
            neg = true;
            continue;
        }

        // number
        if (isdigit(c)) {
            int num = readNextNumber(f, c);
            if (neg) {
                num *= -1;
            }
            neg = false;
            lawa.addLiteral(num);
        }
    }
    fclose(f);
    return true;
}

#endif //LAWA_DIMACS_PARSING_H
