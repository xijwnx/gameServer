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
bool hu(map<hcard,int>,int);						//��


class player
{
public:
	int cdnum=0;       //������
	hcard Jmain=San;   //����
	int tong=0;        //ͳ����
	hcard lastin=Yi;        //����
	hcard lastout = Yi;     //
	bool lastin_is_hua=false; //���²������Ƿ�Ϊ��
	bool lastout_is_hua = false; //���²������Ƿ�Ϊ��
	map<hcard,int> mycds; //����
	map<hcard, int> hua={ {Yi,0},{San,0},{Wu,0},{Qi,0},{Jiu,0} };  //���ƻ�����
	vector<cdgroup> dui;           //��
	vector<cdgroup> zhao;            //��
	vector<hcard> fan;				//��
	
	player();
	void getcd(int);				//�����
	void getdui(hcard,bool,int=0);			//��
	void getzhao(hcard,bool,int=0);          //����
	void getfan(hcard);				//����
	void ganta(hcard);				//��̤
	void mv_tong();					//ͳ
	void throwcd(hcard);			//����
	
	//void printheap();
};

