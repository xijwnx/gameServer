#pragma once
#include <deque>
#include <vector>
#include <string>
#include <memory>
#include "player.h"
#include <string>

using std::deque;
using std::vector;
using std::string;
using std::shared_ptr;
using std::wstring;
const vector<wstring> name{
	L"乙",L"二",L"三",L"四",L"五",L"六",L"七",
	L"八",L"九",L"十",L"化",L"千",L"孔",L"己",
	L"土",L"子",L"上",L"大",L"人",L"可",L"知",L"礼",L"别"
};
enum status {
	wait, response, over
};
class table
{
public:
	shared_ptr<player> plys[3];     //玩家
	deque<int> cdheap;              //牌库
	hcard last;						//当前被打出的牌
	bool last_is_J;					//当前被打出的牌是否为经
	status sts;						//当前桌面状态	
	int turn=0;						//当前玩家
	int responsewho = 0;
	table();
	void inittable();
	int gettop();
	int getbuttom();
	void midheap(unsigned int);
	void sendcd();
	void pass();
	void gameover();
	//void printheap();
};

