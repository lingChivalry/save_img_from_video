#pragma once
#include <opencv.hpp>
#include "opencv2/opencv.hpp"
class COpenCVVideoControlBar
{
private:
	std::string windowName;
	bool needUpdate;
	bool firstInit;
	bool updating;
	void dirty();

public:
	int position = 0;
	int count;
	COpenCVVideoControlBar(std::string masterWindowName);
	virtual ~COpenCVVideoControlBar();

	void UpdateStatus(cv::VideoCapture & cap);

};

