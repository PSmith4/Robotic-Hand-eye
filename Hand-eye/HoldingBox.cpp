
#define debug false
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#include "opencv2/highgui/highgui.hpp"
#include "HoldingBox.h"
#include "ratio.h"
#include <iostream>
using namespace cv;
//RatioSingleton ratio;
using namespace std;
//extern float ratio;
#include "opencv2/imgproc/imgproc.hpp"

#include <string>

	HoldingBox::HoldingBox(RotatedRect rect, Mat& _image)
	{

		rotatedRect = rect;
		Orientate(_image);
		namedWindow("HoldingBox",CV_WINDOW_KEEPRATIO);
		Point2f* rect_points=getPoints();
		warpAffine(_image, image, getRotationMatrix2D(rect.center, trueAngle,1),_image.size());
		if ((trueAngle>0 && trueAngle<90) || (trueAngle>180 && trueAngle<270))
			rect.size=Size2f(rect.size.height, rect.size.width);
        try{
		cv::getRectSubPix(image,rect.size,rect.center,image);
        }
        catch(cv::Exception e){cout<<"holdingbox submatrix error"<<endl;}
//This section is harded coded. for later adaption, improve with dynamic box configs, stored in a csv (68,38,8)
        //cout<<rect.size.height<<" "<<rect.size.width<<end;
		//ratio= rect.size.height/100; //num pixesl per mm
		sockets.push_back(Socket("Socket_Type1.csv", "Socket_Type1_configA.csv",Point2f(10.0*RatioSingleton::GetInstance()->GetRatio(),10.0*RatioSingleton::GetInstance()->GetRatio()), image, 25.0*RatioSingleton::GetInstance()->GetRatio(), sockets.size()+1));
		sockets.push_back(Socket("Socket_Type1.csv", "Socket_Type1_configA.csv",Point2f(10.0*RatioSingleton::GetInstance()->GetRatio(),40.0*RatioSingleton::GetInstance()->GetRatio()), image, 25.0*RatioSingleton::GetInstance()->GetRatio(), sockets.size()+1));
		sockets.push_back(Socket("Socket_Type1.csv", "Socket_Type1_configA.csv",Point2f(10.0*RatioSingleton::GetInstance()->GetRatio(),70.0*RatioSingleton::GetInstance()->GetRatio()), image, 25.0*RatioSingleton::GetInstance()->GetRatio(), sockets.size()+1));
	}

	void HoldingBox::Orientate(Mat& image)
	{
		Point2f rect_points[4];
		rotatedRect.points( rect_points );
		float radius=(rotatedRect.size.width+ rotatedRect.size.height)/10.0;
		Point2f center[4];
		referene_corner=-1; //vc10 thinks its funny to make to_string(int) ambiguas
		double largest_red=-1;
		for(int i =0; i<4; i++)
		{
			//center.x= (rect.center.x+rect_points[i].x*4)/5;
			//center.y= (rect.center.y+rect_points[i].y*4)/5;
			if(rotatedRect.center.x>rect_points[i].x)
				center[i].x=rect_points[i].x+radius*0.66;
			else
				center[i].x=rect_points[i].x-radius*0.66;

			if(rotatedRect.center.y>rect_points[i].y)
				center[i].y=rect_points[i].y+radius*0.66;
			else
				center[i].y=rect_points[i].y-radius*0.66;

			if(center[i].y>radius && center[i].x>radius && center[i].y+radius<image.rows && center[i].x+radius<image.cols)
			{
				//Mat corner = image(Range(center[i].y-radius, center[i].y+radius),Range(center[i].x-radius, center[i].x+radius));
				Mat corner;
				getRectSubPix(image, Size2f(radius,radius), center[i], corner);
				double red_intensity=mean(corner)[2]/(mean(corner)[0]+mean(corner)[2]+mean(corner)[1]);
				if (red_intensity> largest_red)
				{
					referene_corner=i;
					largest_red=red_intensity;
				}
			}
		}

		if (largest_red<0.35)
			throw std::invalid_argument("red");

		if (debug)
		{
                    //putText(image,to_string(long long(rotatedRect.angle)),rotatedRect.center,FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));
			//putText(image,to_string(long long(rotatedRect.angle)),rotatedRect.center,FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));
			//circle(image, center[1],radius,cv::Scalar(0,255,0));
			//circle(image, center[2],radius,cv::Scalar(255,0,0));
			//circle(image, center[3],radius,cv::Scalar(255,255,0));



			for(int i =0; i<4; i++)
			{
                        //putText(image,to_string(i),center[i],FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));
				if (i==referene_corner)
					circle(image, center[i],radius,cv::Scalar(0,0,255));
				else
					circle(image, center[i],radius,cv::Scalar(0,255,0));
			}
		}
	}

	Point2f* HoldingBox::getPoints()
	{
		static Point2f rect_points[4];
		rotatedRect.points( rect_points );
		Point2f temp;
		//cout<<referene_corner<<endl;
		switch (referene_corner){
			case 0:
				temp=rect_points[0];
				rect_points[0]=rect_points[1];
				rect_points[1]=temp;
				temp=rect_points[2];
				rect_points[2]=rect_points[3];
				rect_points[3]=temp;
				trueAngle=rotatedRect.angle;
				break;
			case 1:
				trueAngle=rotatedRect.angle+90;
				break;
			case 2:
				temp=rect_points[1];
				rect_points[1]=rect_points[2];
				rect_points[2]=temp;
				temp=rect_points[0];
				rect_points[0]=rect_points[3];
				rect_points[3]=temp;
				trueAngle=rotatedRect.angle+180;
				break;
			case 3:
				temp=rect_points[1];
				rect_points[1]=rect_points[3];
				rect_points[3]=temp;
				temp=rect_points[2];
				rect_points[2]=rect_points[0];
				rect_points[0]=temp;
				trueAngle=rotatedRect.angle-90;
				break;

		}
		if (trueAngle>=360)
				trueAngle=trueAngle-360;
			if (trueAngle<0)
				trueAngle=trueAngle+360;
		return rect_points;
	}

	void HoldingBox::Draw(Mat& input)
	{
		for(int i=0; i < sockets.size(); i++)
			{
				try{sockets.at(i).draw();}catch(cv::Exception e){}
			}
			imshow("HoldingBox",image);

			Point2f* rect_points;
			rect_points=getPoints();
			//for(unsigned int j=2; j<4; ++j)
				//line(input, rect_points[j], rect_points[(j+1)%4], cv::Scalar(0,255,0),2);
			for(unsigned int j=0; j<2; ++j)
				line(input, rect_points[j], rect_points[(j+1)%4], cv::Scalar(0,0,255),1);
	}

	Point2f HoldingBox::getReferenceCorner()
	{
		static Point2f rect_points[4];
		rotatedRect.points( rect_points );
		return rect_points[referene_corner];
	}

	vector<Point2f> HoldingBox::getNextPoint()
	{
		vector<Point2f> returned;
		Point2f nextPoint;
		for (int i =0; i< sockets.size(); i++)
		{
			nextPoint=sockets.at(i).nextRequestedLocation();
			if(nextPoint.x!=0 && nextPoint.y !=0)
			{
				//point found, now rotate back into camera frame
				vector<Point2f> returned;

				//nextPoint.x=nextPoint.x;// /ratio;
				//nextPoint.y=nextPoint.y;// /ratio;
				//cout<<trueAngle<<endl;
				//cout<<getReferenceCorner().x<<" "<<nextPoint.x* cos(trueAngle/180.0*M_PI)<<" "<<nextPoint.y*sin(trueAngle/180.0*M_PI);
				RotatedRect rect= rotatedRect;
				if ((trueAngle>0 && trueAngle<90) || (trueAngle>180 && trueAngle<270))
					rect.size=Size2f(rect.size.height, rect.size.width);

				//returned.x=getReferenceCorner().x+rect.size.height*sin(trueAngle/180.0*M_PI) + nextPoint.x* cos(trueAngle/180.0*M_PI) +nextPoint.y*sin(trueAngle/180.0*M_PI);
				//returned.y=getReferenceCorner().y-rect.size.height*cos(trueAngle/180.0*M_PI) +nextPoint.x*sin(trueAngle/180.0*M_PI)+ nextPoint.y*cos(trueAngle/180.0*M_PI);

				returned.push_back(getReferenceCorner());
				returned.push_back(Point2f(returned.back().x-rect.size.height*cos(float((trueAngle+90)/180.0*M_PI)),returned.back().y-rect.size.height*sin(float((trueAngle+90)/180.0*M_PI))));

//				line(image, Point2f(0,0), nextPoint,cv::Scalar(255,0,255),1);
				returned.push_back(Point2f(returned.back().x+ nextPoint.x* cos(float(trueAngle/180.0*M_PI)) -nextPoint.y*sin(float(trueAngle/180.0*M_PI)), returned.back().y+nextPoint.x*sin(float(trueAngle/180.0*M_PI))+ nextPoint.y*cos(float(trueAngle/180.0*M_PI))));
				return returned;
			}
		}
		return returned; //if here return 0,0
	}
