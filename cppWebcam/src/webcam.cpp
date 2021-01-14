#include "webcam.h"

using namespace cv;


VideoCapture vid(0);

bool isWebcamSetupCorrectly() {
	if (!vid.isOpened()) {
		std::cout << "could not find any video capturing devices..." << std::endl;
		waitKey(3000);
		return false;
	}

	return true;
}

Mat getWebcamFrame() {
	Mat frame;
	vid.read(frame);

	return frame;
}