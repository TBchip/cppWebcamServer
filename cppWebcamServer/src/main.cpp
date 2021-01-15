#include <iostream>
#include "opencv2/opencv.hpp"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>

using namespace cv;
using namespace asio::ip;


const int port = 5130;
std::vector<address> openAddresses;
Mat latestFrame;

void updateLatestFrame(std::vector<uchar> data) {
    latestFrame = imdecode(data, IMREAD_COLOR);
}

void listenForWebcam(const address targetAddress) {
    while (1) {
        try {
            /*waiting for connection*/
            asio::io_service io_service;
            tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

            tcp::socket socket(io_service);
            acceptor.accept(socket);

            /*check if connection is from targetadress*/
            asio::ip::address remoteAddress = socket.remote_endpoint().address();
            if (remoteAddress != targetAddress)
                continue;

            /*getting header*/
            std::vector<char> header(8);
            asio::read(socket, asio::buffer(header));
            std::string headerStr(header.begin(), header.end());

            /*getting buffer size*/
            std::string::size_type sz;
            int dataSize = std::stoi(headerStr, &sz);

            /*getting data*/
            std::vector<uchar> data(dataSize);
            asio::read(socket, asio::buffer(data));

            updateLatestFrame(data);
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
        }

        
    }
}

void listenForConnection() {
    while (1) {
        try {
            /*waiting for connection*/
            asio::io_service io_service;
            tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

            tcp::socket socket(io_service);
            acceptor.accept(socket);

            /*add connected adress to list*/
            asio::ip::address remoteAdress = socket.remote_endpoint().address();
            openAddresses.push_back(remoteAdress);
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
}

void showLatestFrame() {
    const int maxFPS = 30;

    // wait for latestFrame to be initialized
    while (latestFrame.rows == 0) {}

    while (1)
    {
        imshow("Webcam", latestFrame);
        waitKey(1000 / maxFPS);
    }
}

int main(int argv, char** argc) {
    listenForWebcam(openAddresses[0]);

	return 0;
}