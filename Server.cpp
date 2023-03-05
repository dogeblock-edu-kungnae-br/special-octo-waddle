#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32")

#define SERVERPORT 9000
#define BUFSIZE 512

// ДЕКЛАРАЦИЯ ТИПОВ
// typedef int SOCKET;

// ДЕКЛАРАЦИЯ ФУНКЦИИ
void err_quit(const char *msg);
void err_display(const char *msg);

int main(int argc, char* argv[]) {
	int retval;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	printf("[УВЕДОМЛЕНИЕ] Успешная инициализация winsock!\n");
	// НАЧАЛО ПРОГРАММЫ
	// СОЗДАТЬ SOCKET
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
	// ПЕРЕМЕННЫЕ ДЛЯ ПЕРЕДАЧИ ДАННЫХ
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// ОТОБРАЖЕНИЕ ИНФОРМАЦИИ О ПОДКЛЮЧЕННОМ КЛИЕНТЕ
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP-СЕРВЕР] Присоединился новый клиент: IP-адрес=%s, Номер-порта=%s\n",
			addr, ntohs(clientaddr.sin_port));
		// ПЕРЕДАЧА ДАННЫХ С КЛИЕНТОМ
		while(1) {
			// ПОЛУЧИТЬ ДАННЫЕ
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				break;
			}
			else if (retval == 0) break;
			// ВЫВОД ПОЛУЧЕННЫХ ДАННЫХ
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);
			// ОТПРАВИТЬ ДАННЫЕ
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
		}
		// ЗАКРЫТЬ SOCKET
		closesocket(client_sock);
		printf("[TCP-СЕРВЕР] ОТКЛЮЧЕН ОТ КЛИЕНТА: IP-адрес=%s, Номер-порта=%s\n",
			addr, ntohs(clientaddr.sin_port));
	}
	// ЗАКРЫТЬ SOCKET
	closesocket(listen_sock);
	// КОНЕЦ ПРОГРАММЫ
	WSACleanup();
	return 0;
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
