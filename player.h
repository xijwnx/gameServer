#pragma once
#include <map>
#include <vector>

using std::map;
using std::vector;

enum hcard {
	Yi, Er, San, Si, Wu, Liu, Qi, Ba,
	Jiu, Shi, Hua, Qian, Kong, Ji, Tu, Zi,
	Shang, Da, Ren, Ke, Zhi, Li, Bie
};

struct cdgroup {
	int num;
	int hua;
	hcard cd;
	cdgroup(hcard, int, int);
};
bool hu(map<hcard,int>,int);						//胡


class player
{
public:
	int cdnum=0;       //手牌数
	hcard Jmain=San;   //主经
	int tong=0;        //统次数
	hcard lastin=Yi;        //最新
	hcard lastout = Yi;     //
	bool lastin_is_hua=false; //最新操作牌是否为花
	bool lastout_is_hua = false; //最新操作牌是否为花
	map<hcard,int> mycds; //手牌
	map<hcard, int> hua={ {Yi,0},{San,0},{Wu,0},{Qi,0},{Jiu,0} };  //手牌花经数
	vector<cdgroup> dui;           //对
	vector<cdgroup> zhao;            //招
	vector<hcard> fan;				//贩
	
	player();
	void getcd(int);				//获得牌
	void getdui(hcard,bool,int=0);			//对
	void getzhao(hcard,bool,int=0);          //开招
	void getfan(hcard);				//开贩
	void ganta(hcard);				//赶踏
	void mv_tong();					//统
	void throwcd(hcard);			//弃牌
	
	//void printheap();
};

