#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp> 
using namespace cv;
using namespace std;
int main()
{
	Mat frame;
	Mat back;
	Mat fore;
	char *filename = "../videos/can_220_street.MOV";
	//VideoCapture cap1(1);/*to capture from camera*/
	VideoCapture cap1(filename);
	BackgroundSubtractorMOG2 bg;//works on GMM
	bg.set("nmixtures", 10);
	vector < vector < Point > >contours;
	namedWindow("Frame");
	int i = 0;

	int frameH = (int)cap1.get(CV_CAP_PROP_FRAME_HEIGHT);
	int frameW = (int)cap1.get(CV_CAP_PROP_FRAME_WIDTH);
	int fps = (int)cap1.get(CV_CAP_PROP_FPS);
	int numframe = (int)cap1.get(CV_CAP_PROP_FRAME_COUNT);
	int fnumber = 0;//µ±Ç°Ö¡Êý

	for (;fnumber<numframe;fnumber++)
	{
		int flag = 0;
		cap1 >> frame;
		if (fnumber <= 2)continue;
		bg.operator()(frame, fore);
		bg.getBackgroundImage(back);
		erode(fore, fore, cv::Mat());
		erode(fore, fore, cv::Mat());
		dilate(fore, fore, cv::Mat());
		dilate(fore, fore, cv::Mat());
		dilate(fore, fore, cv::Mat());
		findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		drawContours(frame, contours, -1, Scalar(255, 255, 255), 1);
		Scalar color = Scalar(200, 200, 200);
		int a = 0;
		vector<Rect> boundRect(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			boundRect[i] = boundingRect(contours[i]);
		}
		for (i = 0; i < contours.size(); i++)
		{
			if (boundRect[i].width >= 40 || boundRect[i].height >= 40)//eliminates small boxes
			{
				a = a + (boundRect[i].height)*(boundRect[i].width);
			}
			//  cout<<"Net contour area is "<<a<<"\n";
			if (a >= int(frame.rows)*int(frame.cols) / 2)//change denominator as per convenience
			{
				putText(frame, "Tampering", Point(5, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 255), 2);
				cout << "\a";
				flag = 1;
			}
		}
		imshow("Frame", frame);
		if (flag == 1) waitKey(0);
		else waitKey(1);
	}
	return 1;
}