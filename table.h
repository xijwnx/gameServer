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
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"ʮ",L"��",L"ǧ",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"֪",L"��",L"��"
};
enum status {
	wait, response, over
};
class table
{
public:
	shared_ptr<player> plys[3];     //���
	deque<int> cdheap;              //�ƿ�
	hcard last;						//��ǰ���������
	bool last_is_J;					//��ǰ����������Ƿ�Ϊ��
	status sts;						//��ǰ����״̬	
	int turn=0;						//��ǰ���
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

