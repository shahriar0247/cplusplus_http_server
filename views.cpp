#include "http server.h"


void home(SOCKET clientSocket) {

	std::string response = render_html("lol.html");
	route(response, clientSocket);

}


void login(SOCKET clientSocket) {

	std::string response = render_html("login.html");
	route(response, clientSocket);


}



void add_all_views() {

	add_view("/login", login);
	add_view("/", home);
}


