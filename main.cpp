#include "pch.h"
#include <memory>
#include "opencv2/opencv.hpp"
#include <iostream>
#include "COpenCVVideoControlBar.h"

using namespace std;
using namespace cv;

#define VIDEO_FILE ("./vid/C0006.MP4")
#define MAIN_WINDOW_NAME "Frame"
#define VIDEO_START_FRAME (0)
const Size chessboardSize(8, 11);
const int startID = 20;

#define KEYCODE_ESCAP 27
#define KEYCODE_SPACE 32
#define KEYCODE_LEFT 2424832
#define KEYCODE_RIGHT 2555904
#define KEYCODE_S 115



struct MOUSE_STATE {
	int event;
	int x;
	int y;
	int flags;
} mouse_state;

void initWindow() {
	cv::namedWindow(MAIN_WINDOW_NAME, cv::WINDOW_NORMAL);
	cv::resizeWindow(MAIN_WINDOW_NAME, 960, 540);
}

enum FRAME_CONTROL {
	FRAME_PLAY,
	FRAME_PAUSE,
	FRAME_FORWARD_ONE_FRAME,
	FRAME_BACKWARD_ONE_FRAME,
	FRAME_ONE_FRAME_PAUSE
};

void ProcessMainLoopKeyEvent(bool & needQuit, FRAME_CONTROL & control, Mat & frame , int &imgNum)
{
	needQuit = false;
	// Press  ESC on keyboard to exit
	int c = (int)waitKeyEx(1);
	//cout << (int)c <<endl;	
	if (c == KEYCODE_ESCAP)
	{
		needQuit = true;
		return;
	}
	else if (c == KEYCODE_SPACE) {
		if (control == FRAME_PAUSE)
			control = FRAME_PLAY;
		else
			control = FRAME_PAUSE;
	}
	else if (c == KEYCODE_LEFT) {
		control = FRAME_BACKWARD_ONE_FRAME;
	}
	else if (c == KEYCODE_RIGHT) {
		control = FRAME_FORWARD_ONE_FRAME;
	}
	else if (c == KEYCODE_S) {
		control = FRAME_PAUSE;
		string imgName = "./imgs/img" + to_string(imgNum) + ".jpg";
		cout << "img " << imgNum << "is saved" << endl;
		imwrite(imgName, frame);
		imgNum++;
	}
}

FRAME_CONTROL frameControlFlag;

Mat GetVideoFrame(VideoCapture & capf, FRAME_CONTROL & control) {
	Mat frame;
	if (control == FRAME_PLAY) {

	}
	else if (control == FRAME_PAUSE || control == FRAME_ONE_FRAME_PAUSE) {
		double frame_index = capf.get(CV_CAP_PROP_POS_FRAMES);
		Mat frame2;
		capf.retrieve(frame2);
		frame = frame2.clone();
		return frame;
		//capf.set(CV_CAP_PROP_POS_FRAMES, frame_index);
	}
	else if (control == FRAME_FORWARD_ONE_FRAME) {
		double frame_index = capf.get(CV_CAP_PROP_POS_FRAMES);
		control = FRAME_ONE_FRAME_PAUSE;

	}
	else if (control == FRAME_BACKWARD_ONE_FRAME) {
		double frame_index = capf.get(CV_CAP_PROP_POS_FRAMES);
		capf.set(CV_CAP_PROP_POS_FRAMES, frame_index - 2.0);
		control = FRAME_ONE_FRAME_PAUSE;
	}

	capf >> frame;
	return frame;
}

int main() {
	Mat frame;
	

	VideoCapture cap(VIDEO_FILE);
	//frame = GetVideoFrame(cap, frameControlFlag);

	cap.set(CV_CAP_PROP_POS_FRAMES, VIDEO_START_FRAME);
	frameControlFlag = FRAME_PLAY;
	if (!cap.isOpened()) {

		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	initWindow();

	COpenCVVideoControlBar controlbar(MAIN_WINDOW_NAME);

	cv::setMouseCallback(MAIN_WINDOW_NAME, [](int event, int x, int y, int flags, void *userdata)-> void {
		mouse_state.event = event;
		mouse_state.x = x;
		mouse_state.y = y;
		mouse_state.flags = flags;
	});

	int imgNum = startID;
	
	while (1) {
		frame = GetVideoFrame(cap, frameControlFlag);
		controlbar.UpdateStatus(cap);


		if (frame.empty()) {
			cout << "frame is empty" << endl;
			break;
		}
			
		bool needQuit = false;
		vector<Point2f> pointBuf;
		bool found = findChessboardCorners(frame, chessboardSize, pointBuf, CALIB_CB_FAST_CHECK);
		Mat viewFrame = frame.clone();
		drawChessboardCorners(viewFrame, chessboardSize, Mat(pointBuf), found);
		imshow("Frame", viewFrame);
		ProcessMainLoopKeyEvent(needQuit, frameControlFlag, frame, imgNum);
		//cout << int('s') << endl;
		if (needQuit)
			break;
	}
	system("pause");
}