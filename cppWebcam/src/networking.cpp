#include "networking.h"

using namespace asio::ip;


//const std::string g_Host = "192.168.2.6";
const std::vector<std::string> g_HostVec{
	"62.131.213.61",
	"84.84.191.209"
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
		tcp::resolver::iterator endpoint = resolver.resolve(tcp::resolver::query(g_HostVec[g_HostSelected], g_Port));

		try {
			asio::connect(this->socket, endpoint);
		}
		catch(std::exception& ignored){
			(void)ignored;

			g_HostSelected++;
			while (g_HostSelected >= g_HostVec.size())
				g_HostSelected -= (int) g_HostVec.size();
		}
	};

	void send(std::string const& message) {
		socket.send(asio::buffer(message));
	}
};

void sendData(std::string data) {
	asio::io_service svc;
	Client client(svc);

	std::string dataLen = std::to_string(data.size());
	std::string header = "--------";
	for (int i = 0; i < dataLen.size(); i++) {
		header[i] = dataLen[i];
	}

	std::cout << g_HostVec[g_HostSelected] << std::endl;

	client.send(header);
	client.send(data);
}