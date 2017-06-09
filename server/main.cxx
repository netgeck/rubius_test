/* 
 * File:   main.cxx
 * Author: kostya
 *
 * Created on 9 июня 2017 г., 21:19
 */

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <memory>


#include <syslog.h>
#include <string>



using namespace std;

namespace basio = boost::asio;

//Here is a simple synchronous server:using basio;
typedef std::shared_ptr<basio::ip::tcp::socket> socket_ptr;

void client_session(socket_ptr sock) {
	while (true) {
		char data[512];
		size_t len = sock->read_some(basio::buffer(data));
		if (len > 0)
			write(*sock, basio::buffer("ok", 2));
	}
}

/*
 * 
 */
int main(int argc, char** argv) {
	syslog(LOG_NOTICE, "старт сервера");
	
	basio::io_service service;
	basio::ip::tcp::endpoint ep(basio::ip::tcp::v4(), 2001); // listen on 2001
	basio::ip::tcp::acceptor acc(service, ep);
	while (true) {
		socket_ptr sock(new basio::ip::tcp::socket(service));
		acc.accept(*sock);
		std::thread(std::bind(client_session, sock));
	}

	return 0;
}

