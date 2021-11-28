#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")



class all_routes_class {
public:
	std::string path;
	std::function<void(SOCKET)> function;
};

all_routes_class all_routes[1024];

SOCKET init_server(int port) {
	// initialze winsock

	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		std::cerr << "Cant initilize winsock, quitting" << std::endl;
		return 1;
	}

	// create a socket

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET) {
		std::cerr << "Cant create socket, quitting" << std::endl;
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

void client_connection(sockaddr_in client, SOCKET clientSocket) {




	char host[NI_MAXHOST]; // client's remote name
	char service[NI_MAXSERV]; // port client is connected to

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " conneted on port " << service;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}




	char buf[4096];
	ZeroMemory(buf, 4096);



	int bytesRecv = recv(clientSocket, buf, 4096, 0);
	std::cout << buf << std::endl;
	std::string headers = std::string(buf, sizeof(buf));
	std::string request_info = headers.substr(0, headers.find(" HTTP/1.1"));
	std::string request_type = request_info.substr(0, request_info.find(" "));
	std::string request_path = request_info.substr(request_type.length() + 1);

	if (request_type == "GET") {
		for (all_routes_class one_route : all_routes) {
			if (one_route.path == request_path) {
				std::function<void(SOCKET)> function_name = one_route.function;
				function_name(clientSocket);
			}
		}


	}


	closesocket(clientSocket);


}

void route(std::string buf3, SOCKET clientSocket) {



	std::string buf2 = "HTTP/1.1\rHost: 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string((buf3).length()) + "\r\n\r\n" + buf3;
	char buf4[1024];
	strcpy_s(buf4, buf2.c_str());

	send(clientSocket, buf4, sizeof(buf4), 0);

}

std::string render_html(std::string filename) {
	std::string response = "Hello World";
	std::ifstream html_template;
	html_template.open(filename);

	if (!html_template.is_open()) {
		response = "<h2>File not found</h2>";
	}

	std::stringstream strStream;
	strStream << html_template.rdbuf(); //read the file
	response = strStream.str(); //str holds the content of the file
	return response;
}

void home(SOCKET clientSocket) {

	 std::string response = render_html("lol.html");
	route(response, clientSocket);

}

void add_view(std::string path, std::function<void(SOCKET)> function) {

	all_routes_class route;
	route.path = path;
	route.function = function;
	all_routes[0] = route;

}

int main()
{

	add_view("/", home);

	SOCKET listening = init_server(80);

	std::vector<std::thread> ThreadVector;

	while (true) {

		sockaddr_in client;

		int clientsize = sizeof(client);

		SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);

		ThreadVector.emplace_back([&]() {client_connection(client, clientSocket); });



	}



	WSACleanup();

}
