#ifndef _SOCKET_H
#define _SOCKET_H

#include <vector>
#include <string>
#include "PinHole.h"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;
class Socket{
	public:
		Socket(string _locations, string _config,Point2f _location, Mat& container, float height, int _ID);
		Point2f nextRequestedLocation();
		vector<Point2f> allRequestedLocation();
		void draw();
	private:
		int ID;
		Point2f location;
		vector<PinHole> pinHoles;
        Mat image;
		bool active;
};
#endif
