#include <bits/stdc++.h>

using namespace std;
const int NR_SECONDS = 600 - 3;
const int testcases = 20;

set<pair<int, int>> edges;
vector<vector<int>> v, v_t, ctc, pie;
vector<int> used, ctc_ind, st, used_pie, not_piv;
int n, nr_ctc, m;
time_t c_start;
int max_moves, max_fail, theta, pie_counter;
double initial_temperature, temp_rate;
vector<int> current_configuration, best_configuration, candidates_nodes, phi;
vector<set<int>> in_degree, out_degree;
vector<pair<int, int>> candidate_list;
set<int> feedback_vertex_set;
vector<double> priority, probability;

void set_size() {

    nr_ctc = 0;
    pie_counter = 0;
    v.resize(n + 1, vector<int>());
    v_t.resize(n + 1, vector<int>());
    ctc.resize(n + 1, vector<int>());
    pie.resize(n + 1, vector<int>());
    in_degree.resize(n + 1, set<int>());
    out_degree.resize(n + 1, set<int>());
    used.resize(n + 1, 0);
    not_piv.resize(n + 1, 0);
    used_pie.resize(n + 1, 0);
    ctc_ind.resize(n + 1);
    priority.resize(n + 1);
    phi.resize(n << 1 | 1);
    probability.resize(n << 1 | 1);
}

void add_edge_SA(int x, int y) {
    edges.insert(make_pair(x, y));
    in_degree[y].insert(x);
    out_degree[x].insert(y);
}

int evaluate_move(pair<int, int> x) {
    int node = x.first, pos = x.second, ret = -1;
    for (int i = 0; i < current_configuration.size(); ++i) {
        if (i <= pos) {
            ret += edges.count(make_pair(node, current_configuration[i]));
        } else {
            ret += edges.count(make_pair(current_configuration[i], node));
        }
    }
    return ret;
}

void create_candidate_list() {
    candidate_list.clear();
    set<int> not_in_configuration;
    for (auto i : candidates_nodes)
        not_in_configuration.insert(i);
    for (auto it : current_configuration)
        not_in_configuration.erase(it);
    for (auto it : not_in_configuration) {
        int minus_degree = 1, plus_degree = (int) current_configuration.size() + 1;
        for (int i = 0; i < current_configuration.size(); ++i) {
            auto it2 = current_configuration[i];
            if (in_degree[it].count(it2)) {
                minus_degree = i + 1;
            }
            if (out_degree[it].count(it2)) {
                plus_degree = i;
            }
        }
        candidate_list.emplace_back(it, minus_degree);
        candidate_list.emplace_back(it, plus_degree);
    }
}

int evaluate_candidate(int sz) {
    return (int) candidates_nodes.size() - sz;
}

void apply_move(pair<int, int> move) {
    set<int> to_be_erased;
    for (int i = 0; i < current_configuration.size(); ++i) {
        if ((i < move.second - 1 && edges.count(make_pair(move.first, current_configuration[i]))) ||
            (i >= move.second - 1 && edges.count(make_pair(current_configuration[i], move.first)))) {
            to_be_erased.insert(current_configuration[i]);
        }
    }
    vector<int> new_configuration;
    for (int i = 0; i <= current_configuration.size(); ++i) {
        if (i + 1 == move.second) {
            new_configuration.emplace_back(move.first);
        }
        if (i < current_configuration.size()) {
            if (to_be_erased.count(current_configuration[i])) continue;
            new_configuration.emplace_back(current_configuration[i]);
        }
    }
    current_configuration = new_configuration;
}


