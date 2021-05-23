#include <fstream>
#include <iostream>
#include "../lib/asd_gen.hpp"
using namespace std;
const double z = 2.33;

double problem();

double measure_test(int);
pair<double, double> measure_time(int,double);

int main(int argc, char** argv)
{
    int ORD = atoi(argv[1]);

    ifstream info_in("../util/info");
    double dump;
    int c;
    info_in >> dump >> c;
    info_in.close();

    ofstream out;
    int gap[4] = {1, 2, 3, 5};
    int N;

    out.open("randomcase");
    for(int ord=10; ord<=ORD; ord*=10) {
        for(int i=0; i<4; ++i) {
            N = gap[i]*ord;
            gen_randomcase(N);

            cerr << "Measuring case " << N << "\n";
            auto p = measure_time(c, N*(N/1000000.0));

            out << fixed << N << " " << p.first << " " << p.second << "\n";
        }
    }
    out.close();

    out.open("bestcase");
    for(int ord=10; ord<=ORD; ord*=10) {
        for(int i=0; i<4; ++i) {
            N = gap[i]*ord;
            gen_bestcase(N);
            
            cerr << "Measuring case " << N << "\n";
            auto p = measure_time(c, N/1000000.0);

            out << fixed << N << " " << p.first << " " << p.second << "\n";
        }
    }
    out.close();

/*    out.open("worstcase");
    for(int ord=10; ord<=ORD; ord*=10) {
        for(int i=0; i<4; ++i) {
            N = gap[i]*ord;
            gen_worstcase(N);

            cerr << "Measuring case " << N << "\n";
            auto p = measure_time(c, N*(N/1000000.0));

            out << fixed << N << " " << p.first << " " << p.second << "\n";
        }
    }
    out.close();
*/
    return 0;
}

double measure_test(int i)
{
    string input  = string("../input/test")  + to_string(i);
    string output = string("../output/test") + to_string(i);

    freopen(input.data(), "r", stdin);
    freopen(output.data(), "w", stdout);

    return problem();
}

pair<double, double> measure_time(int c, double D)
{
    double e, delta, stdev, sum2 = 0, cn = 0, t = 0;

    do {
        cerr << "Sono dentro!\n";
        for(int i=0; i<c; ++i) {
            double test = measure_test(i);
            t += test;
            sum2 += test*test;
        }
        cn += c;
        e = t/cn;
        stdev = sqrt(sum2/cn-e*e);
        delta = z*stdev/sqrt(cn);
    } while(delta>=D);

    return {e, delta};
}
