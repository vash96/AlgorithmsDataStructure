#include <iostream> // input/output/def base
#include <vector> // vector
#include <string> // string
#include <chrono>
#include "../lib/asd_trie.hpp" // struttura dati asd_trie
#include "../lib/asd_word.hpp" // struttura dati asd_word
using namespace std;
using namespace chrono;

// Ridefinizioni perche' mi scoccio di scrivere tipi troppo lunghi
typedef vector<int>   v_int;
typedef vector<short> v_short;
typedef vector<bool>  v_bool;
typedef vector<vector<int>> graph;

double problem();
void read_input(v_short&);
void gen_alpha(v_short&);
void gen_words(v_short&,vector<asd_word>&);
void gen_graph(graph&,vector<asd_word>&);
void print_graph(graph&,vector<asd_word>&);
void topo_sort(graph&,v_int&);
void dfs(int,graph&,v_bool&,v_int&);
int  relation(int,int,vector<asd_word>&);
int  solver(graph&);
int  max_length(graph&,v_int&,v_int&);

/*
 * Funzione che accorpa un po' tutto, per pulizia:
 * - Leggo
 * - Riconosco l'alfabeto
 * - Listo le parole (distinte) del testo + i corrispondenti vettori delle frequencies
 * - Dalle parole al grafo
 * - Stampo l'output
 */
double problem()
{
    double t1 = clock(); 

    asd_word::SIGMA = 0;
    for(int i=0; i<256; ++i)
        asd_word::alphabet[i] = 0;

    v_short text;
    vector<asd_word> words;
    graph G;

    read_input(text);
    gen_alpha(text);
    gen_words(text, words);
    gen_graph(G, words);

    cout << solver(G) << "\n";
    print_graph(G, words);

    double t2 = clock();

    return (t2-t1)/CLOCKS_PER_SEC; //( duration_cast<duration<double>>(t2-t1) ).count();
}

/*
 * Leggo carattere per carattere.
 * Ho deciso di leggere gli ASCII come interi,
 * in quanto convertirli in char non mi avrebbe
 * aiutato e anzi mi avrebbe causato dei problemi
 * con la scelta delle variabili e delle funzioni
 * (dato che andiamo a pescare dagli extended).
 *
 * Complessita' in tempo: Theta(|T|).
 */
void read_input(v_short& text)
{
    short c = getchar();
    while(c != -1) {
        text.push_back(c);
        c = getchar();
    }
    text.pop_back(); // Elimino l'ultimo carattere perché su UNIX-like e' un "a capo" superfluo
}

/*
 * Semplicemente rileggo il testo e mi segno (true) i caratteri che trovo.
 * Dopodiche' li mappo in {0, ..., SIGMA-1} tramite alphabet[].
 *
 * Complessita' in tempo: Theta(|T|).
 */
void gen_alpha(v_short& text)
{
    v_bool vis(256, false);

    for(short ch: text)
        vis[ch] = true;

    for(int ch=0; ch<256; ++ch) {
        if(vis[ch])
           asd_word::alphabet[ch] = asd_word::SIGMA++;
    }
}

/*
 * L'idea e' di leggere e riconoscere le parole tramite
 * una macchina a (due) stati.
 * Ogni volta che una parola viene riconosciuta la aggiungo
 * alla lista delle parole se e solo se risulta non ancora scoperta.
 * Per capirlo sfrutto la struttura ad albero asd_trie.
 *
 * Complessita' in tempo: Theta(|T|).
 * Infatti leggo (dal testo) ogni carattere una sola volta.
 * Dopodiche', se formo una parola w, la aggiungo al TRIE tramite add_word
 * che ha complessita' Theta(|w|).
 * Ovviamente \sum{|w| : w \in T} <= |T|, quindi in tutto ho ancora Theta(|T|).
 *
 * Complessita' in spazio: O(|T|).
 * Banalmente la somma delle lunghezze di tutte le parole distinte e' <= |T|.
 */
