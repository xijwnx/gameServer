#include "player.h"
#include <iostream>

cdgroup::cdgroup(hcard cd, int cdnum, int hnum) :cd(cd), num(cdnum), hua(hnum) {}

player::player()
{
}

void player::getcd(int b)
{
	int z = b / 5;
	int mod = b % 5;
	hcard cd = static_cast<hcard>(z);
	if ((cd == Yi || cd == San || cd == Wu || cd == Qi || cd == Jiu) && mod < 2) {
		hua[cd]++;
		lastin_is_hua = true;
	}
	else lastin_is_hua = false;
	if (mycds.find(cd) == mycds.end()) mycds[cd] = 1;
	else mycds[cd]++;
	lastin = cd;
	cdnum++;
}


void player::getdui(hcard cd,bool w,int c)
{
	c += w ? 1 : 0;
	cdgroup x(cd, 3, c);
	dui.push_back(x);
	mycds[cd] -= 2;
	hua[cd] -= c;
	if (mycds[cd] == 0) mycds.erase(cd);
	cdnum -= 2;
}

void player::getzhao(hcard cd,bool w,int c)
{
	c += w ? 1 : 0;
	cdgroup x(cd, 4, c);
	dui.push_back(x);
	mycds[cd] -= 3;
	hua[cd] -= c;
	if (mycds[cd] == 0) mycds.erase(cd);
	cdnum -= 3;
}

void player::getfan(hcard cd)
{
	fan.push_back(cd);
	mycds.erase(cd);
	cdnum -= 4;
}

void player::ganta(hcard cd)
{
	fan.push_back(cd);
	for (auto i = zhao.begin(); i != zhao.end(); i++) {
		if (i->cd == cd) {
			zhao.erase(i);
			break;
		}
	}
	mycds.erase(cd);
	cdnum -= 1;
}

void player::mv_tong()
{
	tong++;
}

void player::throwcd(hcard x)
{
	if (x == Yi || x == San || x == Wu || x == Qi || x == Jiu) {
		if (hua[x] == mycds[x]) {
			hua[x]--;
			lastout_is_hua = true;
		}
		else lastout_is_hua = false;
	}
	mycds[x]--;
	cdnum--;
	if (mycds[x] == 0) mycds.erase(x);
	lastout = x;
}


//void player::printheap()
//{
//	for (auto i : mycds) {
//		if (i.first == Yi || i.first == San || i.first == Wu || i.first == Qi || i.first == Jiu) {
//			for (int j = 0; j < i.second - hua[i.first]; j++) cout << name[i.first] << " ";
//			for (int j = 0; j < hua[i.first]; j++) cout << name[i.first] << "* ";
//			
//		}
//		else {
//			for (int j = 0; j < i.second; j++) cout << name[i.first] << " ";
//		}
//		cout << endl;
//	}
//	cout << endl;
//}

bool hu(map<hcard, int> hand, int tong)
{
	return false;
}
