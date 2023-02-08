#include "base_on_server.h"

using namespace BaseOnServer;

u32 const K = 2;
u32 const L = 2;
u32 const BIT_LEN = 8192;

u64 bcnt = 0;

using Bset = std::bitset<BIT_LEN>;

Graph g;
Vec<Bset> b_edges, b_edges_r;

std::ostream& operator<<(std::ostream& out, Bset& b) {
	for (u32 i = 0; i < g.vrt_num; ++i) if(b[i]) {
		out << i << " ";
	}
	out << "\n";
	return out;
}

bool bnext(Bset& b) {
	bool carry = true;
	bool next = false;
	for (u32 i = 0; i < g.vrt_num; ++i) {
		next = b[i] ^ carry;
		carry = b[i];
		b[i] = next;
		if (!carry) return true;
	}
	return false;
}

bool bsat(Bset& b) {
	auto n = b.count();
	for (u32 i = 0; i < g.vrt_num; ++i) if (b[i]) {
		u32 k = (b_edges[i] & b).count();
		u32 l = (b_edges_r[i] & b).count();
		if (k + K < n || l + L < n) return false;
	}
	return true;
}

bool bmaximal(Bset& b) {
	auto n = b.count();
	bool flag = false;
	for (u32 i = 0; i < g.vrt_num; ++i) if (!b[i]) {
		b.set(i);
		flag = bsat(b);
		b.reset(i);
		if (flag) return false;
	}
	
	return true;
}

void benum() {
	b_edges.resize(g.vrt_num);
	b_edges_r.resize(g.vrt_num);

	for (u32 i = 0; i < g.vrt_num; ++i) {
		for (auto& v : g.edges[i]) b_edges[i].set(v);
		for (auto& v : g.edges_r[i]) b_edges_r[i].set(v);
	}

	Bset cur;

	auto t = std::time(nullptr);

	while (bnext(cur)) {
		if (bsat(cur) && bmaximal(cur)) {
			++bcnt;
			std::cout << "[" << (std::time(nullptr) - t) << "s] " << "No." << bcnt << " solution: " << cur;
			t = std::time(nullptr);
		}
	}
}

int main() {
	Str input_path = "../data/SNAP_modified/Wiki-Vote.txt";
	std::ifstream input_stream(input_path);

	input_stream >> g;
	input_stream.close();

	auto tik = std::time(nullptr);

	benum();

	auto tok = std::time(nullptr);

	std::cout << "---------------------\n"
		<< "Total (" << K << ", " << L << ")-plex(es): " << bcnt << "\n"
		<< "Total runtime: " << (tok - tik) << "s\n"
		<< "---------------------";

	return 0;
}