#include "base_on_server.h"

#define OPT_USED

using namespace BaseOnServer;
using sec = std::chrono::duration<double>;
using clk = std::chrono::steady_clock;

u32 K = 2;
u32 L = 2;

u64 sol_cnt = 0;
u64 sol_bnd = 1000;
double time_bnd = 1000;

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

Set& extend(Graph const& g, Set& s) {
    u32 n = g.vrt_num;

    for (u32 i = 0; i < n; ++i) {
        if (!s.exist(i)) {
            s.add(i);
            if (sat(g, s)) continue;
            s.del(i);
        }
    }
    
    return s;
}

struct EnumAlmostCoro {
    enum State { Start, Resume, Finish };

    State cur;

    Graph const& g;
    Set const& s;
    u32 v;

    Set value;
    Set e, q;
    u32 n, m;
    Vec<u32> p;

    EnumAlmostCoro(Graph const& g_, Set const& s_, u32 v_):
        cur(Start), g(g_), s(s_), v(v_), m(0), n(0) {
        auto e1 = s - Set(g.edges[v]);
        auto e2 = s - Set(g.edges_r[v]);
        auto n1 = std::min(K - 1, u32(e1.data.size()));
        auto n2 = std::min(L - 1, u32(e2.data.size()));
        n = n1 + n2;
        e = e1 + e2;
        n = std::min(n, u32(e.data.size()));
        m = n + 1;
        value = s - e;
        value.add(v);
    }

    bool operator()() {
        if (cur == Finish) return false;
        if (cur == Resume) goto CoroResume;
        cur = Resume;

        while (m --> 0) {
            p.clear();
            while (e.next_pick(m, p, q.data)) {
                value = value + q;
                if (sat(g, value)) {
                    return true;
                CoroResume:;
                }
                value = value - q;
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

    if (sol_cnt >= sol_bnd || sec(T - tik).count() >= time_bnd) {
        tok = T;

        std::cout << "---------------------\n"
            << "Total found (" << K << ", " << L << ")-plex(es): " << sol_cnt << "\n"
            << "Total runtime: " << sec(tok - tik).count() << "s\n"
            << "---------------------\n";

        std::exit(0);
    }
}

void enumAll(Graph& g, Set const& s, Set const& used, std::set<Set>& sol, bool output_flag = true) {
    Set new_used(used);
    for (u32 i = 0; i < g.vrt_num; ++i) {
        if (s.exist(i)) continue;
#ifdef OPT_USED
        if (used.exist(i)) continue;
#endif

        auto enumAlmost = EnumAlmostCoro(g, s, i);
        Set& t = enumAlmost.value;

        while (enumAlmost()) {
            Set& c = extend(g, t);

            if (sol.find(c) == sol.end()) {
                sol.insert(c);
                if (output_flag) print_and_check(c);
                // std::cout << "{" << used;
                enumAll(g, c, used, sol, !output_flag);
                // std::cout << "}" << used;
                if (!output_flag) print_and_check(c);
            }
#ifdef OPT_USED
            new_used.add(i);
#endif
        }
    }
}

i32 main(i32 argc, char* argv[]) {
    Str input_path = "../data/small_graph.txt";

    if (argc >= 2) input_path = Str(argv[1]);
    if (argc >= 3) {
        i32 tmp = sscanf(argv[2], "%llu", &sol_bnd);
        if (tmp != 1) {
            return -1;
        }
    }
    if (argc >= 4) {
        i32 tmp = sscanf(argv[3], "%lf", &time_bnd);
        if (tmp != 1) {
            return -1;
        }
    }
    if (argc >= 6) {
        i32 tmp1 = sscanf(argv[4], "%u", &K);
        i32 tmp2 = sscanf(argv[5], "%u", &L);
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

    enumAll(g, Set{}, Set{}, sol);

    tok = clk::now();

    std::cout << "---------------------\n"
        << "Reading file: " << input_path << "\n"
        << "Total found (" << K << ", " << L << ")-plex(es): " << sol_cnt << "\n"
        << "Total runtime: " << sec(tok - tik).count() << "s\n"
        << "---------------------\n";

    return 0;
}


