#include "networking.h"

using namespace asio::ip;

struct Client
{
	asio::io_service& io_service;
	asio::ip::tcp::socket socket;

	Client(asio::io_service& svc, std::string const& host, std::string const& port)
		: io_service(svc), socket(io_service)
	{
		tcp::resolver resolver(io_service);
		tcp::resolver::iterator endpoint = resolver.resolve(tcp::resolver::query(host, port));
		asio::connect(this->socket, endpoint);
	};

	void send(std::string const& message) {
		socket.send(asio::buffer(message));
	}
};

void sendData(std::string host, int port, std::string data) {
	asio::io_service svc;
	Client client(svc, host, std::to_string(port));

	std::string dataLen = std::to_string(data.size());
	std::string header = "--------";
	for (int i = 0; i < dataLen.size(); i++) {
		header[i] = dataLen[i];
	}

	client.send(header);
	client.send(data);
}