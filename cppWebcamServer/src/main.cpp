#include <iostream>
#include "opencv2/opencv.hpp"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>

using namespace cv;
using namespace asio::ip;


Mat latestFrame;

void updateLatestFrame(std::vector<uchar> data) {
    latestFrame = imdecode(data, IMREAD_COLOR);
}

void listenForSockets(const int port) {
    while (1) {
        try {
            /*waiting for connection*/
            asio::io_service io_service;
            tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

            tcp::socket socket(io_service);
            acceptor.accept(socket);

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

        imshow("Webcam", latestFrame);
        waitKey(1);
    }
}

int main(int argv, char** argc) {
	listenForSockets(52946);

	return 0;
}