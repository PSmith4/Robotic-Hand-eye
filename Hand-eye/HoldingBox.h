#include "Socket.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
using namespace std;


class HoldingBox{
public:
	HoldingBox(RotatedRect rect, Mat& image);
	void Draw(Mat& input);
	Point2f getReferenceCorner();
		vector<Point2f>  getNextPoint();

private:
	void Orientate(Mat& image);
	RotatedRect rotatedRect;
	Point2f* getPoints();

	int type;
	int referene_corner;
	
	double trueAngle;
	vector<Socket> sockets;
	Mat image;

};
