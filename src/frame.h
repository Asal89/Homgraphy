/*
 * frame.h
 *
 *  Created on: Aug 29, 2018
 *      Author: jetski
 */

#ifndef SRC_FRAME_H_
#define SRC_FRAME_H_

#include <opencv2/core/core.hpp>
#include "UEyeOpenCV.hpp"
#include <sstream>
#include <string>       // std::string
#include <iostream>

class frame {
public:

	frame();

	frame(cv::Mat im , std::string time);

	frame(const frame& fr);

	frame& operator=(const frame& fr);

//	void getFrame(UeyeOpencvCam& hcam);

	cv::Mat getImage();

	std::string getTime();



private:

	cv::Mat image_;
	std::string timeStamp_;

};

#endif /* SRC_FRAME_H_ */
