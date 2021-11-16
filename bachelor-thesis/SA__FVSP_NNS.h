#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
#include <chrono>


using namespace std;
#ifndef BACHELOR_THESIS_SA__FVSP_NNS_H
#define BACHELOR_THESIS_SA__FVSP_NNS_H


class SA_FVSP_NNS {

private:
    time_t c_start;
    int max_moves{}, max_fail{}, n{};
    map<pair<int, int>, bool> edges;
    double initial_temperature{}, temp_rate{}, seconds;
    vector<int> current_configuration, best_configuration;
    vector<set<int>> in_degree, out_degree;
    vector<pair<int, int>> candidate_list;
    set<int> feedback_vertex_set;
    vector<int> candidates_nodes;

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

    [[nodiscard]] int evaluate_candidate(int sz) const {
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


public:


    explicit SA_FVSP_NNS(int n, time_t c_start, double seconds) :
            n(n), in_degree(n + 1, set<int>()), out_degree(n + 1, set<int>()), c_start(c_start), seconds(seconds) {}

    void add_edge(int x, int y) {
        edges[make_pair(x, y)] = true;
        in_degree[y].insert(x);
        out_degree[x].insert(y);
    }

    void reduce_graph() {
        set<int> to_be_erased;
        int sw = 1;
        vector<int> nodes, nodes2;
        for (int i = 1; i <= n; ++i) {
            nodes.emplace_back(i);
        }

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
                        add_edge(node, it);
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
                        add_edge(it, node);
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

        for (auto it : nodes) {
            candidates_nodes.emplace_back(it);
        }
    }


    void run(int max_move, int max_failure, double init_temp, double tmp_rate) {
        max_moves = max_move;
        max_fail = max_failure;
        initial_temperature = init_temp;
        temp_rate = tmp_rate;
        int count_moves, count_fails = 0;
        bool failure;
        do {
            count_moves = 0;
            failure = true;
            do {
                create_candidate_list();
                if (candidate_list.empty())
                    break;
                pair<int, int> best_move_;
                best_move_ = candidate_list[rand() % candidate_list.size()];
                int eval = evaluate_move(best_move_);
                if (eval <= 0 || pow(M_E, -eval / initial_temperature) >= ((double) rand() / (RAND_MAX))) {
                    apply_move(best_move_);
                    count_moves++;
                    if (evaluate_candidate(current_configuration.size()) <
                        evaluate_candidate(best_configuration.size())) {
                        best_configuration = current_configuration;
                        failure = false;
                        cout << best_configuration.size() << '\n';
                    }
                }
                if ((double) (clock() - c_start) / CLOCKS_PER_SEC > seconds) {
                    return;
                }
            } while (count_moves != max_moves);
            if ((double) (clock() - c_start) / CLOCKS_PER_SEC > seconds) {
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

    set<int> result() {
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
        cout << best_configuration.size() << '\n';
        return not_used;
    }

};


#endif //BACHELOR_THESIS_SA__FVSP_NNS_H