void gen_words(v_short& text, vector<asd_word>& words)
{
    asd_trie trie_words;
    bool state = false;
    v_short current;

    for(auto& ch: text) {
        if(state == false) {
            if(ch != (short)' ') {
                current.push_back(ch);
                state = true;
            }
        }else {
            if(ch != (short)' ')
                current.push_back(ch);
            else {
                if(trie_words.add_word(current))
                    words.push_back({current});
                current.clear();
                state = false;
            }
        }
    }
    // Caso limite quando una parola termina sulla fine del testo (senza spazio)
    if(!current.empty()) {
        if(trie_words.add_word(current))
            words.push_back({current});
    }
}

/*
 * Confronto le |V|*(|V|+1)/2 coppie di parole distinte
 * e genero G=(V, E).
 *
 * Complessita' in tempo: Theta(|V|^2).
 * Ho Theta(|V|^2) confronti ed ogni confronto costa Theta(|Sigma|)=Theta(1).
 */
void gen_graph(graph& G, vector<asd_word>& words)
{
    int W = words.size();
    
    G.resize(W);
    for(int i=0; i<W; ++i) {
        for(int j=i+1; j<W; ++j) {
            int rel = relation(i, j, words);

            if(rel == +1)
                G[i].push_back(j);
            else if(rel == -1)
                G[j].push_back(i);
        }
    }
}

/*
 * La stampa del grafo e' una semplice lettura di nodi e archi.
 *
 * Complessita' in tempo: Theta(|V|+|E|)
 */
void print_graph(graph& G, vector<asd_word>& words)
{
    int W = words.size();

    cout << "digraph G_T {\n";
    for(int i=0; i<W; ++i) {
        cout << "\t" << i << " [label=\"";
        words[i].print();
        cout << "\"];\n";
    }

    for(int u=0; u<W; ++u) {
        for(auto v: G[u])
            cout << "\t" << u << " -> " << v << ";\n";
    }
    cout << "}\n";
}

/*
 * Topological sort del grafo implementato come da Cormen (circa)
 *
 * Complessita' in tempo: Theta(|V|+|E|)
 */
void topo_sort(graph& G, v_int& sorted)
{
    int W = G.size();
    v_bool vis(W, false);
    sorted.reserve(W);

    for(int u=0; u<W; ++u) {
        if(!vis[u])
            dfs(u, G, vis, sorted);
    }
}

/*
 * Implementazione classica della dfs con un push
 * in aggiunta per il Topological sort.
 */
void dfs(int u, graph& G, v_bool& vis, v_int& sorted)
{
    vis[u] = true;
    for(auto v: G[u]) {
        if(!vis[v])
            dfs(v, G, vis, sorted);
    }
    sorted.push_back(u);
}

/*
 * Relazione > tra i vettori-parola
 * -  0 se uguali
 * - +1 se >
 * - -1 se <
 *
 * Complessita' in tempo: Theta(|Sigma|)=Theta(1).
 */
int relation(int i, int j, vector<asd_word>& words)
{
    bool eq = true, ge = true, le = true;

    for(int k=0, ch_i, ch_j; k<asd_word::SIGMA; ++k) {
        ch_i = words[i].vec[k];
        ch_j = words[j].vec[k];

        if(ch_i != ch_j)
            eq = false;
        if(ch_i < ch_j)
            ge = false;
        if(ch_i > ch_j)
            le = false;
    }

    if(eq and ge and le)
        return 0;
    if(!eq and ge and !le)
        return +1;
    if(!eq and !ge and le)
        return -1;

    return 42;
}

/* Funzione che raggruppa la "soluzione" sul grafo:
 * - Topological sort
 * - Ricerca del longest path
 */
int solver(graph& G)
{
    vector<int> sorted, dist;

    topo_sort(G, sorted);
    return max_length(G, sorted, dist);
}

/*
 * Conoscendo il Topological sort posso calcolare
 * il cammino semplice piu' lungo rilassando gli archi
 * dai source ai sink.
 *
 * Complessita' in tempo: Theta(|V|+|E|).
 */
int max_length(graph& G, v_int& sorted, v_int& dist)
{
    int V = sorted.size();
    int MAX = 0;
    dist.resize(V, 0);
    
    for(int i=V-1, u; i>=0; --i) {
        u = sorted[i];
        MAX = max(MAX, dist[u]);
        for(auto v: G[u])
            dist[v] = max(dist[v], dist[u]+1);
    }

    return MAX;
}
