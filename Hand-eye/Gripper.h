#include <vector>
#include <stdio.h>
#include "opencv2/core.hpp"
using namespace cv;
using namespace std;

class Gripper
{
public:
	Gripper(RotatedRect rect, Mat& image);
	void Draw(Mat& input);
	inline Point2f getCenterPoint(){return centerPoint;};
private:
	Point2f centerPoint;

	void Orientate(Mat& image);
	RotatedRect rotatedRect;

};

