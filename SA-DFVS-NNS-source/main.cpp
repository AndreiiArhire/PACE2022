#include <bits/stdc++.h>

using namespace std;
const int NR_SECONDS = 20 - 3;
const int testcases = 20;

set<pair<int, int>> edges;
vector<vector<int>> ctc;
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

void add_edge(int x, int y) {
    /*O(lgN)*/
    edges.insert(make_pair(x, y));
    in_degree[y].insert(x);
    out_degree[x].insert(y);
}

void erase_edge(int x, int y) {
    /*O(lgN)*/
    edges.erase(make_pair(x, y));
    in_degree[y].erase(x);
    out_degree[x].erase(y);
}

void dfs_pie(int nod) {
    used_pie[nod] = pie_counter;
    for (int i : out_degree[nod]) {
        if (!used_pie[i] && in_degree[nod].count(i)) {
            dfs_pie(i);
        }
    }
}


void dfs(int nod) {
    used[nod] = 1;
    for (int i : out_degree[nod]) {
        if (!used[i] && !in_degree[nod].count(i)) dfs(i);
    }
    st.emplace_back(nod);
}

void dfs_t(int nod) {
    used[nod] = 2;
    for (int i : in_degree[nod]) {
        if (used[i] != 2 && !out_degree[nod].count(i)) dfs_t(i);
    }
    ctc_ind[nod] = nr_ctc;
    ctc[nr_ctc].emplace_back(nod);
}

bool erase_SCC_edges() {
    /*O((N+M)lgN)*/
    bool ret = false;
    nr_ctc = 0;
    ctc.clear();
    ctc.resize(n + 1, vector<int>());
    for (auto i : candidates_nodes) used[i] = 0;
    for (auto i : candidates_nodes) if (!used[i]) dfs(i);
    while (!st.empty()) {
        int nod = st.back();
        if (used[nod] != 2) dfs_t(nod), ++nr_ctc;
        st.pop_back();
    }
    vector<pair<int, int>> to_be_erased;
    for (auto it : edges) {
        if (edges.count(make_pair(it.second, it.first))) continue;
        if (ctc_ind[it.first] != ctc_ind[it.second]) {
            to_be_erased.emplace_back(it);
            ret = true;
        }
    }
    ret |= (bool) (to_be_erased.size());
    for (auto it : to_be_erased) {
        erase_edge(it.first, it.second);
    }
    return ret;
}

bool erase_self_loop_nodes() {
    /*O((N+M)lgN)*/
    bool ret = false;
    vector<int> new_candidates_nodes;
    for (auto i : candidates_nodes) {
        if (!edges.count(make_pair(i, i))) {
            new_candidates_nodes.emplace_back(i);
            continue;
        }
        vector<pair<int, int>> to_be_erased;
        feedback_vertex_set.insert(i);
        for (auto it : in_degree[i]) {
            to_be_erased.emplace_back(it, i);
        }
        for (auto it : out_degree[i]) {
            to_be_erased.emplace_back(i, it);
        }
        for (auto it : to_be_erased) {
            erase_edge(it.first, it.second);
        }
    }
    ret |= new_candidates_nodes.size() != candidates_nodes.size();
    candidates_nodes = new_candidates_nodes;
    return ret;
}

bool erase_0in_out_degree_nodes() {
    /*O((N+M)lgN)*/
    bool ret = false;
    vector<int> new_candidates_nodes;
    for (auto i : candidates_nodes) {
        if (!in_degree[i].empty() && !out_degree[i].empty()) {
            new_candidates_nodes.emplace_back(i);
            continue;
        }
        vector<pair<int, int>> to_be_erased;
        for (auto it : in_degree[i]) {
            to_be_erased.emplace_back(it, i);
        }
        for (auto it : out_degree[i]) {
            to_be_erased.emplace_back(i, it);
        }
        for (auto it : to_be_erased) {
            erase_edge(it.first, it.second);
        }
    }
    ret |= new_candidates_nodes.size() != candidates_nodes.size();
    candidates_nodes = new_candidates_nodes;
    return ret;
}

bool erase_1in_out_degree_nodes() {
    /*O((N+M)lgN)*/
    bool ret = false;
    vector<int> new_candidates_nodes;
    for (auto i : candidates_nodes) {
        if ((in_degree[i].size() != 1 && out_degree[i].size() != 1) || edges.count(make_pair(i, i))) {
            new_candidates_nodes.emplace_back(i);
            continue;
        }
        vector<pair<int, int>> to_be_erased;
        if (in_degree[i].size() == 1) {
            int node = *in_degree[i].begin();
            to_be_erased.emplace_back(node, i);
            for (auto it : out_degree[i]) {
                to_be_erased.emplace_back(i, it);
            }
            for (auto it : out_degree[i]) {
                add_edge(node, it);
            }
        }
        if (out_degree[i].size() == 1) {
            int node = *out_degree[i].begin();
            to_be_erased.emplace_back(i, node);
            for (auto it : in_degree[i]) {
                to_be_erased.emplace_back(it, i);
            }
            for (auto it : in_degree[i]) {
                add_edge(it, node);
            }
        }
        for (auto it : to_be_erased) {
            erase_edge(it.first, it.second);
        }
    }
    ret |= new_candidates_nodes.size() != candidates_nodes.size();
    candidates_nodes = new_candidates_nodes;
    return ret;
}

