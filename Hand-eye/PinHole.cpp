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
   location.y= location.y+diam/2.0;
   try{
    image= container(Rect(location.x,location.y,diam, diam));
	}
	catch(cv::Exception e){cout<<"pin sub-matirx error"<<endl;}
	filled=false;
	requested=false;

	//cout<<mean(container)[0]+mean(container)[2]+mean(container)[1] <<" "<<mean(image)[0]+mean(image)[2]+mean(image)[1]<<endl;
	if (mean(image)[0]+mean(image)[2]+mean(image)[1] >(mean(container)[0]+mean(container)[2]+mean(container)[1])*1.25)
		filled=true;

}

void PinHole::draw()
{

		for (int i=0; i<image.rows; i++)
			{
				for (int j=0; j<image.cols; j++)
				{
					/*The following lines make the red and blue channels zero
					(this section of the image will be shades of green)*/

					if(filled)
					{	image.at<Vec3b>(i,j)[2] = 0;
						image.at<Vec3b>(i,j)[1] = 0;
						image.at<Vec3b>(i,j)[0] = 255;
					}
					else if(requested)
					{
						image.at<Vec3b>(i,j)[0] = 0;
						image.at<Vec3b>(i,j)[2] = 255;
						image.at<Vec3b>(i,j)[1] = 255;
						//line(image,Point2f(diam/2.0,diam/2.0),Point2f(diam/2.0,diam/2.0),cv::Scalar(255,225,255),1);
					}
					else
					{
						image.at<Vec3b>(i,j)[2] = 0;
						image.at<Vec3b>(i,j)[1] = 0;
						image.at<Vec3b>(i,j)[0] = 0;
					}
				}
			}
}
