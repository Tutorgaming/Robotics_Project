#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include "Djset.h"
#include <algorithm>

class ObjDetect{
public:
	cv::Mat hsvImage;
	cv::Mat renderImage;
	bool chk[2000][2000];
	std::vector<bool> djchk;
	int hsvbound[6];
	djset disjoint;
	int rows;
	int cols;

	ObjDetect(){
		for(int i = 0; i < 6; i++){
			if(i%2 == 0) hsvbound[i] = 256;
			else hsvbound[i] = -1;
		}
	}

	void Initialize(int rows, int cols){
		disjoint.Initialize(rows, cols);
		this->rows = rows;
		this->cols = cols;
	}

	void SetColor(cv::Mat &image, int i, int j){
		cv::cvtColor(image, hsvImage, CV_BGR2HSV);
		cv::Vec3b vec = hsvImage.at<cv::Vec3b>(i,j);
		if(vec[0] < hsvbound[0]) hsvbound[0] = vec[0];
		if(vec[0] > hsvbound[1]) hsvbound[1] = vec[0];
		if(vec[1] < hsvbound[2]) hsvbound[2] = vec[1];
		if(vec[1] > hsvbound[3]) hsvbound[3] = vec[1];
		if(vec[2] < hsvbound[4]) hsvbound[4] = vec[2];
		if(vec[2] > hsvbound[5]) hsvbound[5] = vec[2];
	}

	void ProcessImage(cv::Mat &image, cv::Mat &outputImage){
		cv::cvtColor(image, hsvImage, CV_BGR2HSV);
		renderImage = cv::Mat(hsvImage);
		cv::Vec3b vec;
		bool valid;
		for(int i = 0; i < renderImage.rows; i++){
			for(int j = 0; j < renderImage.cols; j++){
				vec = hsvImage.at<cv::Vec3b>(i, j);
				valid = true;
				valid = valid && (hsvbound[0] <= vec[0] && vec[0] <= hsvbound[1]);
				valid = valid && (hsvbound[2] <= vec[1] && vec[1] <= hsvbound[3]);
				valid = valid && (hsvbound[4] <= vec[2] && vec[2] <= hsvbound[5]);
				chk[i][j] = false;
				if(valid){
					renderImage.at<cv::Vec3b>(i, j) = vec;
					chk[i][j] = true;
				}
				else renderImage.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
			}
		}

		cv::cvtColor(renderImage, outputImage, CV_HSV2BGR);
	}

	std::vector<cv::Point> GetPoints(){ //FIND CORNERS POINT
		for(int i = 0; i < renderImage.rows; i++){
			for(int j = 0; j < renderImage.cols; j++){
				disjoint.SetAt(i, j, chk[i][j]);
			}
		}

		std::vector<cv::Point> vec;
		int maxcount = 0;
		djchk.resize(rows*cols);
		for(int i = 0; i < djchk.size(); i++) djchk[i] = false;
		for(int i = 0; i < rows; i++){
			for(int j = 0; j < cols; j++){
				if(!chk[i][j]) continue;
				int parent = disjoint.FindParent((i*rows)+cols);
				if(djchk[parent]) continue;
				djchk[parent] = true;
				if(disjoint.arr[parent].count > maxcount){
					vec.clear();
					vec.push_back(disjoint.arr[parent].minj);
					vec.push_back(disjoint.arr[parent].mini);
					vec.push_back(disjoint.arr[parent].maxj);
					vec.push_back(disjoint.arr[parent].maxi);										
					maxcount = disjoint.arr[parent].count;
				}
			}
		}	
		/*for(int i = 0; i < vec.size(); i++){
			std::cout << vec[i] << std::endl;
		}
		std::cout << "==============" << std::endl;*/
		
		return vec;
	}

	std::vector<int> GetEdges(std::vector<cv::Point> &points){
		std::vector<int> edges;

		for (int i = 0; i < points.size(); i++){
			int dx = points[i].x - points[(i + 1) % points.size()].x;
			int dy = points[i].y - points[(i + 1) % points.size()].y;
			edges.push_back( dx*dx + dy*dy );
			//std::cout << "   edges [ " << i << "] = " << edges[i]; 
		}
		std::cout << std::endl;
		return edges;
	}

	std::vector<int> GetDiagonals(std::vector<cv::Point> &points){
		std::vector<int> diagonals;
		if (points.size() != 0){
			int dx1 = points[0].x - points[2].x;
			int dy1 = points[0].y - points[2].y;
			int dx2 = points[1].x - points[3].x;
			int dy2 = points[1].y - points[3].y;

			diagonals.push_back(dx1*dx1 + dy1*dy1);
			diagonals.push_back(dx2*dx2 + dy2*dy2);
			//std::cout << "Diagonal [0] = " << diagonals[0] << "    Diagonal [1] = " << diagonals[1] << std::endl;
		}
		return diagonals;
	}
	
	double GetImageLength(std::vector<cv::Point> &points, std::vector<int> &edges, std::vector<int> &diagonals){
		double answer = 0;
		bool fromEdge = true;
		int comp_x;
		int comp_y;
		for (int x : edges){
			if (x > answer) answer = x;
		}
		for (int i = 0; i < diagonals.size(); i++){
			if (i == 0){
				comp_x = points[0].x - points[2].x;
				comp_y = points[0].y - points[2].y;
			}
			else{
				comp_x = points[1].x - points[3].x;
				comp_y = points[1].y - points[3].y;
			}
			int diagonal = diagonals[i];
			int a = std::min(comp_x, comp_y);
			diagonal += a*a;
			diagonal /= 2;
			if (diagonal > answer){
				fromEdge = false;
				answer = diagonal;
			}
		}
		/*if (fromEdge){
			std::cout << "FROM EDGE NA JA :D " << std::endl;
		}
		else {
			std::cout << "FROM DIAGONALS " << std::endl;
		}*/
		return sqrt(answer);
	}


	double FindLength(double imageLength , double focalLength , double OBJLENGTH ){
		double distance;
		distance = (OBJLENGTH / imageLength)*focalLength*2*1000;
		
		return distance;
	}



};