#include <iostream>
#include <vector>
#include <bitset>

#include "webcam.h"
#include "networking.h"

using namespace cv;

typedef unsigned char byte;


void sendFrame(Mat frame) {
	std::vector<uchar> buf;
	imencode(".jpg", frame, buf);

	std::string bufStr(buf.begin(), buf.end());

	sendData(bufStr);
}

void webcamLoop() {
	const int webcamFps = 30;

	while (1) {
		try {
			Mat frame;

			frame = getWebcamFrame();

			sendFrame(frame);
			imshow("Webcam", frame);

			if (waitKey(1000 / webcamFps) >= 0)
				break;
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << std::endl;
		}
	}
}

int main(int argv, char** argc) {
	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	if (!isWebcamSetupCorrectly())
		return 1;

	webcamLoop();

	return 0;
}