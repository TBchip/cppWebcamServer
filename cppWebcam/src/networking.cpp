#include "networking.h"

using namespace asio::ip;


//const std::string g_Host = "192.168.2.6";
const std::vector<std::string> g_HostVec{
	"62.131.213.62",
	"62.131.213.63"
};
int g_HostSelected = 0;
const std::string g_Port = "51304";

struct Client
{
	asio::io_service& io_service;
	asio::ip::tcp::socket socket;

	Client(asio::io_service& svc)
		: io_service(svc), socket(io_service)
	{
		tcp::resolver resolver(io_service);
		std::cout << 2 << std::endl;
		tcp::resolver::iterator endpoint = resolver.resolve(tcp::resolver::query(g_HostVec[g_HostSelected], g_Port));
		std::cout << 3 << std::endl;
		try {
			asio::connect(this->socket, endpoint);
		}
		catch(std::exception& e){
			g_HostSelected++;
			while (g_HostSelected >= g_HostVec.size())
				g_HostSelected -= g_HostVec.size();
		}
		std::cout << 4 << std::endl;
	};

	void send(std::string const& message) {
		socket.send(asio::buffer(message));
		std::cout << 6 << std::endl;
	}
};

void sendData(std::string data) {
	asio::io_service svc;
	std::cout << 1 << std::endl;
	Client client(svc);


	std::string dataLen = std::to_string(data.size());
	std::string header = "--------";
	for (int i = 0; i < dataLen.size(); i++) {
		header[i] = dataLen[i];
	}

	std::cout << 5 << std::endl;
	client.send(header);
	client.send(data);
}