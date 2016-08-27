#include "Gripper.h"
using namespace cv;
using namespace std;
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include "opencv2/imgproc/imgproc.hpp"

Gripper::Gripper(RotatedRect rect, Mat& image)
{

		rotatedRect = rect;
		Orientate(image);

}

void Gripper::Draw(Mat& input)
{
			static Point2f rect_points[4]; 
		rotatedRect.points( rect_points );

			for(unsigned int j=0; j<4; ++j)
				line(input, rect_points[j], rect_points[(j+1)%4], cv::Scalar(0,255,255),2);
}


void Gripper::Orientate(Mat& image)
{
	
	Mat gripperimage;
	warpAffine(image, gripperimage, getRotationMatrix2D(rotatedRect.center, rotatedRect.angle,1),image.size());
	
	//cv::getRectSubPix(gripperimage,rotatedRect.size,rotatedRect.center,gripperimage);
	
	
	
	float radius=(rotatedRect.size.width+ rotatedRect.size.height)/7.5;
	Point2f center[4] ;
	Point2f rect_points[4]; 
	rotatedRect.points( rect_points );
	
	RotatedRect temp= rotatedRect;
	temp.angle=0;
	temp.points(center);

	
	//center[0]=Point2f(radius,radius);
	//center[1]=Point2f(gripperimage.size().width-radius, radius);
	//center[2]=Point2f(radius, gripperimage.size().height-radius);
	//center[3]=Point2f(gripperimage.size().width-radius, gripperimage.size().height-radius);

	double largest_green=-1;
	double largest_blue=-1;
	int blueDot=-1;
	int greenDot=-1;
	

	for(int i =0; i<4; i++)
	{

		if(rotatedRect.center.x>center[i].x)
				center[i].x=center[i].x+radius;
		else
				center[i].x=center[i].x-radius;
			
		if(rotatedRect.center.y>center[i].y)
				center[i].y=center[i].y+radius;
		else
				center[i].y=center[i].y-radius;

		try{
			Mat corner;
			getRectSubPix(gripperimage,Size2f(radius,radius),center[i],corner);
			//gripperimage(Rect(center[i].y-radius, center[i].y+radius),Range(center[i].x-radius, center[i].x+radius));
		double green_intensity=mean(corner)[1]/(mean(corner)[0]+mean(corner)[2]+mean(corner)[1]);
		double blue_intensity=mean(corner)[0]/(mean(corner)[0]+mean(corner)[2]+mean(corner)[1]);
		if (green_intensity> largest_green)
		{
			greenDot=i;
			largest_green=green_intensity;
		
		}	
		if (blue_intensity> largest_blue)
		{
			blueDot=i;
			largest_blue=blue_intensity;
		
		}	
		}
		catch(cv::Exception e){}
	}
	//cout<<largest_blue<<" "<<largest_green<<endl;
	if( largest_blue<0.35 && largest_green<0.35)
		throw std::invalid_argument("green&blue");

	for(int i =0; i<4; i++)
	{
		//cv::putText(image,to_string(long long(i)),rect_points[i],FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));
		if (i==blueDot)
			circle(gripperimage, center[i],radius,cv::Scalar(255,0,0));
		else if(i==greenDot)
			circle(gripperimage, center[i],radius,cv::Scalar(0,255,0));
		else
			circle(gripperimage, center[i],radius,cv::Scalar(0,255,255));
	}

	

	centerPoint.x=((rect_points[blueDot].x+rect_points[greenDot].x)/2);
	centerPoint.y=((rect_points[blueDot].y+rect_points[greenDot].y)/2);
	circle(image, centerPoint,radius,cv::Scalar(0,255,255));

	namedWindow("gripperimage",CV_WINDOW_FREERATIO);
	imshow("gripperimage",gripperimage);
}

