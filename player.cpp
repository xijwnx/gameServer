#include "player.h"
#include <iostream>
#include <algorithm>

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
	int jnum = c + w;
	cdgroup x(cd, 3, jnum);
	dui.push_back(x);
	mycds[cd] -= 2;
	hua[cd] -= c;
	if (mycds[cd] == 0) mycds.erase(cd);
	cdnum -= 2;
}

void player::getzhao(hcard cd,bool w,int c)
{
	int jnum = c + w;
	cdgroup x(cd, 4, jnum);
	zhao.push_back(x);
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

void player::ganta(hcard hcd)
{
	fan.push_back(hcd);
	zhao.erase(find_if(zhao.begin(), zhao.end(), [hcd](cdgroup x) {return x.cd == hcd; }));
	mycds.erase(hcd);
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

bool hu(map<hcard, int> hand, int tong)
{
	return false;
}
