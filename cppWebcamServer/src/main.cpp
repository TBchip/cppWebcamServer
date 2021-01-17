#include <iostream>
#include <regex>
#include "opencv2/opencv.hpp"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>

using namespace cv;
using namespace asio::ip;


const int port = 51304;
std::vector<address> openAddresses;
Mat latestFrame;

//for stopping infinite loop threads
bool stop_listenForConnection = false;
bool stop_listenForWebcam = false;

void updateLatestFrame(std::vector<uchar> data) {
    latestFrame = imdecode(data, IMREAD_COLOR);
}

void listenForWebcam(const address targetAddress) {
    while (!stop_listenForWebcam) {
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
    while (!stop_listenForConnection) {
        try {
            /*waiting for connection*/
            asio::io_service io_service;
            tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

            tcp::socket socket(io_service);
            acceptor.accept(socket);

            /*add connected adress to list if not in it already*/
            asio::ip::address remoteAdress = socket.remote_endpoint().address();
            if(!std::count(openAddresses.begin(), openAddresses.end(), remoteAdress))
                openAddresses.push_back(remoteAdress);
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
}
int getWebcamIndex() {
    system("cls");

    int selectedIndex = -1;
    while (selectedIndex < 0 || selectedIndex >= openAddresses.size())
    {
        std::cout << "[u] update list" << std::endl;
        std::cout << "[c] close server" << std::endl;
        std::cout << std::endl;

        for (int i = 0; i < openAddresses.size(); i++) {
            std::cout << "[" << i << "] ";
            std::cout << openAddresses[i].to_string();
            std::cout << std::endl;
        }
        if (openAddresses.size() == 0)
            std::cout << "no open clients found" << std::endl;

        std::cout << std::endl << "select an id: ";

        std::string indexInput;
        std::cin >> indexInput;

        system("cls");

        if (indexInput == "u")
            continue;
        else if (std::regex_match(indexInput, (std::regex)"^-?\\d+"))
            selectedIndex = std::stoi(indexInput);
        else if (indexInput == "c") {
            selectedIndex = -1;
            break;
        }
    }

    return selectedIndex;
}

void showLatestFrame() {
    const int maxFPS = 30;

    // wait for latestFrame to be initialized
    while (latestFrame.rows == 0) { Sleep(100); }

    while (1)
    {
        imshow("Webcam", latestFrame);
        if (waitKey(1000/maxFPS) >= 0)
            break;
    }

    destroyWindow("Webcam");
}

int main(int argv, char** argc) {
    while (1) {
        //listen for open clients
        std::thread listenConnection(listenForConnection);

        //select an adress
        int selectedWebcam = getWebcamIndex();

        //stops the listen for connection thread
        stop_listenForConnection = true;
        listenConnection.join();

        if (selectedWebcam == -1)
            break;
        else {
            //start listening for webcam frames from client
            std::thread listenForWebcamThread(listenForWebcam, openAddresses[selectedWebcam]);

            //show the incoming frames
            showLatestFrame();

            //stops the listen for webcam thread
            stop_listenForWebcam = true;
            listenForWebcamThread.join();
        }
    }

	return 0;
}