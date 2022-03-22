#include <bits/stdc++.h>
#include <chrono>

using namespace std;
const int NR = 1e5 + 10;
int c, k;
string s;
clock_t begin1;
vector<int> good[NR];
vector<int> bad[NR];
map<string, int> ingredients_count;
map<int, string> ingredients_count_reverse;
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
int pop_size = 1000, lg2;
vector<string> curr_pop;

string crossover(string parent1, string parent2) {
    string offspring;
    for (int i = 0; i < ingredients_count.size(); ++i) {
        offspring.push_back(rand() % 2 ? parent1[i] : parent2[i]);
    }
    for (int i = ingredients_count.size(); i < ingredients_count.size() + lg2; ++i) {
        offspring.push_back(rand() % 2 ? parent1[i] : parent2[i]);
    }
    offspring.push_back(rand() % 2 ? parent1[parent1.size() - 2] : parent1[parent1.size() - 1]);
    offspring.push_back(rand() % 2 ? parent2[parent2.size() - 2] : parent2[parent2.size() - 1]);
    if (offspring[offspring.size() - 2] == '0' && offspring[offspring.size() - 1] == '0') {
        if (rand() % 10 == 0) {
            offspring[offspring.size() - 2] = '1';
        }
    }
    if (offspring[offspring.size() - 2] == '1' && offspring[offspring.size() - 1] == '1') {
        offspring[offspring.size() - 2] = '0';
        offspring[offspring.size() - 1] = '0';
        for (int i = 0; i < ingredients_count.size(); ++i) {
            offspring[i] = ('1' - offspring[i]) ^ ('1' - offspring[ingredients_count.size() + i % lg2]) + '0';
        }
    }
    return offspring;
}

int fitness(string bitstring) {
    int ret = 0;
    for (int i = 1; i <= c; ++i) {
        int sw = 1;
        for (auto it : good[i]) {
            if (bitstring[it] == '0') {
                sw = 0;
                break;
            }
        }
        if (sw) {
            for (auto it : bad[i]) {
                if (bitstring[it] == '1') {
                    sw = 0;
                    break;
                }
            }
        }
        ret += sw;
    }
    return ret;
}

ofstream out("test.out");
ifstream in("test.in");
string first_string;

signed main() {

    seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(NULL);
    in >> c;
    for (int i = 1; i <= c; ++i) {
        in >> k;
        for (int j = 1; j <= k; ++j) {
            in >> s;
            if (!ingredients_count.count(s)) {
                ingredients_count[s] = ingredients_count.size() - 1;
                ingredients_count_reverse[ingredients_count.size() - 1] = s;
            }
            good[i].push_back(ingredients_count[s]);
        }
        in >> k;
        for (int j = 1; j <= k; ++j) {
            in >> s;
            if (!ingredients_count.count(s)) {
                ingredients_count[s] = ingredients_count.size() - 1;
                ingredients_count_reverse[ingredients_count.size() - 1] = s;
            }
            bad[i].push_back(ingredients_count[s]);
        }
    }
    for (int i = 0; i < ingredients_count.size(); ++i) {
        first_string.push_back('0');
    }
    in >> k;
    for (int i = 1; i <= k; ++i) {
        in >> s;
        first_string[ingredients_count[s]] = '1';
    }
    lg2 = log2(ingredients_count.size());
    for (int i = 1; i <= lg2; ++i) {
        first_string.push_back(rand() % 2 ? '1' : '0');
    }
    first_string.push_back('0');
    first_string.push_back('0');
    curr_pop.emplace_back(first_string);
    for (int i = 2; i <= pop_size; ++i) {
        string chromosome;
        for (int j = 1; j <= ingredients_count.size(); ++j) {
            chromosome.push_back('0' + rand() % 2);
        }
        for (int j = 1; j <= lg2; ++j) {
            chromosome.push_back('0' + rand() % 2);
        }
        for (int l = 1; l <= 2; ++l) {
            if (rand() % 10 == 0) {
                chromosome.push_back('1');
            } else {
                chromosome.push_back('0');
            }
        }
        curr_pop.emplace_back(chromosome);
    }

    vector<pair<string, int>> scores;
    int last = 0;
    vector<double> probability(pop_size + 2, 0);
    for (int i = 1; i <= pop_size; ++i) {
        probability[i] = ((double) (pop_size - i + 1)) /
                         ((double) (((double) (pop_size)) * (pop_size + 1) / 2));
    }
    for (int g = 1; g; ++g) {
        cout << g << ' ';
        vector<string> new_generation;
        for (int i = 0; i < pop_size; ++i) {
            new_generation.push_back(curr_pop[i]);
        }

        for (int i = 1; i <= pop_size; ++i) {
            int prt1 = 0;
            int prt2 = i - 1;
            new_generation.emplace_back(crossover(curr_pop[prt1], curr_pop[prt2]));
        }

        for (int i = 1; i <= 10000; ++i) {
            int prt1 = pop_size - 1, prt2 = pop_size - 1;
            double r = ((double) rand() / (RAND_MAX));
            for (int j = 1; j <= pop_size; ++j) {
                if (r <= probability[j]) {
                    prt1 = j - 1;
                    break;
                } else {
                    r -= probability[j];
                }
            }
            r = ((double) rand() / (RAND_MAX));
            for (int j = 1; j <= pop_size; ++j) {
                if (r <= probability[j]) {
                    prt2 = j - 1;
                    break;
                } else {
                    r -= probability[j];
                }
            }
            new_generation.emplace_back(crossover(curr_pop[prt1], curr_pop[prt2]));
        }
        scores.clear();
        for (auto &i : new_generation) {
            scores.emplace_back(i, fitness(i));
        }
        sort(scores.begin(), scores.end(), [](const pair<string, int> &ii, const pair<string, int> &jj) {
            return ii.second > jj.second;
        });
        curr_pop.clear();
        for (int i = 0; i < pop_size; ++i) {
            curr_pop.push_back(scores[i].first);
        }
        cout << scores[0].second << '\n';

        clock_t end1 = clock();
        long long elapsed_secs = double(end1 - begin1) / CLOCKS_PER_SEC;
        if (elapsed_secs / 300 != last) {
            last = elapsed_secs / 300;
            string s = "testD" + to_string(last);
            ofstream out2(s);
            vector<string> output;
            for (int i = 0; i < ingredients_count.size(); ++i) {
                if (scores[0].first[i] == '1') {
                    output.emplace_back(ingredients_count_reverse[i]);
                }
            }
            out2 << output.size() << ' ';
            for (auto it : output) {
                out2 << it << ' ';
            }
            out2.close();
        }

    }
    vector<string> output;
    for (int i = 0; i < ingredients_count.size(); ++i) {
        if (scores[0].first[i] == '1') {
            output.emplace_back(ingredients_count_reverse[i]);
        }
    }
    out << output.size() << ' ';
    for (auto it : output) {
        out << it << ' ';
    }

    return 0;
}