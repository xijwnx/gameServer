#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cctype>
#include <thread>
#include <mutex>
#include <chrono>
#include <stdexcept>
#include <winsock2.h>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "player.h"
#include "table.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;
SOCKET clients[3];
enum msgtype { hand = '0', mydzf, nextdzf, enddzf, notis, throwncd, errorturn, errorop, statuss,win,loss };
constexpr int PORT = 12345;  // 服务器端口
table T;
std::mutex consoleMutex;
std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
std::string utf8_encode(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}
void broadcast(const wstring& s, msgtype mtp) {
	string nts;
	nts = converter.to_bytes(s);
	nts.insert(nts.begin(), static_cast<char>(mtp));
	send(clients[0], nts.c_str(), nts.size(), 0);
	send(clients[1], nts.c_str(), nts.size(), 0);
	send(clients[2], nts.c_str(), nts.size(), 0);
	cout << "send to all players : " << nts.size() << " bytes" << endl;
}

void gameover(int pid) {
	if (pid == -1) {
		broadcast(L"黄了！游戏结束！", loss);
	}
	else {
		wstring winp = L"恭喜您取得胜利！";
		wstring lossp = L"很遗憾，玩家" + to_wstring(pid) + L"取得胜利。";
		string str;
		str = converter.to_bytes(winp);
		str.insert(str.begin(), static_cast<char>(win));
		send(clients[pid], str.c_str(), str.size(), 0);
		str = converter.to_bytes(lossp);
		str.insert(str.begin(), static_cast<char>(loss));
		send(clients[(pid + 1) % 3], str.c_str(), str.size(), 0);
		send(clients[(pid + 2) % 3], str.c_str(), str.size(), 0);
	}
	//cout << "send to all players : " << str.size() << " bytes" << endl;
}

string dzfheap(table& t, int id) {
	wstring cds;
	auto& p = t.plys[id];
	for (auto& i : p->dui) {
		for (int j = 0; j < i.hua; j++) {
			cds += name[i.cd] + L"* ";
		}
		for (int j = 0; j < 3 - i.hua; j++) {
			cds += name[i.cd] + L" ";
		}
		cds += L"\r\n";
	}
	for (auto& i : p->zhao) {
		for (int j = 0; j < i.hua; j++) {
			cds += name[i.cd] + L"* ";
		}
		for (int j = 0; j < 4 - i.hua; j++) {
			cds += name[i.cd] + L" ";
		}
		cds += L"\r\n";
	}
	for (auto& i : p->fan) {
		cds += name[i] + L"* ";
		cds += name[i] + L"* ";
		cds += name[i] + L" ";
		cds += name[i] + L" ";
		cds += name[i] + L" ";
	}
	std::string utf8Message2 = converter.to_bytes(cds);
	//utf8Message2.insert(utf8Message2.begin(), static_cast<char>(mydzf));
	return utf8Message2;
}

string cdheap(table& t, int id) {
	wstring cds;
	map<hcard, int> cd(T.plys[id]->mycds);
	map<hcard, int> hcd(T.plys[id]->hua);
	int max_num = max_element(cd.begin(), cd.end(), [](pair<hcard, int> a, pair<hcard, int> b) {return a.second < b.second; })->second;
	for (int i = 0; i < max_num; i++) {
		wstringstream ostr;
		ostr << setiosflags(ios::left);
		for (auto& j : cd) {
			wstring t;
			if (j.second) {
				t = name[j.first];
				if ((j.first == Yi || j.first == San || j.first == Wu || j.first == Qi || j.first == Jiu) && hcd[j.first] > 0) {
					t += L"*";
					hcd[j.first]--;
				}
				j.second--;
				ostr << setw(4) << t;
			}
			else {
				t = L"    ";
				ostr << setw(7) << t;
			}
		}
		cds += ostr.str() + L"\r\n";
	}
	cds += L"\r\n您刚刚摸到的牌：" + name[T.plys[id]->lastin] + (T.plys[id]->lastin_is_hua ? L"*" : L"");
	std::string utf8Message2 = converter.to_bytes(cds);
	utf8Message2.insert(utf8Message2.begin(), static_cast<char>(hand));
	return utf8Message2;
}
// 游戏逻辑
class LandlordGame {
public:
	LandlordGame() {
		T.plys[0] = make_shared<player>();
		T.plys[1] = make_shared<player>();
		T.plys[2] = make_shared<player>();
		T.inittable();
		T.sendcd();
		T.sts = wait;
		T.turn = 0;
	}

