#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32")

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 512

void err_quit(const char* msg);
void err_display(const char* msg);

int main(int argc, char* argv[]) {
	int retval;
	// ЕСЛИ В АРГУМЕНТЕ КОМАНДНОЙ СТРОКИ ЕСТЬ СТРОКА, ОНА УСТАНАВЛИВАЕТСЯ КАК IP-АДРЕС СЕРВЕРА.
	if (argc > 1) SERVERIP = argv[1];
	// ИНИЦИАЛИЗАЦИЯ WINSOCK
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	// НАЧАЛО ПРОГРАММЫ
	// СОЗДАТЬ SOCKET
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	// ПЕРЕМЕННЫЕ ДЛЯ ПЕРЕДАЧИ ДАННЫХ
	char buf[BUFSIZE + 1];
	int len;
	// ПЕРЕДАЧА ДАННЫХ С СЕРВЕРОМ
	while (1) {
		// ВВОД ДАННЫХ
		printf("\n[ДАННЫЕ ДЛЯ ОТПРАВКИ] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;
		// УДАЛИТЬ СИМВОЛ '\n'
		len = (int)strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;
		// ОТПРАВИТЬ ДАННЫЕ
		retval = send(sock, buf, (int)strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		printf("[TCP-КЛИЕНТ] ОТПРАВЛЕНО %d БАЙТА", retval);
		// ПОЛУЧИТЬ ДАННЫЕ
		retval = recv(sock, buf, retval, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0) break;
		// ПЕЧАТЬ ПОЛУЧИТЬ ДАННЫЕ
		buf[retval] = '\0';
		printf("[TCP-КЛИЕНТ] ПОЛУЧЕНО %d БАЙТА", retval);
		printf("[ПОЛУЧЕННЫЕ ДАННЫЕ] %s\n", buf);
	}
	// ЗАКРЫТЬ SOCKET
	closesocket(sock);
	// ВЫЙТИ(ЗАКРЫТЬ) WINSOCK
	WSACleanup();
	return 0;
}

void err_quit(const char* msg) {
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

void err_display(const char* msg) {
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
