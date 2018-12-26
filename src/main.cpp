
/* Standard includes */
#include <iostream>
#include <stdio.h>
#include <chrono>
#include <fstream>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <chrono>
#include <cstdlib>
#include <stdint.h>

/* opencv's includes */
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

/* boost's includes */
#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/atomic/atomic.hpp>

/* project's includes */
#include "UEyeOpenCV.hpp"
#include <ueye.h>
#include "frame.h"
#include "utils.hpp"
#include "homography.h"
#include "Reconstruction.h"

/* namespaces */
using namespace std;
using namespace cv;

/* typedefs */
typedef boost::lockfree::spsc_queue<frame, boost::lockfree::capacity<1024> > FRAME_SPSCQ;

/* user's set variables */
string folderPath = "/home/jetski/Desktop/deleteIt/";
string saveFormat = "jpg";
string configuration = "mono"; //TODO:change to "stereo" when done implementing
int leftID  = 2;
int rightID = 3;
const int RAW8 = 1;
const int BGR8 = 1;
int img_width  = 1936;
int img_height = 1216;
int scale_factor = 2; //insert even number

//////////////////////// 35 mm //////////////////////////
//double KmatRef[3][3] = {{6811.10072821551/(double)scale_factor, 0, 961.272247363782/(double)scale_factor },
//        {0, 6824.78998611754/(double)scale_factor, 647.340658150072/(double)scale_factor},
//        {0, 0, 1}};
//
//double Kmat[3][3] = {{6823.01473173781/(double)scale_factor, 0, 967.153157981033/(double)scale_factor },
//	{0, 6835.53522151543/(double)scale_factor, 625.313551182732/(double)scale_factor},
//	{0, 0, 1}};
//
//double rotation[3][3] = {{0.999755456172702, -0.00604948034743692, -0.0212704405323284},
//	{0.00606388484271891, 0.99998142673197, 0.000612773782206468},
//	{0.0212663385077824,-0.000741605434102429, 0.999773570798835}};
//double translation[3][1] = {{-233.405463440145},
//	{-1.47986087932354},
//	{20.7515828935872}};
/////////////////////////////////////////////////////////

//////////////////////// 12.5 mm //////////////////////////
double KmatRef[3][3] = {{2091.86631779203/(double)scale_factor, 0, 977.959275443175/(double)scale_factor },
        {0, 2096.65807138121/(double)scale_factor, 631.77833913523/(double)scale_factor},
        {0, 0, 1}};

double Kmat[3][3] = {{2092.13799098311/(double)scale_factor, 0, 972.281077317332/(double)scale_factor },
	{0, 2096.63994907357/(double)scale_factor, 643.148773140331/(double)scale_factor},
	{0, 0, 1}};

double rotation[3][3] = {{0.999778317597323, -0.00615271184272392, -0.0201360323610835},
	{0.00605442103312433, 0.99996947630974, -0.00493866730780091},
	{0.0201658039319006, 0.0048156604743155, 0.99978505178162}};
double translation[3][1] = {{-224.889713525602},
	{-1.76505204191513},
	{-1.23520866054531}};
/////////////////////////////////////////////////////////

double normal[1][3] = {0, 0, -1};

double dst = 5000; // homography plane distance

/* global variables */
int producerCount = 0;
stringstream TargetPath;
boost::atomic_int consumerCount(0);
boost::atomic<bool> done(false);
FRAME_SPSCQ leftQ;
FRAME_SPSCQ rightQ;
bool boostGain;
double ActualExposure;
int iterations;
bool save;
bool showOutputImage;
bool auto_exposure;
bool autoGain;
int masterGain;
int redGain;
int greenGain;
int blueGain;
double exposure_ms;
double FPS;
homography H;
stereoParams stereo_params;
Mat map_x, map_y, warped;


void leftProducer(void) {

	UeyeOpencvCam uEyeCamera(img_width, img_height, leftID);
	uEyeCamera.setExposure(auto_exposure, exposure_ms) ;
	double newFPS = uEyeCamera.setFPS(FPS);
	ActualExposure = uEyeCamera.getExposure();
	uEyeCamera.setGain( autoGain, masterGain, redGain,greenGain, blueGain, boostGain);

	cout << "Actual Exposure Time: " << ActualExposure << endl;
	cout << "Actual FPS: " << newFPS << endl;

	Mat rawInputImage;
	Mat rgbInputImage;
	Mat rgbInputImageSmall;
	std::string currStamp;

	for (int i = 0; i != iterations; ++i) {
		++producerCount;
		currStamp.clear();  //reset timestamp
		rawInputImage = uEyeCamera.getFrame();
		currStamp = getTimeStamp();
		if (!rawInputImage.empty()){
			frame fr(rawInputImage.clone(),currStamp);
			while (!leftQ.push(fr))
			;
		}
		else {
			cout << "Skipped image" << endl;
			waitKey(100);
			continue;
		}
	}

}

