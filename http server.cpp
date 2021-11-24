#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

SOCKET init_server(int port) {
	// initialze winsock

	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		cerr << "Cant initilize winsock, quitting" << endl;
		return 1;
	}

	// create a socket

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET) {
		cerr << "Cant create socket, quitting" << endl;
		return 1;
	}

	// bind socket

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// tell winsock the socket is for listening

	listen(listening, SOMAXCONN);

	return listening;
}

void accent_client(SOCKET listening) {


	sockaddr_in client;

	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);

	char host[NI_MAXHOST]; // client's remote name
	char service[NI_MAXSERV]; // port client is connected to

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << " conneted on port " << service;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}



	char buf[4096];
	char buf2[1024] = "HTTP/1.1\rHost: 200 OK\r\nContent-Type: text/html\r\nContent-Length: 633\r\n\r\n<html>\n                                                            <head>\n                                                                <title>Hello World</title>\n                                                            </head>\n                                                            <body>\n                                                                <h1>Hello World!</h1>\n                                                                <p>Welcome to the index.html web page..</p>\n                                                            </body>\n                                                            </html>\n\n\n\n";
	while (true) {
		ZeroMemory(buf, 4096);

		//wait for client to send data

		int bytesRecv = recv(clientSocket, buf, 4096, 0);
		cout << buf << endl;

		if (bytesRecv == SOCKET_ERROR) {
			cerr << "Error in recv(), Quitting" << endl;
			break;
		}

		if (bytesRecv == 0) {
			cout << "Client disconnected" << endl;
			break;
		}
		send(clientSocket, buf2, sizeof(buf2), 0);







		// echo message back to client
	}

	// close socket

	closesocket(clientSocket);


}

int main()
{
	SOCKET listening = init_server(4444);



	
	accent_client(listening);




	// cleanup winsock

	WSACleanup();
	cin.get();
}
