/*
 * frame.cpp
 *
 *  Created on: Aug 29, 2018
 *      Author: jetski
 */

#include "frame.h"
#include "UEyeOpenCV.hpp"


frame::frame(){
};

frame::frame(cv::Mat im ,std::string time){
	image_ = im;
	timeStamp_= time;
}

frame::frame(const frame& fr){
	image_ = fr.image_.clone();
	timeStamp_ = fr.timeStamp_;
}

frame& frame::operator=(const frame& fr){
	image_ = fr.image_.clone();
	timeStamp_ = fr.timeStamp_;
	return *this;
}


//void frame::getFrame(UeyeOpencvCam& hcam){
//	image_ = hcam.getFrame().clone();
//	timeStamp_.clear();
//	timeStamp_ = getTimeStamp();
//	return;
//}

cv::Mat frame::getImage(){
	return image_.clone();
}


std::string frame::getTime(){
	return timeStamp_;
}