void rightProducer(void) {

	UeyeOpencvCam uEyeCamera(img_width, img_height, rightID);
	uEyeCamera.setExposure(auto_exposure, exposure_ms) ;
	double newFPS = uEyeCamera.setFPS(FPS);
	ActualExposure = uEyeCamera.getExposure();
	uEyeCamera.setGain( autoGain, masterGain, redGain,greenGain, blueGain, boostGain);

	cout << "Actual Exposure Time: " << ActualExposure << endl;
	cout << "Actual FPS: " << newFPS << endl;

	Mat rawInputImage;
	Mat rgbInputImage;
	Mat rgbInputImageSmall;
	std::string currStamp;

	for (int i = 0; i != iterations; ++i) {
		++producerCount;
		currStamp.clear();  //reset timestamp
		rawInputImage = uEyeCamera.getFrame();
		currStamp = getTimeStamp();
		if (!rawInputImage.empty()){
			frame fr(rawInputImage.clone(),currStamp);
			while (!rightQ.push(fr))
			;
//			showMono(rawInputImage, "r");
		}
		else {
			cout << "Skipped image" << endl;
			waitKey(100);
			continue;
		}
	}

}

void consumer(void) {

	Mat leftCurrImage;
	Mat rightCurrImage;
	string currStamp;
	frame leftFrame;
	frame rightFrame;
	int counter = 1;


	while (!done) {
		while (!leftQ.pop(leftFrame) && !done){
		}

		while(!rightQ.pop(rightFrame) && !done){
		}

		/* load images from frames */
		leftCurrImage = leftFrame.getImage();
		rightCurrImage = rightFrame.getImage();


		if (save){
			/* save left image */
			currStamp = leftFrame.getTime();
			string leftFileName = TargetPath.str() + "/left_" + to_string(counter) + "_" + currStamp;
			if(saveImage(leftCurrImage, leftFileName, saveFormat)){
				++consumerCount;
			}

			/* save right image */
			currStamp = rightFrame.getTime();
			string rightFileName = TargetPath.str() + "/right_" + to_string(counter) + "_" + currStamp;
			if(saveImage(rightCurrImage, rightFileName, saveFormat)){
				++consumerCount;
			}
		}

		/* show images */
		if(showOutputImage){
			showStereo(leftCurrImage, rightCurrImage, "output");
		}
		
		Mat warped, corr_intensities, corr_x_derivative, corr_y_derivative, std_map, cost_map, masked;
		resize(leftCurrImage, leftCurrImage, Size(), 1/(double)scale_factor, 1/(double)scale_factor);
		resize(rightCurrImage, rightCurrImage, Size(), 1/(double)scale_factor, 1/(double)scale_factor);
		Reconstruction reconstruct(map_x, map_y, rightCurrImage, leftCurrImage, stereo_params);

		reconstruct.getMasked(masked);
		reconstruct.getWarped(warped);
		reconstruct.XYcalculate();
		cout << "Mean value of Y: " << reconstruct.getInterpulatedY() / 1000 << " [meters]" << endl;
		imshow("masked", masked);
		waitKey(10);

		/////////// Show homography /////////////
		remap(rightCurrImage, warped, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0));
		Mat impair;
		addWeighted(leftCurrImage, 0.5, warped, 0.5, 0.0, impair);
		imshow("pair", impair);
		waitKey(10);
		/////////////////////////////////////////


		counter++;
	}
}

