#include <bits/stdc++.h>

using namespace std;

const int testcases = 20;

int main() {
    int a, b, c, n, m;
    for (int i = 21; i <= 40; ++i) {
        cout << i << '\n';
        string path_in =
                "C:\\Users\\andre\\OneDrive\\Desktop\\PACE2022\\brute-testcases\\grader_test" + to_string(i) +
                ".in";
        string path_out =
                "C:\\Users\\andre\\OneDrive\\Desktop\\PACE2022\\correct-testcases\\grader_test" + to_string(i) +
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