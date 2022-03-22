#include <bits/stdc++.h>

using namespace std;

string PATH = R"(C:\Users\andre\OneDrive\Desktop\pace\exact_public)";

signed main() {
    for (int i = 1; i <= 199; ++i) {
        string path = PATH + "\\e_";
        if (i < 100) {
            path += "0";
        }
        if (i < 10) {
            path += "0";
        }
        path += to_string(i);
        ifstream in(path);
        string output_path = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\public_exact\grader_test)";
        output_path += to_string(i) + ".in";
        ofstream out(output_path);
        string s;
        int n, m, t;
        int edges = 0;
        while (getline(in, s)) {
            if (s.empty() || s[0] == '%') continue;
            istringstream is(s);
            is >> n >> m >> t;
            break;
        }
        out << n << ' ' << m << '\n';
        for (int j = 1; j <= n; ++j) {
            getline(in, s);
            if (!s.empty() && s[0] == '%') {
                j--;
                continue;
            }
            istringstream is(s);
            while (is >> t) {
                ++edges;
                out << j << ' ' << t << '\n';
            }
        }
        out.close();
        cout << i << ' ' << edges << ' ' << m << '\n';
        if (!edges) {
            ofstream out(output_path);
            out << "0 0\n";
            out.close();
        }
        in.close();
    }
    /*
    std::string s = "100 123 42";

     */
}