void run() {
    max_moves = 100;
    max_fail = 10;
    initial_temperature = 0.6;
    temp_rate = 0.99;
    theta = 5;
    int count_moves, count_fails = 0;
    bool failure;
    do {
        count_moves = 0;
        failure = true;
        do {
            create_candidate_list();
            sort(candidate_list.begin(), candidate_list.end(),
                 [](pair<int, int> a, pair<int, int> b) { return priority[a.first] < priority[b.first]; });
            double all_phi = 0;
            for (int i = 0; i < candidate_list.size(); ++i) {
                phi[i] = ceil((double) i / (double) theta) + 1;
                all_phi += phi[i];
            }
            double p = (double) rand() / (RAND_MAX);
            pair<int, int> best_move_;
            for (int i = 0; i < candidate_list.size(); ++i) {
                probability[i] = phi[i] / all_phi;
                p -= probability[i];
                if (p <= 0) {
                    best_move_ = candidate_list[i];
                    break;
                }
            }
            if (candidate_list.empty())
                break;
            int eval = evaluate_move(best_move_);
            if (eval <= 0 || pow(M_E, -eval / initial_temperature) >= ((double) rand() / (RAND_MAX))) {
                apply_move(best_move_);
                count_moves++;
                if (evaluate_candidate(current_configuration.size()) <
                    evaluate_candidate(best_configuration.size())) {
                    best_configuration = current_configuration;
                    failure = false;
                }
            }
            if ((double) (clock() - c_start) / CLOCKS_PER_SEC > NR_SECONDS) {
                return;
            }
        } while (count_moves != max_moves);
        if ((double) (clock() - c_start) / CLOCKS_PER_SEC > NR_SECONDS) {
            return;
        }
        if (failure) {
            count_fails++;
        } else {
            count_fails = 0;
        }
        initial_temperature *= temp_rate;
    } while (count_fails != max_fail);
}

set<int> get_result() {
    set<int> not_used;
    for (auto i : candidates_nodes) {
        not_used.insert(i);
    }
    for (auto it : best_configuration) {
        not_used.erase(it);
    }
    for (auto it : feedback_vertex_set) {
        not_used.insert(it);
    }
    return not_used;
}


vector<int> reduce_first_5(vector<int> nodes) {
    vector<int> nodes2;
    int sw = 1;
    while (sw) {
        for (auto i : nodes) { /// delete self loops
            if (!edges.count(make_pair(i, i)))
                continue;
            feedback_vertex_set.insert(i);
            for (auto it : in_degree[i]) {
                edges.erase(make_pair(it, i));
                out_degree[it].erase(i);
            }
            for (auto it : out_degree[i]) {
                edges.erase(make_pair(i, it));
                in_degree[it].erase(i);
            }
            in_degree[i].clear();
            out_degree[i].clear();
        }

        for (auto i : nodes) {      /// in-coming or out-going degree than erase it
            if (!in_degree[i].empty() && !out_degree[i].empty())
                continue;
            for (auto it : in_degree[i]) {
                edges.erase(make_pair(it, i));
                out_degree[it].erase(i);
            }
            for (auto it : out_degree[i]) {
                edges.erase(make_pair(i, it));
                in_degree[it].erase(i);
            }
            in_degree[i].clear();
            out_degree[i].clear();
        }
        for (auto i : nodes) { // bypass nodes
            if (edges.count(make_pair(i, i))) continue;
            if (in_degree[i].size() == 1) {
                int node = *in_degree[i].begin();
                for (auto it : out_degree[i]) {
                    add_edge_SA(node, it);
                }
                for (auto it : in_degree[i]) {
                    edges.erase(make_pair(it, i));
                    out_degree[it].erase(i);
                }
                for (auto it : out_degree[i]) {
                    edges.erase(make_pair(i, it));
                    in_degree[it].erase(i);
                }
                in_degree[i].clear();
                out_degree[i].clear();
            }
            if (out_degree[i].size() == 1) {
                int node = *out_degree[i].begin();
                for (auto it : in_degree[i]) {
                    add_edge_SA(it, node);
                }
                for (auto it : in_degree[i]) {
                    edges.erase(make_pair(it, i));
                    out_degree[it].erase(i);
                }
                for (auto it : out_degree[i]) {
                    edges.erase(make_pair(i, it));
                    in_degree[it].erase(i);
                }
                in_degree[i].clear();
                out_degree[i].clear();
            }
        }
        for (auto i : nodes) {
            if (!in_degree[i].empty())
                nodes2.emplace_back(i);
        }
        sw = nodes.size() != nodes2.size();
        nodes = nodes2;
        nodes2.clear();
    }
    return nodes;
}

