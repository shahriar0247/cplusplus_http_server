#include <iostream>
#include <string>
#include <functional>
#include <WS2tcpip.h>

void add_view(std::string path, std::function<void(SOCKET)> function);

std::string render_html(std::string filename);

void route(std::string buf3, SOCKET clientSocket);