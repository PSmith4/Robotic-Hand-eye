#include "PinHole.h"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
using namespace cv;
using namespace std;

PinHole::PinHole(Point2f _location, Mat& container)
{
    location=_location;

    //cout<<location.x<<" "<<location.y<<endl;
    diam = float(container.size().width)/7.5;
   location.x= location.x+diam/2.0;
   location.y= location.y-diam/2.0;
   try{
    image= container(Rect(location.x,location.y,diam, diam));
	}
	catch(cv::Exception e){cout<<"pin sub-matirx error"<<endl;}
	filled=false;
	requested=false;

	//cout<<mean(container)[0]+mean(container)[2]+mean(container)[1] <<" "<<mean(image)[0]+mean(image)[2]+mean(image)[1]<<endl;
	if (mean(image)[0]+mean(image)[2]+mean(image)[1] >(mean(container)[0]+mean(container)[2]+mean(container)[1])*1.25)
		filled=true;

    if(!filled)
    {
    Point2f darkestPoint;
    double darkest =1000;
       for (int i=1; i<image.rows-1; i++)
			{
				for (int j=1; j<image.cols-1; j++)
				{
                    Mat sect=image(Rect(i-1,j-1,3,3));
                    double temp=mean(sect)[0]+mean(sect)[2]+mean(sect)[1];
                    if(temp<darkest)
                    {
                        darkest=temp;
                        darkestPoint=Point2f(i,j);
                    }
				}
            }
    try{
    location=darkestPoint;
    image= image(Rect(darkestPoint.x-1,darkestPoint.y-1,3,3));
	}
	catch(cv::Exception e){cout<<"pin sub-matirx error"<<endl;}
        //image.at<Vec3b>(darkestPoint)[0] =255;

    }
}

void PinHole::draw()
{

		for (int i=0; i<image.rows; i++)
		//int i = image.rows/2;
			{
				for (int j=0; j<image.cols; j++)
			// int j= image.cols/2;
				{
					/*The following lines make the red and blue channels zero
					(this section of the image will be shades of green)*/

					if(filled)
					{	image.at<Vec3b>(i,j)[2] =0;
						image.at<Vec3b>(i,j)[1] =0;
						image.at<Vec3b>(i,j)[0] =255;
					}
					else if(requested)
					{
						image.at<Vec3b>(i,j)[0] =0;
						image.at<Vec3b>(i,j)[2] =255;
						image.at<Vec3b>(i,j)[1] =255;
						//line(image,Point2f(diam/2.0,diam/2.0),Point2f(diam/2.0,diam/2.0),cv::Scalar(255,225,255),1);
					}
					else
					{
						image.at<Vec3b>(i,j)[2] =255;
						image.at<Vec3b>(i,j)[1] =255;
						image.at<Vec3b>(i,j)[0] =255;
					}
				}
			}
}
