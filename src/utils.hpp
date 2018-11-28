/*
 * utils.hpp
 *
 *  Created on: Sep 2, 2018
 *      Author: jetski
 */

#ifndef SRC_UTILS_HPP_
#define SRC_UTILS_HPP_

/* cv2 include */
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
/* ids include */
#include <ueye.h>
/* general include */
#include <stdio.h>
#include <fstream>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <chrono>
#include <cstdlib>
#include <iostream>
/* Serial port include */
#include "serialib.h"

/* for serial lib */
#if defined (_WIN32) || defined( _WIN64)
#define DEVICE_PORT  "COM3"  // COM3 for windows
#endif

#ifdef __linux__
#define DEVICE_PORT "/dev/ttyACM0" // ttyACM0 for linux
#endif
#define TEENSY_BUFFER_SIZE 512

/* namespace */
using namespace cv;
using namespace std;

/* structs */
struct stereoParams{
	//TODO: to implement in utils.cpp
	Mat Kref;
	Mat K;
	Mat r;
	Mat t;
	Mat n;
	float d;
};


string getTimeStamp();
/*
 * TODO:add description
 */
bool int2bool(int input);
/*
 *TODO:add description
 */
stringstream createTargetFolder(bool auto_exposure, int exposureTime , int FPS, string path ,string cfg);
/*
 *TODO:add description
 */
bool saveImage(Mat im, string fileName, string format);
/*
 * add any capable opencv::imwrite format as a string, i.e "jpg". "bin"
 * for binary
 */
void showMono(Mat raw, const string figureName);
/*
 *TODO:add description
 */
void showStereo(Mat rawLeft, Mat rawRight, const string figureName);
/*
 *TODO:add description
 */


///////////// serial port function (for external trigger) //////////////

void serialSetup(serialib& serialPort);
/*
 *TODO:add description
 */
void serialWrite(serialib& serialPort, const char* a);
/*
 *TODO:add description
 */

#endif /* SRC_UTILS_HPP_ */
