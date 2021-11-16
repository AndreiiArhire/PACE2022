#include "SA__FVSP_NNS.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
const int NR_SECONDS = 600-3;
const int testcases = 20;

signed main() {
    for (int t = 1; t <= 1; ++t) {
        cout << "test " << t << " began\n";
        string path_in =
                "C:\\Users\\andre\\OneDrive\\Desktop\\bachelor-testcases\\correct-testcases\\grader_test" +
                to_string(t) +
                ".in";
        string path_out =
                "C:\\Users\\andre\\OneDrive\\Desktop\\bachelor-testcases\\SA-DFVSP-NNS-results\\grader_test" +
                to_string(t) +
                ".out";
        ifstream in(path_in);
        ofstream out(path_out);

        time_t c_start = clock();
        int n, m, x, y;
        in >> n >> m;
        SA_FVSP_NNS solver(n, c_start, NR_SECONDS);
        for (int i = 1; i <= m; ++i) {
            in >> x >> y;
            solver.add_edge(x, y);
        }
        solver.reduce_graph();
        solver.run(100, 10, 0.6, 0.99);
        set<int> output = solver.result();
        out << output.size() << '\n';
        for (auto it : output) out << it << ' ';
        out << '\n';
        in.close();
        out.close();
        cout << "test " << t << " finished\n";
    }

    return 0;
}
