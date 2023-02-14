#include "base_on_server.h"

using namespace BaseOnServer;
using sec = std::chrono::duration<double>;
using clk = std::chrono::steady_clock;

u32 K = 2;
u32 L = 2;

u64 sol_cnt = 0;
u64 sol_bnd = 1000;

std::chrono::time_point<clk> T, tik, tok;

bool sat(Graph const& g, Set const& s) {
    u32 n = s.data.size();
    auto f = [&s](u32 x) { return s.exist(x); };

    for (auto &v : s.data) {
        auto &es = g.edges[v];
        auto &es_r = g.edges_r[v];
        u32 out = std::count_if(es.begin(), es.end(), f);
        u32 in = std::count_if(es_r.begin(), es_r.end(), f);

        if (((out + K) < n) || ((in + L) < n)) return false;
    }

    return true;
}

Set extend(Graph const& g, Set const& s) {
    Set ret = s;
    u32 n = g.vrt_num;

    for (u32 i = 0; i < n; ++i) {
        if (!ret.exist(i)) {
            ret.add(i);
            if (sat(g, ret)) continue;
            ret.del(i);
        }
    }
    
    return ret;
}

struct EnumAlmostCoro {
    enum State { Start, Resume, Finish };

    State cur;

    Graph const& g;
    Set const& s;
    u32 v;

    std::set<Set> save;
    Set value;
    Set e1, e2, e;
    u32 n1, n2, n, m;
    Set p1, q1, p2, q2;

    EnumAlmostCoro(Graph const& g_, Set const& s_, u32 v_):
        cur(Start), g(g_), s(s_), v(v_), m(0), n1(0), n2(0), n(0) {
        e1 = s - Set(g.edges[v]);
        e2 = s - Set(g.edges_r[v]);
        n1 = std::min(K - 1, u32(e1.data.size()));
        n2 = std::min(L - 1, u32(e2.data.size()));
        n = n1 + n2;
        e = e1 + e2;
        n = std::min(n, u32(e.data.size()));
    }
    
    bool operator()() {
        if (cur == Finish) return false;
        if (cur == Resume) goto CoroResume;
        cur = Resume;
         
        while (e.next_pick(n, p1.data, q1.data)) {
            m = -1;
            while (++m <= n) {
                p2.data.clear();
                while (q1.next_pick(m, p2.data, q2.data)) {
                    value = s - (e - (q1 - q2)) + v;
                    if (sat(g, value) && save.find(value) == save.end()) {
                        save.insert(value);
                        return true;
                    CoroResume:;
                    }
                }
            }
        }
        cur = Finish;
        return false;
    }
};

void print_and_check(Set const& c) {
    ++sol_cnt;
    std::cout << "[" << sec(clk::now() - T).count() << "s] No." << sol_cnt << " solution: " << c;
    T = clk::now();

    if (sol_cnt >= sol_bnd) {
        tok = T;

        std::cout << "---------------------\n"
            << "Total found (" << K << ", " << L << ")-plex(es): " << sol_cnt << "\n"
            << "Total runtime: " << sec(tok - tik).count() << "s\n"
            << "---------------------\n";

        std::exit(0);
    }
}

void enumAll(Graph& g, Set const& s, std::set<Set>& sol, bool output_flag = true) {
    for (u32 i = 0; i < g.vrt_num; ++i) {
        if (s.exist(i)) continue;

        auto enumAlmost = EnumAlmostCoro(g, s, i);
        Set& t = enumAlmost.value;

        while (enumAlmost()) {
            Set c = extend(g, t);

            if (sol.find(c) == sol.end()) {
                sol.insert(c);
                if (output_flag) print_and_check(c);
                enumAll(g, c, sol, !output_flag);
                if (!output_flag) print_and_check(c);
            }
        }
    }
}

i32 main(i32 argc, char* argv[]) {
    Str input_path = "../data/small_graph.txt";

    if (argc >= 1) input_path = Str(argv[1]);
    if (argc >= 2) {
        i32 tmp = sscanf(argv[2], "%llu", &sol_bnd);
        if (tmp != 1) {
            return -1;
        }
    }
    if (argc >= 4) {
        i32 tmp1 = sscanf(argv[3], "%u", &K);
        i32 tmp2 = sscanf(argv[4], "%u", &L);
        if (tmp1 != 1 || tmp2 != 1) {
            return -1;
        }
    }

    std::ifstream input_stream(input_path);
    Graph g;
    std::set<Set> sol;

    input_stream >> g;
    input_stream.close();

    tik = T = clk::now();

    enumAll(g, Vec<u32>{}, sol);

    tok = clk::now();

    std::cout << "---------------------\n"
        << "Reading file: " << input_path << "\n"
        << "Total found (" << K << ", " << L << ")-plex(es): " << sol_cnt << "\n"
        << "Total runtime: " << sec(tok - tik).count() << "s\n"
        << "---------------------\n";

    return 0;
}