int main(int argc, char* argv[]) {

	/* update variables due to argv[] */
	iterations      = atof(argv[1])			  ;
	showOutputImage = int2bool(atoi(argv[2])) ;
	auto_exposure   = int2bool(atoi(argv[3])) ;
	autoGain        = int2bool(atoi(argv[4])) ;
	masterGain      = atoi(argv[5])			  ;
	redGain         = atoi(argv[6])           ;
	greenGain       = atoi(argv[7])           ;
	blueGain        = atoi(argv[8])           ;
	exposure_ms     = atof(argv[9])           ;
	FPS             = atof(argv[10])          ;
	save            = int2bool(atoi(argv[11]));

	/* initialize stereo parameters */
	stereo_params.K = Mat(3, 3, CV_64FC1, Kmat);
	stereo_params.Kref = Mat(3, 3, CV_64FC1, KmatRef);
	stereo_params.r = Mat(3, 3, CV_64FC1, rotation);
	stereo_params.t = Mat(3, 1, CV_64FC1, translation);
	stereo_params.n = Mat(1, 3, CV_64FC1, normal);
	stereo_params.d = dst;

	/* initialize homography */
	homography H(stereo_params);
	map_x.create(img_height/scale_factor, img_width/scale_factor, CV_32FC1);
	map_y.create(img_height/scale_factor, img_width/scale_factor, CV_32FC1);

	for(int r = 0; r < img_height/scale_factor; r++)
	{
		for(int c = 0; c < img_width/scale_factor; c++)
		{
			map_x.at<float>(r, c) = (H.getHomography().at<double>(0,0)*c + H.getHomography().at<double>(0,1)*r + H.getHomography().at<double>(0,2)) / (H.getHomography().at<double>(2,0)*c + H.getHomography().at<double>(2,1)*r + H.getHomography().at<double>(2,2));
			map_y.at<float>(r, c) = (H.getHomography().at<double>(1,0)*c + H.getHomography().at<double>(1,1)*r + H.getHomography().at<double>(1,2)) / (H.getHomography().at<double>(2,0)*c + H.getHomography().at<double>(2,1)*r + H.getHomography().at<double>(2,2));
		}
	}

	/* open serial port for arduino connection */
	serialib serialPort;
	serialSetup(serialPort);
	serialWrite(serialPort, "0");

	/* in save mode - create target folder */
	if (save){
		TargetPath = createTargetFolder(auto_exposure, exposure_ms , FPS, folderPath, configuration);
		cout << "Target Folder:" << TargetPath.str() << endl;
	}

	/* check if queues are lockfree  */
	cout << "boost::lockfree::queue is ";
	if (!leftQ.is_lock_free()) cout << "not ";
	cout << "lockfree" << endl;
	cout << "boost::lockfree::queue is ";
	if (!rightQ.is_lock_free()) cout << "not ";
	cout << "lockfree" << endl;

	/* start producer thread for each camera, and one for processing */
	auto LeftProducerThreadStart = std::chrono::high_resolution_clock::now();
	auto RightProducerThreadStart = std::chrono::high_resolution_clock::now();
	auto consumerThreadStart = std::chrono::high_resolution_clock::now();
	boost::thread LeftProducerThread(leftProducer);
	boost::thread RightProducerThread(rightProducer);
	boost::thread consumerThread(consumer);

	/* wait for the cameras to be ready. after that start triggering */
	sleep(10);
	cout << "sleep..." << endl;
	serialWrite(serialPort, "1");


	LeftProducerThread.join();
	auto LeftProducerThreadEnd = std::chrono::high_resolution_clock::now();
	auto LeftProducerThreadTime = LeftProducerThreadEnd - LeftProducerThreadStart;
	std::cout << "Left Producer Thread Time " << std::chrono::duration_cast<std::chrono::milliseconds>(LeftProducerThreadTime).count() << "ms to run.\n";

	RightProducerThread.join();
	auto RightProducerThreadEnd = std::chrono::high_resolution_clock::now();
	auto RightProducerThreadTime = RightProducerThreadEnd - RightProducerThreadStart;
	std::cout << "Right Producer Thread Time " << std::chrono::duration_cast<std::chrono::milliseconds>(RightProducerThreadTime).count() << "ms to run.\n";

	done = true;
	consumerThread.join();
	auto consumerThreadEnd = std::chrono::high_resolution_clock::now();
	auto consumerThreadTime = consumerThreadEnd - consumerThreadStart;
	std::cout << "Consumer Thread Time " << std::chrono::duration_cast<std::chrono::milliseconds>(consumerThreadTime).count() << "ms to run.\n";

	cout << "Total produced " << producerCount << " objects." << endl;
	cout << "Consumed " << consumerCount << " objects." << endl;

	serialWrite(serialPort, "0");
	sleep(3);
	serialPort.Close();
	cout << "Press Enter to Continue";
	cin.ignore();

}



