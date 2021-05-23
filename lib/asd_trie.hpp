#ifndef _ASD_TRIE_HPP_
#define _ASD_TRIE_HPP_

#include <vector>
using namespace std;

/* Poco da dire, ogni nodo ha due figli (0,1) e un flag
 * per capire se qui termina una parola
 */
struct node_t
{
    bool isword;
    node_t* children[2];

    node_t() : isword(false) {
        children[0] = nullptr;
        children[1] = nullptr;
    }

    ~node_t() {
        if(children[0])
            delete children[0];
        if(children[1])
            delete children[1];
    }
};

/*
 * asd_trie e' un trie ovvero una struttura dati
 * ad albero per memorizzare stringhe.
 * La struttura e' composta da nodi di tipo node_t e l'unico
 * metodo e' quello per aggiungere una parola.
 */
struct asd_trie
{
    int n_words;
    node_t* root;

    asd_trie() : n_words(0), root(new node_t) { }

    ~asd_trie() {
        if(root)
            delete root;
    }


    /*
     * add_word(word) aggiunge word al trie:
     * - se era gia' presente ritorna false
     * - altrimenti ritorna true e setta il rispettivo flag.
     *
     * L'algoritmo e' iterativo e scandendo la parola decide
     * come muoversi nell'albero (partendo dalla radice root).
     *
     * La parola viene vista come un enorme stringa binaria (per motivi di spazio)
     * e percio', ad ogni passo, devo andare a selezionare un determinato bit
     * tramite quella bislacca formula (in realta' e' molto easy da capire).
     *
     * Complessita' in tempo: Theta(|word|)
     * Infatti scandisco una e una sola volta la parola e il lavoro nel for
     * ha costo O(1).
     */
    bool add_word(const vector<short>& word) {
        node_t* r = root;
        for(int i=0; i<(int)word.size()*8; ++i) {
            short ch = (word[i/8] & (1<<(7-i%8))) >> (7-i%8);
            if(!r->children[ch])
                r->children[ch] = new node_t;
            r = r->children[ch];
        }

        if(r->isword)
            return false;
        else
            return r->isword = true;
    }

};

#endif