	// 处理玩家的消息
	void handlePlayerMessage(int playerID, const std::string& message) {
		cout << "player " << playerID << " :" << message << endl;
		int a;
		int pid = playerID - 1;
		auto& p = T.plys[pid];
		string ret, nts, error;
		wstring wnts;
		if (pid != T.turn&&T.sts!=over) {
			error.insert(error.begin(), static_cast<char>(errorturn));
			send(clients[pid], error.c_str(), error.size(), 0);
		}
		else {
			switch (T.sts) {
			case wait:
				if (message == "tong") {
					T.plys[pid]->mv_tong();
					a = T.getbuttom();
					T.plys[T.turn]->getcd(a);
					ret = cdheap(T, pid);
					send(clients[pid], ret.c_str(), ret.size(), 0);
					cout << "sned to players" << pid + 1 << " : " << ret.size() << " bytes" << endl;
					wnts = L"玩家" + to_wstring(pid + 1) + L"统了！";
					broadcast(wnts, notis);
				}
				else if (message == "ta") {
					auto i = p->zhao.begin();
					while (i != p->zhao.end() && i->cd != p->lastin)i++;
					if (i == p->zhao.end()) {
						error.insert(error.begin(), static_cast<char>(errorop));
						send(clients[pid], error.c_str(), error.size(), 0);
					}
					else {
						T.plys[T.turn]->ganta(T.plys[T.turn]->lastin);
						a = T.getbuttom();
						T.plys[T.turn]->getcd(a);
						ret = dzfheap(T, pid);
						wnts = L"玩家" + to_wstring(pid + 1) + L"赶踏： " + name[p->lastin];
						broadcast(wnts, notis);
						nts = cdheap(T, pid);
						send(clients[pid], nts.c_str(), nts.size(), 0);
						nts = dzfheap(T, pid);
						nts.insert(nts.begin(), static_cast<char>(mydzf));
						send(clients[pid], nts.c_str(), nts.size(), 0);
						nts[0] = static_cast<char>(nextdzf);
						send(clients[(pid + 2) % 3], nts.c_str(), nts.size(), 0);
						nts[0] = static_cast<char>(enddzf);
						send(clients[(pid + 1) % 3], nts.c_str(), nts.size(), 0);
					}
				}
				else if (message == "hu") {
					gameover(pid);
				}
				else if (isdigit(message.c_str()[0])) {
					int id = atoi(message.c_str());
					//cout << id << endl;
					hcard cd = static_cast<hcard>(id);
					if (p->mycds.find(cd) != p->mycds.end()) {
						T.plys[pid]->throwcd(cd);
						T.last = cd;
						T.last_is_J = T.plys[pid]->lastout_is_hua;
						T.sts = response;
						T.responsewho = pid;
						wnts = L"玩家" + to_wstring(pid + 1) + L"打出：" + name[T.last] + (T.last_is_J ? L"*" : L"");
						broadcast(wnts, notis);
						nts = cdheap(T, pid);
						send(clients[pid], nts.c_str(), nts.size(), 0);
						T.pass();
					}
					else {
						error.insert(error.begin(), static_cast<char>(errorop));
						send(clients[pid], error.c_str(), error.size(), 0);
					}
				}
				else {
					error.insert(error.begin(), static_cast<char>(errorop));
					send(clients[pid], error.c_str(), error.size(), 0);
				}
				break;
			case response:
				if (message == "dui") {
					if (p->mycds[T.last] >= 2 && p->dui.size() < 2) {
						if (T.last == Yi || T.last == San || T.last == Wu || T.last == Qi || T.last == Jiu) {
							if (T.plys[pid]->mycds[T.last] == 2)
								T.plys[pid]->getdui(T.last, T.last_is_J, p->hua[T.last]);
							else if (p->hua[T.last]) {
								T.plys[pid]->getdui(T.last, T.last_is_J, 1);
							}
							else {
								T.plys[pid]->getdui(T.last, T.last_is_J);
							}
						}
						else {
							T.plys[pid]->getdui(T.last, false);
						}
						T.sts = wait;
						wnts = L"玩家" + to_wstring(pid + 1) + L"对了： " + name[T.last];
						broadcast(wnts, notis);
						nts = dzfheap(T, pid);
						nts.insert(nts.begin(), static_cast<char>(mydzf));
						send(clients[pid], nts.c_str(), nts.size(), 0);
						nts[0] = static_cast<char>(nextdzf);
						send(clients[(pid + 2) % 3], nts.c_str(), nts.size(), 0);
						nts[0] = static_cast<char>(enddzf);
						send(clients[(pid + 1) % 3], nts.c_str(), nts.size(), 0);
						nts = cdheap(T, pid);
						send(clients[pid], nts.c_str(), nts.size(), 0);
					}
					else {
						error.insert(error.begin(), static_cast<char>(errorop));
						send(clients[pid], error.c_str(), error.size(), 0);
					}
				}
				else if (message == "zhao") {
					if (p->mycds[T.last] < 3) {
						error.insert(error.begin(), static_cast<char>(errorop));
						send(clients[pid], error.c_str(), error.size(), 0);
					}
					else {
						if (T.last == Yi || T.last == San || T.last == Wu || T.last == Qi || T.last == Jiu) {
							if (T.plys[pid]->mycds[T.last] == 3)
								T.plys[pid]->getzhao(T.last, T.last_is_J, p->hua[T.last]);
							else {
								T.plys[pid]->getzhao(T.last, T.last_is_J, 1);
							}
						}
						else {
							T.plys[pid]->getzhao(T.last, false);
						}
						p->getcd(T.gettop());
						T.sts = wait;
						wnts = L"玩家" + to_wstring(pid + 1) + L"开招： " + name[T.last];
						broadcast(wnts, notis);
						nts = dzfheap(T, pid);
						nts.insert(nts.begin(), static_cast<char>(mydzf));
						send(clients[pid], nts.c_str(), nts.size(), 0);
						nts[0] = static_cast<char>(nextdzf);
						send(clients[(pid + 2) % 3], nts.c_str(), nts.size(), 0);
						nts[0] = static_cast<char>(enddzf);
						send(clients[(pid + 1) % 3], nts.c_str(), nts.size(), 0);
						nts = cdheap(T, pid);
						send(clients[pid], nts.c_str(), nts.size(), 0);
					}
				}
				else if (message == "fan") {
					p->getfan(T.last);
					p->getcd(T.gettop());
					T.sts = wait;
					wnts = L"玩家" + to_wstring(pid + 1) + L"开贩： " + name[T.last];
					broadcast(wnts, notis);
					nts = dzfheap(T, pid);
					nts.insert(nts.begin(), static_cast<char>(mydzf));
					send(clients[pid], nts.c_str(), nts.size(), 0);
					nts[0] = static_cast<char>(nextdzf);
					send(clients[(pid + 2) % 3], nts.c_str(), nts.size(), 0);
					nts[0] = static_cast<char>(enddzf);
					send(clients[(pid + 1) % 3], nts.c_str(), nts.size(), 0);
					nts = cdheap(T, pid);
					send(clients[pid], nts.c_str(), nts.size(), 0);
				}
				else if (message == "hu") {
					gameover(pid);
				}
				else if (message == "pass") {
					T.pass();
					if (T.turn == T.responsewho) {
						T.pass();
						if (T.cdheap.size() == 3) {
							wnts = L"游戏结束，各摸各满";
							broadcast(wnts, notis);
							T.plys[T.turn]->getcd(T.gettop());
							T.plys[(T.turn + 1) % 3]->getcd(T.gettop());
							T.plys[(T.turn + 2) % 3]->getcd(T.gettop());
							nts = cdheap(T, 0);
							send(clients[0], nts.c_str(), nts.size(), 0);
							nts = cdheap(T, 1);
							send(clients[1], nts.c_str(), nts.size(), 0);
							nts = cdheap(T, 2);
							send(clients[2], nts.c_str(), nts.size(), 0);
							T.sts = over;
						}
						else {							
							T.plys[T.turn]->getcd(T.gettop());
							nts = cdheap(T, T.turn);
							send(clients[T.turn], nts.c_str(), nts.size(), 0);
							T.sts = wait;
							wnts = name[T.last] + (T.last_is_J ? L"*  " : L"  ");
							broadcast(wnts, throwncd);
						}						
					}
				}
				else {
					error.insert(error.begin(), static_cast<char>(errorop));
					send(clients[pid], error.c_str(), error.size(), 0);
				}
				break;
			case over:
				if (message == "hu") {
					gameover(pid);
				}
				else {
					gameover(-1);
				}				
				break;
			}
		}
	}

private:
	//gamests gsts = newcd;

};

