#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE

#include <iostream>
#include <vector>
#include <asio.hpp>
#include "opencv2/opencv.hpp"

void sendData(std::string data);