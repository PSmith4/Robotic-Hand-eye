#include "Socket.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "ratio.h"
#include <string>
#include <vector>
using namespace std;
using namespace cv;
//extern float ratio; //holdingbox defined variabl, pixels per mm
Socket::Socket(string _locationsfile, string _config,Point2f _location, Mat& container, float height, int _ID)
{
	ID=_ID;
    location=_location;


	try{    image =container(cv::Rect_<float>(location.x, location.y,container.size().width-(location.x*2) ,height));}
	catch(Exception e){}


	////namedWindow("1Socket"+std::to_string( ID),CV_WINDOW_FREERATIO);
	//imshow("1Socket"+std::to_string(ID),image);
	Mat redThresh = image.clone();
	inRange(redThresh, cv::Scalar(0, 0, 120), cv::Scalar(125, 125, 255), redThresh);
	////namedWindow("2Socket"+std::to_string(long long(ID)),CV_WINDOW_KEEPRATIO);
	//imshow("2Socket"+std::to_string(long long(ID)),redThresh);

	Mat erroded = redThresh.clone();
	cv::erode(erroded, erroded,Mat());
	cv::dilate(erroded, erroded,Mat());
	////namedWindow("3Socket"+std::to_string(long long(ID)),CV_WINDOW_KEEPRATIO);
	//cv::resizeWindow("3Socket"+std::to_string(long long(ID)),200,100);
	//imshow("3Socket"+std::to_string(long long(ID)),erroded);

	Mat cannyed;
	Canny(erroded,cannyed, 20,60); // Maybe use int threshold = 200;    cv::Mat mask = output > threshold;
    vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//namedWindow("4Socket"+std::to_string(long long(ID)),CV_WINDOW_FREERATIO);
	//imshow("4Socket"+std::to_string(long long(ID)),cannyed);


	findContours( erroded, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	for (int i = 0; i < contours.size(); i++)
	{

        Rect2f PotentialSocket= boundingRect(contours[i]);
		//cout<< PotentialSocket.size.area()<<" "<<image.rows*image.cols *0.25<<endl;
		if (PotentialSocket.area()  > (image.rows*image.cols *0.25) ) // check if this rectangle found is at least 1/4 the expected size
		{
			try{
				static Point2f rect_points[2];
				rect_points[0]=Point2f(PotentialSocket.x,PotentialSocket.y);
				rect_points[1]=Point2f(PotentialSocket.x+PotentialSocket.width,PotentialSocket.y+PotentialSocket.height);


				//Mat temp=image.clone();
                //drawContours(temp,contours,i,cv::Scalar(0,225,255));
                //rectangle(temp,rect_points[0],rect_points[1],cv::Scalar(0,255,0));

				//cout<<topLeft.x<<"m"<<topLeft.y<<endl;
				location=rect_points[0]+location;
				//namedWindow("5Socket"+std::to_string((ID)),CV_WINDOW_FREERATIO);
				//imshow("5Socket"+std::to_string((ID)),temp);
                //cout<<"resizing"<<endl;
				//getRectSubPix(image, PotentialSocket.size, PotentialSocket.center, image);
				if(rect_points[1].x<image.size().width && rect_points[1].y<image.size().height)
				image =image(PotentialSocket);

			} catch(Exception e){cout<<"socket crash"<<endl;}
		}
	}

	//namedWindow("6Socket"+std::to_string((ID)),CV_WINDOW_FREERATIO);
	//imshow("6Socket"+std::to_string((ID)),image);

	//cout<<mean(image)[2]/(mean(image)[2]+mean(image)[1]+mean(image)[0])<<endl;
	if (mean(image)[2]/(mean(image)[2]+mean(image)[1]+mean(image)[0]) > 0.45)
	{
		active=true;
		//cout<<"active"<<endl;
	}
	else
	{
		//cout<<"notactive"<<endl;
		active=false;
	}
	if(active)
	{
		namedWindow("Socket"+std::to_string(ID),CV_WINDOW_KEEPRATIO);
		ifstream locationFile (_locationsfile.c_str());
		string line;
		while (getline(locationFile,line))
		{
			//cout<<line<<endl;
			stringstream  lineStream(line);
			string        unit;
			getline(lineStream,unit,',');

			double x=atof(unit.c_str());
			getline(lineStream,unit,',');
			double y=atof(unit.c_str());
            //cout<<x<<" "<<y<<endl;
  			x= x*RatioSingleton::GetInstance()->GetRatio();//ratio ;
			y=y*RatioSingleton::GetInstance()->GetRatio();
			//cout<<x<<" "<<y<<endl;
			try{pinHoles.push_back(PinHole(Point2f(x,y),image));} catch(cv::Exception e){}
		}

		ifstream config(_config.c_str());
		while (getline(config,line))
		{
			stringstream  lineStream(line);
			string        cell;
			//cout<<line<<endl;
			while(getline(lineStream,cell,','))
			{
				try{
				int postion = atoi(cell.c_str());
				pinHoles.at(postion-1).request();
				}
				catch(out_of_range& e){cout<<"Check config file, range exeeded"<<endl;}
			}

		}
	}

}

Point2f Socket::nextRequestedLocation()
{
	if( active==true)
	{
		for(int i =0; i< pinHoles.size(); i++)
		{
			//cout<<i<<" "<< pinHoles.at(i).isRequested()<< pinHoles.at(i).isFilled()<<endl;
			if(pinHoles.at(i).isRequested() && pinHoles.at(i).isFilled()==false)
			{
				Point2f temp= pinHoles.at(i).getLocation();
				//temp.y=temp.y-image.size().height;
				return location+temp;

			}
		}
	}
	return Point2f(0,0);
}

vector<Point2f> Socket::allRequestedLocation()
{
	vector<Point2f> returned;
	if (active==false)
		return returned;
	for(int i =0; i< pinHoles.size(); i++)
	{
			if(pinHoles.at(i).isRequested())
				returned.push_back(pinHoles.at(i).getLocation()+location);
	}
	return returned;
}

void Socket::draw()
{
    //std::cout << " " << image.size().width <<" " << image.size().height << std::endl;
	for (int i=0; i<2; i++)
			{
				for (int j=0; j<image.cols; j++)
				{
					/*The following lines make the red and blue channels zero
					(this section of the image will be shades of green)*/
					image.at<Vec3b>(i,j)[0] = 0;
					if(active)
						image.at<Vec3b>(i,j)[2] = 0;
					else
						image.at<Vec3b>(i,j)[1] = 0;
				}
			}
	for(int i =0; i< pinHoles.size(); i++)
	{
		pinHoles.at(i).draw();
	}
	if(active)
		imshow("Socket"+std::to_string(ID),image);
}
