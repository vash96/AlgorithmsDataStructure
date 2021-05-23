#ifndef _ASD_WORD_HPP_
#define _ASD_WORD_HPP_

#include <vector>
#include <iostream>
using namespace std;

/*
 * La struttura rappresenta la coppia (parola, vettore)
 * del problema di laboratorio.
 * Inoltre tengo due variabili statiche:
 * - SIGMA rappresenta la cardinalita' dell'alfabeto
 * - alphabet e' una sorta di mappa : [0, 256) -> [0, SIGMA)
 */
struct asd_word {
    static short SIGMA;
    static short alphabet[256];

    vector<int> vec;
    vector<short> word;

    asd_word() { }
    asd_word(vector<short>& w) : word(w) {
        vec.resize(SIGMA, -1);

        for(int ch: w)
            ++vec[alphabet[ch]];
    }

    void print() {
        for(auto ch: word)
            putchar(ch);
    }

    bool operator==(const asd_word& rhs) const {
        return word == rhs.word;
    }
};

short asd_word::SIGMA;
short asd_word::alphabet[256];

#endif
