#include "utils.h"

using namespace Utils;
using sec = std::chrono::duration<double>;
using clk = std::chrono::steady_clock;
using Bset = std::vector<u32>;

u32 K = 2;
u32 L = 2;

u64 bcnt = 0;
u64 bbnd = 1000;
double time_bnd = 1000;

std::chrono::time_point<clk> T, tik, tok;

Graph g;
Vec<Bset> b_edges, b_edges_r;

std::ostream& operator<<(std::ostream& out, Bset& b) {
	for (u32 i = 0; i < g.vrt_size; ++i) if(b[i]) {
		out << i << " ";
	}
	out << "\n";
	return out;
}

bool bnext(Bset& b) {
	bool carry = true;
	bool next = false;
	for (u32 i = 0; i < g.vrt_size; ++i) {
		next = b[i] ^ carry;
		carry = b[i];
		b[i] = next;
		if (!carry) return true;
	}
	return false;
}

u32 bcount(Bset const& b) {
	u32 ret = 0;
	for (u32 i = 0; i < g.vrt_size; ++i) {
		ret += b[i];
	}
	return ret;
}

u32 b_and_then_count(Bset const& a, Bset const& b) {
	u32 ret = 0;
	for (u32 i = 0; i < g.vrt_size; ++i) {
		ret += a[i] && b[i];
	}
	return ret;
}

bool bsat(Bset& b) {
	auto n = bcount(b);
	for (u32 i = 0; i < g.vrt_size; ++i) if (b[i]) {
		u32 k = b_and_then_count(b_edges[i], b);
		u32 l = b_and_then_count(b_edges_r[i], b);
		if (k + K < n || l + L < n) return false;
	}
	return true;
}

bool bmaximal(Bset& b) {
	auto n = bcount(b);
	bool flag = false;
	for (u32 i = 0; i < g.vrt_size; ++i) if (!b[i]) {
		b[i] = true;
		flag = bsat(b);
		b[i] = false;
		if (flag) return false;
	}
	
	return true;
}

void benum() {
	Bset cur;
	cur.resize(g.vrt_size);

	while (bnext(cur)) {
		if (bsat(cur) && bmaximal(cur)) {
			++bcnt;
			std::cout << "[" << sec(clk::now() - T).count() << "s] " << "No." << bcnt << " solution: " << cur;
			T = clk::now();
			if (bcnt >= bbnd || sec(T - tik).count() >= time_bnd) {
				return;
			}
		}
	}
}

void senum() {

}

void init() {
	b_edges.resize(g.vrt_size);
	b_edges_r.resize(g.vrt_size);

	for (u32 i = 0; i < g.vrt_size; ++i) {
		b_edges[i].resize(g.vrt_size);
		b_edges_r[i].resize(g.vrt_size);
		for (auto& v : g.from[i]) b_edges[i][v] = true;
		for (auto& v : g.to[i]) b_edges_r[i][v] = true;
	}
}

int main(i32 argc, char* argv[]) {
	Str input_path = "../data/SNAP_modified/Wiki-Vote.txt";

	if (argc >= 2) input_path = Str(argv[1]);
	if (argc >= 3) {
		i32 tmp = sscanf(argv[2], "%llu", &bbnd);
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

	input_stream >> g;
	input_stream.close();

	init();

	tik = T = clk::now();

	benum();

	tok = clk::now();

	std::cout << "---------------------\n"
		<< "Reading file: " << input_path << "\n"
		<< "Total found (" << K << ", " << L << ")-plex(es): " << bcnt << "\n"
		<< "Total runtime: " << sec(tok - tik).count() << "s\n"
		<< "---------------------\n";

	return 0;
}