vector<int> apply_CORE_opration(vector<int> nodes) {
    for (auto it : edges) {
        if (!edges.count(make_pair(it.second, it.first))) {
            not_piv[it.first] = 1;
            not_piv[it.second] = 1;
        }
    }
    sort(nodes.begin(), nodes.end(), [](const int i, const int j) {
        return in_degree[i].size() + out_degree[i].size() < in_degree[j].size() + out_degree[j].size();
    });

    for (auto it : nodes) {
        if (not_piv[it]) continue;
        set<int> clique;
        clique.insert(it);
        for (auto it2 : out_degree[it]) {
            clique.insert(it2);
        }
        int ok = 1;
        for (auto it2 : clique) {
            int correct = 1;
            for (auto it3 : out_degree[it2]) {
                if (clique.count(it3)) {
                    correct++;
                }
            }
            if (correct != clique.size()) {
                ok = 0;
            }
            not_piv[it2] = 1;
        }
        if (ok == 1) {
            for (auto it2 : clique) {
                feedback_vertex_set.insert(it2);
                for (auto it3 : in_degree[it2]) {
                    edges.erase(make_pair(it3, it2));
                    out_degree[it3].erase(it2);
                }
                for (auto it3 : out_degree[it2]) {
                    edges.erase(make_pair(it2, it3));
                    in_degree[it3].erase(it2);
                }
                in_degree[it2].clear();
                out_degree[it2].clear();
            }
            feedback_vertex_set.erase(it);
        }
    }
    vector<int> nodes2;
    for (auto i: nodes) {
        if (!feedback_vertex_set.count(i)) {
            nodes2.emplace_back(i);
        }
    }
    return nodes2;
}


void apply_DOME_operation(const vector<int> &nodes) {
    vector<pair<int, int>> to_be_erased;
    for (auto it : edges) {
        int dome = 1;
        for (auto node : in_degree[it.first]) {
            if (!edges.count(make_pair(it.first, node)) && !edges.count(make_pair(node, it.second))) {
                dome = 0;
                break;
            }
        }
        if (!dome) {
            dome = 1;
            for (auto node : out_degree[it.second]) {
                if (!edges.count(make_pair(node, it.second)) && !edges.count(make_pair(it.first, node))) {
                    dome = 0;
                    break;
                }
            }
        }
        if (dome) {
            to_be_erased.emplace_back(it);
        }
    }
    for (auto it : to_be_erased) {
        edges.erase(it);
    }
}

void reduce_graph() {
    vector<int> nodes;
    for (int i = 1; i <= n; ++i) {
        nodes.emplace_back(i);
    }
    cout << candidates_nodes.size() << ' ' << edges.size() << '\n';
    nodes = reduce_first_5(nodes);
    cout << nodes.size() << ' ' << edges.size() << '\n';
    nodes = apply_CORE_opration(nodes);
    nodes = reduce_first_5(nodes);
    apply_DOME_operation(nodes);
    nodes = reduce_first_5(nodes);
    candidates_nodes.clear();
    for (auto it : nodes) {
        priority[it] = -((double) in_degree[it].size() + out_degree[it].size() -
                         0.3 * abs((double) in_degree[it].size() - out_degree[it].size()));
        if (!feedback_vertex_set.count(it))
            candidates_nodes.emplace_back(it);
    }
}

void add_edge(int x, int y) {
    edges.insert(make_pair(x, y));
    v[x].emplace_back(y);
    v_t[y].emplace_back(x);
}