bool erase_CORE_nodes() {
    /*O(NlgN+M)*/
    bool ret = false;
    for (auto i : candidates_nodes) not_piv[i] = 0;
    for (auto it : edges) {
        if (!edges.count(make_pair(it.second, it.first))) {
            not_piv[it.first] = 1;
            not_piv[it.second] = 1;
        }
    }
    sort(candidates_nodes.begin(), candidates_nodes.end(), [](const int i, const int j) {
        return in_degree[i].size() + out_degree[i].size() < in_degree[j].size() + out_degree[j].size();
    });

    for (auto it : candidates_nodes) {
        if (not_piv[it]) continue;
        set<int> clique;
        clique.insert(it);
        for (auto it2 : out_degree[it]) {
            clique.insert(it2);
        }
        int is_core = 1;
        for (auto it2 : clique) {
            int sz = 1;
            for (auto it3 : out_degree[it2]) {
                if (clique.count(it3)) {
                    sz++;
                }
            }
            if (sz != clique.size()) {
                is_core = 0;
            }
            not_piv[it2] = 1;
        }
        vector<pair<int, int>> to_be_erased;
        if (is_core == 1) {
            for (auto it2 : clique) {
                feedback_vertex_set.insert(it2);
                for (auto it3 : in_degree[it2]) {
                    to_be_erased.emplace_back(it3, it2);
                }
                for (auto it3 : out_degree[it2]) {
                    to_be_erased.emplace_back(it2, it3);
                }
            }
            feedback_vertex_set.erase(it);
        }
        for (auto it2 : to_be_erased) {
            erase_edge(it2.first, it2.second);
        }
    }
    vector<int> new_candidates_nodes;
    for (auto i: candidates_nodes) {
        if (!feedback_vertex_set.count(i)) {
            new_candidates_nodes.emplace_back(i);
        }
    }
    ret |= candidates_nodes.size() != new_candidates_nodes.size();
    candidates_nodes = new_candidates_nodes;
    return ret;
}

bool erase_DOM_edges() {
    /*O(N*M)*/
    bool ret = false;
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
    ret |= (bool) (to_be_erased.size());
    for (auto it : to_be_erased) {
        edges.erase(it);
    }
    return ret;
}

void loop() {
    bool change = true;
    while (change) {
        change = false;
        change |= erase_self_loop_nodes();
        change |= erase_0in_out_degree_nodes();
        change |= erase_1in_out_degree_nodes();
    }
}

void contract_graph() {
    /*O(N(N+M)lgN*/
    bool change = true;
    for (int i = 1; i <= n; ++i) {
        candidates_nodes.emplace_back(i);
    }
    erase_SCC_edges();
    loop();
    erase_CORE_nodes();
    loop();
    //erase_DOM_edges();
    loop();
    for (auto it : candidates_nodes) {
        priority[it] = -((double) in_degree[it].size() + out_degree[it].size() -
                         0.3 * abs((double) in_degree[it].size() - out_degree[it].size()));
    }
}

int evaluate_move(pair<int, int> x) {
    /*O(NlgN)*/
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
    /*O(N^2lgN)*/
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
    /*O(1)*/
    return (int) candidates_nodes.size() - sz;
}

void apply_move(pair<int, int> move) {
    /*O(NlgN)*/
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
    /*O(N^3logN*maxMoves*maxFails)*/
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


void get_result() {
    /*O(NlgN)*/
    for (auto i : candidates_nodes) {
        feedback_vertex_set.insert(i);
    }
    for (auto it : best_configuration) {
        feedback_vertex_set.erase(it);
    }
}


void set_size() {
    /*O(N*lgN)*/
    nr_ctc = 0;
    pie_counter = 0;
    ctc.resize(n + 1, vector<int>());
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

void clear_sets() {
    /*O(N*lgN)*/
    edges.clear();
    ctc.clear();
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

void testcase(const string &p_in, const string &p_out) {
    ifstream in(p_in);
    ofstream out(p_out);
    int x, y;
    in >> n >> m;
    set_size();
    for (int i = 1; i <= m; ++i) {
        in >> x >> y;
        add_edge(x, y);
    }
    contract_graph();
    cout << candidates_nodes.size() << ' ' << edges.size() << '\n';
    run();
    get_result();
    out << feedback_vertex_set.size() << '\n';
    cout << "****" << feedback_vertex_set.size() << '\n';
    for (auto it : feedback_vertex_set) out << it << ' ';
    out << '\n';
    clear_sets();
    in.close();
    out.close();
}

signed main() {

    srand(0);
    string path_input = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\public-testcases\grader_test)";
    string path_output = R"(C:\Users\andre\OneDrive\Desktop\PACE2022\SA-DFVSP-NNS-results\grader_test)";
    for (int t = 1; t <= 1; ++t) {
        c_start = clock();
        cout << "test " << t << " began\n";
        testcase(path_input + to_string(t) + ".in", path_output + to_string(t) + ".out");
        cout << "test " << t << " finished\n";
    }

    return 0;
}
