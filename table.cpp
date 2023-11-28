#include "table.h"
#include <ctime>
#include <memory>

using std::make_shared;

table::table()
{

}

void table::inittable()
{
	srand(time(0));
	vector<int> t(112, 0);
	cdheap.assign(112, 0);
	for (int i = 0; i < 112; i++) {
		int r = rand() % 112;
		while (t[r] != 0) {
			r = rand() % 112;
		}
		cdheap[i] = r;
		t[r] = 1;
	}
}

int table::gettop()
{
	int cd = cdheap.front();
	cdheap.pop_front();
	return cd;
}

int table::getbuttom()
{
	int cd = cdheap.back();
	cdheap.pop_back();
	return cd;
}

void table::midheap(unsigned int sd)
{
	srand(sd);
	int r = rand() % 90 + 10;
	deque<int> f(cdheap.begin(), cdheap.begin() + r);
	cdheap.assign(cdheap.begin() + r, cdheap.end());
	for (auto i : f) {
		cdheap.push_back(i);
	}
}

void table::gameover()
{
}

//void table::printheap()
//{
//	for (int i = 0; i < cdheap.size(); i++) {
//		if (i % 7 == 0 && i) cout << endl;
//		cout << name[cdheap[i] / 5] << '\t';
//		
//	}
//	cout << endl;
//}

void table::sendcd()
{
	for (int i = 0; i < 25; i++) {
		plys[0]->getcd(gettop());
		plys[1]->getcd(gettop());
		plys[2]->getcd(gettop());
	}
	plys[0]->getcd(gettop());
}

void table::pass()
{
	turn++;
	turn %= 3;
}