// 服务器类
class Server {
public:
	Server() {
		// 初始化Winsock
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw std::runtime_error("Failed to initialize Winsock");
		}

		// 创建服务器Socket
		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket == INVALID_SOCKET) {
			throw std::runtime_error("Failed to create server socket");
		}

		// 设置服务器地址
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(PORT);

		// 绑定Socket到地址
		if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			throw std::runtime_error("Failed to bind server socket");
		}

		// 监听连接请求
		if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
			throw std::runtime_error("Failed to listen for incoming connections");
		}

		// 输出服务器信息
		std::cout << "Server started on port " << PORT << std::endl;

		// 启动接受连接的线程
		std::thread(&Server::acceptConnections, this).detach();
	}

	~Server() {
		// 关闭Winsock
		WSACleanup();
	}

	// 处理客户端连接
	void acceptConnections() {
		while (true) {
			SOCKET clientSocket = accept(serverSocket, NULL, NULL);
			if (clientSocket == INVALID_SOCKET) {
				std::cerr << "Failed to accept client connection" << std::endl;
				continue;
			}

			// 输出连接信息
			std::lock_guard<std::mutex> lock(consoleMutex);
			std::cout << "Client connected" << std::endl;

			// 创建处理客户端的线程
			std::thread(&Server::handleClient, this, clientSocket).detach();
		}
	}

	// 处理单个客户端
	void handleClient(SOCKET clientSocket) {
		wstring wstr;
		string tmp;
		int playerID = currentPlayerID++;
		clients[playerID - 1] = clientSocket;
		string s = cdheap(T, playerID - 1);
		wstr = L"欢迎来到花牌游戏，请等待";
		tmp = converter.to_bytes(wstr);
		tmp.insert(tmp.begin(), static_cast<char>(statuss));
		if (playerID != 3)send(clientSocket, tmp.c_str(), tmp.size(), 0);
		while (currentPlayerID < 4);
		send(clientSocket, s.c_str(), s.size(), 0);
		wstr = L"现在是玩家1的出牌回合";
		tmp = converter.to_bytes(wstr);
		tmp.insert(tmp.begin(), static_cast<char>(statuss));
		send(clientSocket, tmp.c_str(), tmp.size(), 0);
		while (true) {
			// 接收客户端消息
			char buffer[1024];

			memset(buffer, 0, sizeof(buffer));
			int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
			if (bytesRead <= 0) {
				// 客户端断开连接
				std::lock_guard<std::mutex> lock(consoleMutex);
				std::cout << "Player " << playerID << " disconnected" << std::endl;
				closesocket(clientSocket);
				return;
			}

			// 处理客户端消息
			std::string message(buffer);
			game.handlePlayerMessage(playerID, message);
			wstr = L"现在是玩家" + to_wstring(T.turn + 1) + (T.sts == wait ? L"的出牌" : L"的响应") + L"回合。";
			broadcast(wstr, statuss);
		}
	}

private:
	SOCKET serverSocket;
	sockaddr_in serverAddr;
	LandlordGame game;
	static int currentPlayerID;
};

int Server::currentPlayerID = 1;

int main() {

	try {
		Server server;

		// 主线程等待
		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		return 0;
	}
	catch (const std::exception& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
		return 1;
	}
}
