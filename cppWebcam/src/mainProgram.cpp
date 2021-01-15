#include "mainProgram.h"

using namespace cv;


void sendFrame(Mat frame) {
	std::vector<uchar> buf;
	imencode(".jpg", frame, buf);

	std::string bufStr(buf.begin(), buf.end());

	sendData(bufStr);
}

void webcamLoop(int fps) {
	try {
		Mat frame;

		frame = getWebcamFrame();

		sendFrame(frame);

		//show the webcam
		//imshow("Webcam", frame);

		if (waitKey(1000 / fps) >= 0)
			return;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

int mainProgram(HANDLE stopEvent) {
	//hide the console window
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	if (!isWebcamSetupCorrectly())
		return 1;

	const int webcamFps = 30;

	// check if service should stop
	while (WaitForSingleObject(stopEvent, 0) != WAIT_OBJECT_0) {
		webcamLoop(webcamFps);
	}

	return 0;
}