#include <bits/stdc++.h>

using namespace std;

const int testcases = 20;

int main() {
    int a, b, c, n, m;
    for (int i = 8; i <= 8; ++i) {
        string path_in =
                "C:\\Users\\andre\\OneDrive\\Desktop\\bachelor-testcases\\SA-DFVSP-NNS\\grader_test" + to_string(i) +
                ".in";
        string path_out =
                "C:\\Users\\andre\\OneDrive\\Desktop\\bachelor-testcases\\correct-testcases\\grader_test" + to_string(i) +
                ".in";
        ifstream in(path_in);
        ofstream out(path_out);
        in >> n >> m;
        out << n << ' ' << m << '\n';
        for (int i = 1; i <= m; ++i) {
            in >> a >> b >> c;
            out << a << ' ' << b << '\n';
        }
        in.close();
        out.close();
    }

    return 0;
}