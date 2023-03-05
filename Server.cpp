#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32")

// ДЕКЛАРАЦИЯ ТИПОВ
// typedef int SOCKET;

// ДЕКЛАРАЦИЯ ФУНКЦИИ
void err_quit(const char *msg);
void err_display(const char *msg);
bool GetIPAddr(const char* name, struct in_addr* addr);
bool GetDomainName(struct in_addr addr, char* name, int namelen);

int main(int argc, char* argv[]) {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	printf("[УВЕДОМЛЕНИЕ] Успешная инициализация winsock!\n");
	// НАЧАЛО ПРОГРАММЫ
	const char* ipv4test = "147.46.114.70";
	printf("IPv4 주소(변환 전) = %s\n", ipv4test);
	struct in_addr ipv4num;
	inet_pton(AF_INET, ipv4test, &ipv4num);
	printf("IPv4 주소(변환 후) = %#x\n", ipv4num.s_addr);
	char ipv4str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ipv4num, ipv4str, sizeof(ipv4str));
	printf("IPv4 주소(다시 변환 후) = %s\n", ipv4str);
	// КОНЕЦ ПРОГРАММЫ
	WSACleanup();
	return 0;
}

bool GetIPAddr(const char* name, struct in_addr* addr) {
	struct hostent* ptr = gethostbyname(name);
	if (ptr == NULL) {
		err_display("gethostbyname()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET)
		return false;
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return true;
}

bool GetDomainName(struct in_addr addr, char* name, int namelen) {
	struct hostent* ptr = gethostbyaddr((const char*)&addr,
		sizeof(addr), AF_INET);
	if (ptr == NULL) {
		err_display("gethostbyaddr()");
		return false;
	}
	if (ptr->h_addrtype != AF_INET)
		return false;
	strncpy(name, ptr->h_name, namelen);
	return true;
}

void err_quit(const char *msg) {
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA),
		(char*)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char *msg) {
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