void dfs_pie(int nod) {
    used_pie[nod] = pie_counter;
    for (int i : out_degree[nod]) {
        if (!used_pie[i] && out_degree[i].count(nod)) {
            dfs_pie(i);
        }
    }
    pie[pie_counter].emplace_back(nod);
}

void dfs(int nod) {
    used[nod] = 1;
    for (int i : v[nod]) {
        if (!used[i]) dfs(i);
    }
    st.emplace_back(nod);
}

void dfs_t(int nod) {
    used[nod] = 2;
    for (int i : v_t[nod]) {
        if (used[i] != 2) dfs_t(i);
    }
    ctc[nr_ctc].emplace_back(nod);
}

void apply_PIE_opration() {
    int x, y;
    for (auto it : edges) {
        x = it.first;
        y = it.second;
        in_degree[y].insert(x);
        out_degree[x].insert(y);
    }
    for (int i = 1; i <= n; ++i) {
        if (!used_pie[i]) {
            ++pie_counter;
            dfs_pie(i);
        }
    }
    vector<pair<int, int>> temp;
    for (auto it : edges) {
        if (used_pie[it.first] == used_pie[it.second] && !edges.count(make_pair(it.second, it.first))) {
            temp.emplace_back(it);
        }
    }
    for (auto it : temp) {
        edges.erase(it);
        in_degree[it.first].erase(it.second);
        in_degree[it.second].erase(it.first);
    }
    temp.clear();
    for (auto it : edges) {
        x = it.first;
        y = it.second;
        in_degree[y].insert(x);
        out_degree[x].insert(y);
    }
    for (int i = 1; i <= n; ++i) if (!used[i]) dfs(i);
    while (!st.empty()) {
        int nod = st.back();
        if (used[nod] != 2) dfs_t(nod), ++nr_ctc;
        st.pop_back();
    }
    for (int i = 0; i < nr_ctc; ++i) {
        for (auto it : ctc[i]) {
            ctc_ind[it] = i + 1;
        }
    }
    for (int i = 1; i <= n; ++i) {
        for (auto it : v[i]) {
            if (ctc_ind[i] != ctc_ind[it]) {
                edges.erase(make_pair(i, it));
            }
        }
    }
}

void clear_sets() {
    edges.clear();
    v.clear();
    v_t.clear();
    ctc.clear();
    pie.clear();
    used.clear();
    used_pie.clear();
    ctc_ind.clear();
    in_degree.clear();
    out_degree.clear();
    st.clear();
    nr_ctc = 0;
    current_configuration.clear();
    best_configuration.clear();
    candidates_nodes.clear();
    phi.clear();
    in_degree.clear();
    out_degree.clear();
    candidate_list.clear();
    feedback_vertex_set.clear();
    priority.clear();
    probability.clear();
    not_piv.clear();
}

signed main() {

    srand(0);
    for (int t = 3; t <= 3; ++t) {
        c_start = clock();
        cout << "test " << t << " began\n";
        string path_in =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\correct-testcases\grader_test)" +
                to_string(t) +
                ".in";
        string path_out =
                R"(C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVSP-NNS-results\grader_test)" +
                to_string(t) +
                ".out";
        ifstream in(path_in);
        ofstream out(path_out);

        int x, y;
        in >> n >> m;
        set_size();
        for (int i = 1; i <= m; ++i) {
            in >> x >> y;
            add_edge(x, y);
        }
        for (auto it : edges) {
            x = it.first;
            y = it.second;
            in_degree[y].insert(x);
            out_degree[x].insert(y);
        }
        //apply_PIE_opration();
        reduce_graph();
        run();
        set<int> output = get_result();
        out << output.size() << '\n';
        for (auto it : output) out << it << ' ';
        out << '\n';
        in.close();
        out.close();
        cout << "test " << t << " finished\n";
        clear_sets();
    }

    return 0;
}