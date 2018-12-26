#include "utils.hpp"


using namespace cv;
using namespace std;



/* some other helper functions: */
string getTimeStamp() {

	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds> (now.time_since_epoch()) -
       	  	  std::chrono::duration_cast<std::chrono::seconds>      (now.time_since_epoch());

	std::stringstream currTimeStamp;
	std::stringstream folderName;
	currTimeStamp << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S-") << ms.count();
	return currTimeStamp.str();

}

bool int2bool(int input){

	if(input == 1){
		return true;
	}
	if(input == 0){
		return false;
	}
	cout << "Wrong arguments >>> default." << endl;
	return true;
}

bool saveImage(Mat im, string fileName, string format){
	if (format != "bin"){
		Mat rgb;
		cvtColor(im, rgb, CV_BayerBG2BGR);
		fileName = fileName + "." + format;
		imwrite(fileName, rgb);
		return true;
	}
	else{
		FILE* pfile = fopen((fileName + ".bin").c_str(), "wb");
		if (pfile == NULL){
			cout << "Can't write to file" << endl;
			return false;
		}
		int width = im.cols;
		int height = im.rows;
		fwrite(im.data, sizeof(unsigned char), width*height, pfile);
		fclose(pfile);
		return true;
	}
}

stringstream createTargetFolder(bool auto_exposure, int exposureTime , int FPS, string path, string cfg) {

	string folderPath = path;
	std::stringstream currTimeStamp;
	std::stringstream Path		   ;
	std::stringstream folderName   ;
	std::stringstream command      ;

	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	currTimeStamp.str("");  //reset timestamp
	currTimeStamp.clear();  //reset timestamp
	currTimeStamp << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S");

	if(auto_exposure == 0)
	{
		folderName << FPS << "fps-" << exposureTime << "ms-" << currTimeStamp.str();
	}
	else
	{
		folderName << FPS << "fps-AutoExposure-" << currTimeStamp.str();
	}
	command << "mkdir -p " << folderPath << folderName.str() ;
	system(command.str().c_str());
	if (cfg == "stereo"){
		//TODO: create folder for each camera
	}
	Path << folderPath <<	folderName.str();
	return Path;
}

void showMono(Mat raw, const string figureName){
	Mat rgb;
	Mat small;

	int width = raw.cols;
	int height = raw.rows;

	//resize & format convert:
	cvtColor(raw, rgb, CV_BayerBG2BGR);
	resize(rgb,small , cv::Size(width/3,height/3));

	imshow(figureName, small);
	waitKey(10);

	return;
}

void showStereo(Mat rawLeft, Mat rawRight, const string figureName){
	Mat l_rgb;
	Mat l_small;
	Mat r_rgb;
	Mat r_small;
	Mat stereo;
	if(rawLeft.cols != rawRight.cols || rawLeft.rows != rawRight.rows){
		cout << "Wrong dimension" << endl;
		cout << "rawLeft.cols" << rawLeft.cols << endl;
		cout << "rawRight.cols" << rawRight.cols << endl;
		cout << "rawRight.rows" << rawRight.rows << endl;
		cout << "rawLeft.rows" << rawLeft.rows<< endl;
		return;
	}

	int width = rawLeft.cols;
	int height = rawLeft.rows;

	//resize & format convert:
	cvtColor(rawLeft, l_rgb, CV_BayerBG2BGR);
	resize(l_rgb,l_small , cv::Size(width/3,height/3));
	//resize & format convert:
	cvtColor(rawRight, r_rgb, CV_BayerBG2BGR);
	resize(r_rgb,r_small , cv::Size(width/3,height/3));

	hconcat(l_small,r_small, stereo);

	imshow(figureName, stereo);
	waitKey(10);

	return;
}

void serialSetup(serialib& serialPort){
	int portErr;                                            // Used for portErrurn values
	char teensyOutput[TEENSY_BUFFER_SIZE];
	// Open serial port
	portErr = serialPort.Open(DEVICE_PORT, 9600);                       // Open serial link at 115200 bauds
	if (portErr != 1) {                                                 // If an error occured...
		cout << "Error while opening port. Permission/address problem" << endl;     // ... display a message ...                                                 // ... quit the application
		return;
	}
	cout << "Serial port opened successfully" << endl;
	return;
}

void serialWrite(serialib& serialPort, const char* a){
	int portErr;
	portErr = serialPort.WriteString(a);
	if (portErr != 1) {                                                 // If the writting operation failed ...
			printf("Error while writing data\n");
			return;
	}
	cout << "Write operation is successful" << endl;
	return;
}

//TODO: add all upcoming functions to some new classes
