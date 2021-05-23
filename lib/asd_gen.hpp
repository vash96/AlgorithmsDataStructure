#ifndef _ASD_GEN_HPP_
#define _ASD_GEN_HPP_

#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>
#include "asd_util.hpp"
using namespace std;

void gen_randomcase(int);
void gen_bestcase(int);
void gen_worstcase(int);
string bin_str(int);
int worstword(int,int,int,vector<string>&);

/*
 * Genera cn testi di n caratteri composti da parole
 * di lunghezza len da lettere scelte in ASCII[a..b]
 * cn, len, a, b sono parametri letti in ../util/info
 *
 * Il seed viene letto sempre in ../util/info e viene
 * aggiornato al termine dell'esecuzione.
 *
 * Complessita' in tempo: Theta(cn*n)
 */
void gen_randomcase(int n)
{
    ifstream info_in("../util/info");
    double lastseed;
    int cn, len, a, b;

    // Leggo i parametri
    info_in >> lastseed >> cn >> len >> a >> b;
    info_in.close();
    set_seed(lastseed);

    string PATH = "../input/test";
    string testname;
    ofstream out;

    // Produco cn test
    for(int numtest=0; numtest<cn; ++numtest) {
        testname = PATH + to_string(numtest);
        out.open(testname.data());

        // Ad ogni passo scrivo un carattere ed eventualmente uno spazio
        for(int i=0, ch; i<n; ++i) {
            ch = random_ab(a, b);
            out.put(ch);
            if((i+1)%len == 0 and i+1<n) {
                out.put(32);
                ++i;
            }
        }

        out.close();
    }

    // Aggiorno il seed in ../util/info
    out.open("../util/info");
    out << get_seed() << "\n" << cn << "\n" << len << "\n" << a << " " << b << "\n";
    out.close();
}

/*
 * Analogamente a gen_randomcase(), qui produco cn testi
 * di n caratteri con i soliti parametri in ../util/info.
 *
 * Il bestcase ce l'ho quando la complessita' di problem()
 * e' O(n) quindi mi basta costruire sqrt(n) parole lunghe O(1)
 * e per le considerazioni fatte nella relazione, il bestcase avra'
 * complessita' in tempo O(n) >= O(sqrt(n)^2).
 *
 * Tuttavia, per non riempire eccessivamente il testo di spazi, ho deciso
 * di riutilizzare le stesse parole in ordine sparso, tanto due parole
 * identiche contano come una sola.
 * Ad ogni passo scelgo randomicamente un indice k e stampo la stringa k.
 *
 * Complessita' in tempo: Theta(cn*n)
 */
void gen_bestcase(int n)
{
    ifstream info_in("../util/info");
    double lastseed;
    int cn, len, a, b, sqrtn = sqrt(n);

    // Leggo i parametri
    info_in >> lastseed >> cn >> len >> a >> b;
    info_in.close();
    set_seed(lastseed);

    string PATH = "../input/test";
    string testname;
    ofstream out;

    // Genero cn test
    for(int numtest=0; numtest<cn; ++numtest) {
        testname = PATH + to_string(numtest);
        out.open(testname.data());

        // Genero le sqrt(n) parole scegliendo in modo random le lettere
        vector<vector<int>> words;
        for(int i=0; i<sqrtn; ++i) {
            vector<int> word;
            for(int j=0; j<len; ++j)
                word.push_back(random_ab(a, b));
            words.push_back(word);
        }

        int nn = 0;
        while(nn+len < n) {
            int i = random_ab(0, sqrtn);
            for(auto ch: words[i])
                out.put(ch);
            out.put(32);
            nn += len+1;
        }
        while(nn < n) {
            out.put(32);
            ++nn;
        }

        out.close();
    }

    // Aggiorno il seed
    out.open("../util/info");
    out << get_seed() << "\n" << cn << "\n" << len << "\n" << a << " " << b << "\n";
    out.close();
}

/*
 * Funzione per generare una stringa binaria a partire da un interi i
 * La stringa prodotta e' la rappresentazione in binario di i al rovescio
 * 
 * Complessita' in tempo: Theta(log(i)) = Theta(1)
 */
string bin_str(int i)
{
    string ans;

    while(i > 0) {
        ans.push_back(i%2 + '0');
        i /= 2;
    }

    return ans;
}

/*
 * worstword produce una stringa binaria a partire da i e decide
 * se puo' inserirla nel testo (se non e' troppo lunga).
 * In caso affermativo la inserisce.
 *
 * Il valore di ritorno e' la lunghezza della stringa se viene inserita,
 * 0 altrimenti.
 *
 * Complessita' in tempo: Theta(log(i)) = Theta(1)
 */
int worstword(int i, int nn, int n, vector<string>& words)
{
    string word = bin_str(i);
    if(nn+(int)word.length() < n) {
        words.push_back(word);
        return word.length();
    }else
        return 0;
}

/*
 * Dalle considerazioni fatte nella relazione, il caso pessimo
 * per problem() lo trovo quando tutte le parole di un testo sono distinte.
 *
 * gen_worstcase(n) produce quindi cn testi di n caratteri dove posso trovare
 * il massimo numero di parole distinte che posso generare da un alfabeto {0,1}.
 *
 * Ho scelto un alfabeto così semplice perche' era immediata la costruzione del worstcase,
 * infatti mi basta generare tutti i numeri binari da 1 ad un certo valore.
 *
 * Essendo che questa tecnica produce sempre le stesse stringhe, per rendere piu'
 * casuale il tutto riordino randomicamente la lista di queste stringhe binarie.
 * Per farlo utilizzo la funzione gia' implementata dal C++ random_shuffle(), di costo lineare.
 * Scrivere random_shuffle non e' difficile ma dato che era gia' pronta non vedo perche' non approfittarne!
 *
 * Complessita' in tempo: Theta(cn*n)
 */
void gen_worstcase(int n)
{
    ifstream info_in("../util/info");
    int cn, dump;

    // Leggo i parametri (mi interessa solo cn)
    info_in >> dump >> cn;
    info_in.close();

    string PATH = "../input/test";
    string testname;
    ofstream out;

    // Produco le stringhe binarie distinte
    vector<string> words;
    for(int nn=0, i=1; ; ++i) {
        int check = worstword(i, nn, n, words);
        if(check>0)
            nn += 1+check;
        else
            break;
    }


    // Per ogni test applico random_shuffle e stampo le stringhe in quell'ordine
    for(int numtest=0; numtest<cn; ++numtest) {
        testname = PATH + to_string(numtest);
        out.open(testname.data());

        random_shuffle(words.begin(), words.end());
        int nn = 0;
        for(auto& word: words) {
            out << word << " ";
            nn += word.length()+1;
        }
        while(nn < n) {
            out.put(32);
            ++nn;
        }

        out.close();
    }

    out.close();
}

#endif
