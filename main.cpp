#include <opencv2\opencv.hpp>
#include <iostream>
#include "ObjDetect.h"

#define OBJLENGTH 7.65

cv::VideoCapture cam;
bool calibrateColor = false;
ObjDetect detector;
cv::Mat image,org_image, renderImage;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN){
		std::cout << "mouse click: i = " << y << ", j = " << x << std::endl;
		if (calibrateColor == true) detector.SetColor(image, y, x);
	}
}

bool Initialize(){
	cv::namedWindow("original image", 1);
	cv::setMouseCallback("original image", CallBackFunc, NULL);

	cam.open(0);
	cam >> image;
	if (image.rows == 0 && image.cols == 0) return false;
	detector.Initialize(image.rows, image.cols);
	return true;
}

void DrawPoint(cv::Mat &renderImage, std::vector<cv::Point> points){
	for (int i = 0; i < points.size(); i++){
		cv::circle(renderImage, points[i], 5, cv::Scalar(0, 255, 0),10);
	}
}

void DrawText(cv::Mat &renderImage, std::vector<cv::Point> points ,double distance){
	int avr_x=0, avr_y=0;
	int fontFace = CV_FONT_HERSHEY_PLAIN;
	double fontScale = 2;
	int thickness = 3;
	if (points.size() != 0){
		for (int i = 0; i < points.size(); i++){
			avr_x += points[i].x;
			avr_y += points[i].y;
		}
		avr_x /= points.size();
		avr_y /= points.size();
		cv::Point center(avr_x, avr_y);
		char tmpstr[20];
		sprintf(tmpstr, "%f", distance);
		cv::putText(renderImage, tmpstr, center, fontFace, fontScale, cv::Scalar(0,255,0), thickness, 8);
	}
}

int main(){
	std::vector<cv::Point> points;
	std::vector<int> edges;
	std::vector<int> diagonals;
	double imageLength = 0;
	double distance = 0;
	double focalLength = 0.395; //0.367
	if (!Initialize()) return 0;

	while (1){
		cam >> org_image;
		//Blur The image For Better Segmentation
		cv::blur(org_image, image, cv::Size(3, 3));
		detector.ProcessImage(image, renderImage);
		//Get 4 Point of Corners 
		points = detector.GetPoints();
		//Draw The Corners
		DrawPoint(image, points); 
		DrawText(image, points, distance);
		cv::imshow("original image", image);
		char c = cv::waitKey(1);
		if (c == 27) break;
		else if (c == 'c') calibrateColor = !calibrateColor;
			
		edges = detector.GetEdges(points);
		diagonals = detector.GetDiagonals(points);
		imageLength = detector.GetImageLength(points, edges, diagonals);
		distance = detector.FindLength(imageLength, focalLength , OBJLENGTH);

		std::cout << " ImageSize = " <<imageLength << "  PX" << std::endl;
		std::cout << "DISTANCE = " << distance << std::endl;
		
		cv::imshow("render image", renderImage);
	}
	return 0;
}

