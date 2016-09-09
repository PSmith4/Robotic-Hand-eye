/*
* RSD_starter_video.cpp
*
* This code was developed for students studying RME40003 Robot Systems Design at Swinburne
* University of Technology in Melbourne, Australia
*
* This code is largely based on the 'starter_video.cpp' code created by Ethan Rublee, Nov 23,
* 2010 for use with OpenCV software. Ethan Rublee's code comes packaged with the OpenCV files
* and can be found in ..\OpenCV\sources\samples\cpp\
*
* This is a simple starter sample  for using opencv, getting a video stream, displaying images,
* reading and outputting pixel values, and modifying pixel values.
*
*/

/*
* CODE MODIFICATIONS AND BUG FIXES
* 24/07/2014 Code added by Michelle Dunn to read, output and modify pixel values
* 21/08/2014 Comments added by Michelle Dunn for Swinburne students
*/
#include "RobotShell.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include "HoldingBox.h"
#include "Gripper.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>


using namespace cv;
using namespace std;
#include "ratio.h"
//RatioSingleton ratio; //global (i know, it just works nicely ok)
RNG rng(12345);

//hide the local functions in an anon namespace
namespace {

    float mag(Vec2f a){ return sqrt(a.val[0]*a.val[0] + a.val[1]*a.val[1]); }
	Vec2f gripper_img_start; //these we're generated by inspection
	Vec2f gripper_img_end;    //we can automate this later...

    /* //vector projection http://www.maths.usyd.edu.au/u/MOW/vectors/vectors-10/v-10-2.html
        image_point: pixel point to convert into world space
        image_x_start: pixel point of gripper base position
        image_x_end: pixel point of gripper moved 'world_x_distance' along the world x-axis
        out_world_pos: the resultant position in world horizontal plane
    */

	Point2f meanMeasurment(vector<Point2f> measurments)
	{
		Point2f avg;
		if (measurments.size() >20)
		{
            float mean_x=0;
            float mean_y=0;

            for (int i=0; i< measurments.size(); i++)
			{
                mean_x=mean_x+measurments.at(i).x;
                mean_y=mean_y+measurments.at(i).y;
			}
			mean_x=mean_x/float (measurments.size());
            mean_y=mean_y/float (measurments.size());

            float dev_x=0;
            float dev_y=0;
            for (int i=0; i< measurments.size(); i++)
            {
                dev_x+=(measurments.at(i).x-mean_x)*(measurments.at(i).x-mean_x);
                dev_y+=(measurments.at(i).y-mean_y)*(measurments.at(i).y-mean_y);
            }
            dev_x=dev_x/float (measurments.size());
            dev_y=dev_y/float (measurments.size());

            float sd_x=sqrtf(dev_x);
            float sd_y=sqrtf(dev_y);
            cout<<"Mean"<<mean_x<<","<<mean_y<<","<<dev_x<<","<<dev_y<<","<<sd_x<<","<<sd_y<<endl;
			float max_x=numeric_limits<float>::min();
			float max_y=numeric_limits<float>::min();
			float min_x=numeric_limits<float>::max();
			float min_y=numeric_limits<float>::max();
			for (int i=0; i< measurments.size(); i++)
			{
				if (measurments.at(i).x < min_x && measurments.at(i).x>= (mean_x-sd_x*1.5) )
					min_x= measurments.at(i).x;
				if (measurments.at(i).x > max_x && measurments.at(i).x<= (mean_x+sd_x*1.5))
					max_x= measurments.at(i).x;
				if (measurments.at(i).y < min_y && measurments.at(i).y>= (mean_y-sd_y*1.5))
					min_y= measurments.at(i).y;
				if (measurments.at(i).y > max_y  && measurments.at(i).y<= (mean_y+sd_y*1.5))
					max_y= measurments.at(i).y;
			}
			avg.x = (min_x+max_x)/2.0;
			avg.y = (min_y+max_y)/2.0;
			cout<<"x"<<min_x<<","<<max_x<<"="<<avg.x<<endl;
			cout<<"y"<<min_y<<","<<max_y<<"="<<avg.y<<endl;

            measurments.clear();
		}

		else
			throw std::out_of_range("need more readings");


		return avg;
	}



//this gives the exact same answer as the previous code, but now the signs are correct for negative co-ordinates (didn't see that bug :/)

    void ImageToWorld(Vec2f image_point, float world_x_distance, Point2f & out_world_pos)
    {
     cout<<"camera poi positoin"<< image_point.val[0]<<", "<<image_point.val[1]<<endl;
       Vec2f im_poi_delta = image_point - gripper_img_start;
       Vec2f im_x_delta = gripper_img_end - gripper_img_start;
       float pixel_dist = sqrtf(im_x_delta.val[0]*im_x_delta.val[0] + im_x_delta.val[1]*im_x_delta.val[1]);
       Vec2f x_val = (im_x_delta.dot(im_poi_delta) / (pixel_dist*pixel_dist)) * im_x_delta;
       Vec2f y_val = im_poi_delta - x_val;
       float image2world  = world_x_distance / pixel_dist;
       float w_x = mag(x_val) * image2world;
       float w_y = mag(y_val) * image2world;
       cout << "world poi position (" << w_x << ", " << w_y << ")" << endl;
       out_world_pos.x = w_x;
       out_world_pos.y = w_y;
    }

    void help(char** av) {
        cout << "\nThis program justs gets you started reading images from video\n"
            "Usage:\n./" << av[0] << " <video device number>\n"
            << "q,Q,esc -- quit\n"
            << "space   -- save frame\n\n"
            << "\tThis is a starter sample, to get you up and going in a copy pasta fashion\n"
            << "\tThe program captures frames from a camera connected to your computer.\n"
            << "\tTo find the video device number, try ls /dev/video* \n"
            << "\tYou may also pass a video file, like my_vide.avi instead of a device number"
            << endl;
    }

	void Object_Detection(Mat input, Mat background, vector<HoldingBox> &holders,vector<Gripper> &grippers )
	{
		//namedWindow("out1",CV_WINDOW_KEEPRATIO);
		//namedWindow("out2",CV_WINDOW_KEEPRATIO);
		//namedWindow("out3",CV_WINDOW_KEEPRATIO);
		//namedWindow("background",CV_WINDOW_KEEPRATIO);
		Mat output1;
		Mat output2;
		Mat output3;
		int Cannythresh=20;
		int MainThresh=10;

		output1=input-background;

		threshold(output1,output1,10,255,0);
		cvtColor(output1,output2,CV_BGR2GRAY,0);

		//blur( output2, output2, Size(3,3) );
		erode(output2,output2,Mat());
		dilate(output2,output2,Mat());
		//dilate(output2,output2,Mat());
		Canny(output2,output3, 20,60); // Maybe use int threshold = 200;    cv::Mat mask = output > threshold;
		dilate(output3,output3,Mat());

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours( output3, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		for (int i = 0; i < contours.size(); i++)
		{
			RotatedRect PotentialHoldingBox(minAreaRect(contours[i]));
			//cout<<PotentialHoldingBox.size.area()<<endl;
			try
			{
				if (hierarchy[i][3]==-1 && hierarchy[i][2]!=-1 ) //only make parents with children map. as these are our box
				{
					if( PotentialHoldingBox.size.area() > 5500 && PotentialHoldingBox.size.area()<7000)
						holders.push_back( HoldingBox(PotentialHoldingBox,input) );
					else if( PotentialHoldingBox.size.area() > 2000 && PotentialHoldingBox.size.area()<3000)
						grippers.push_back( Gripper(PotentialHoldingBox,input) );
				}
			}
			catch(std::invalid_argument e)
			{
				// if at this point, there was a large block, but it had no red corner.... so its the gripper?
				try
				{
					if( PotentialHoldingBox.size.area() > 2000 && PotentialHoldingBox.size.area()<3000)
						grippers.push_back( Gripper(PotentialHoldingBox,input) );
				}
				catch(std::invalid_argument e){}
			}
			//cv::drawContours(output4,contours,i,cv::Scalar(0,225,255));
		}

	}

	Point2f getVector(Point2f to, Point2f from, Mat &drawMat)
	{
		//Astetic overlay
		Point2f midLine=(from+to);
		midLine.y=midLine.y/2.0;
		midLine.x=midLine.x/2.0;
		float dist = norm(from-to);
		float angle= atan2((from.y-to.y),(to.x)-from.x);
		putText(drawMat, to_string( double(dist)), midLine,FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));
		putText(drawMat, to_string(double(angle*180.0/M_PI))+" deg", midLine+Point2f(0,20*RatioSingleton::GetInstance()->GetRatio()),FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));
		line(drawMat, from, to,cv::Scalar(255,0,255),1);

		//fileSave for debug
		ofstream outFile;
		outFile.open("distanceOutput.csv",fstream::app);
		outFile<<dist<<","<<angle<<endl;
		outFile.close();

		/*Get pin world position*/
		Point2f pinFromGripper;
		pinFromGripper.x=  to.x-from.x;
		pinFromGripper.y= abs(from.y-to.y);

		//cout<<"start referent "<< gripper_img_start.val[0]<<" "<<gripper_img_start.val[1]<<endl;
		//cout<<"from"<<from.x<<", "<<from.y<<endl;
        //cout<<"end referent "<< gripper_img_end.val[0]<<" "<<gripper_img_end.val[1]<<endl;
        //cout<<"pin camera point "<< to.x<<", "<<to.y<<endl;
		//std::cout << "pin world position = { " << pin_world_pos.x << " , " << pin_world_pos.y << " }" << endl;
		/*----------------------*/
		return to;
	}


	void  Robot_calabrate(RobotShell& Robot,VideoCapture& capture, Mat background)
	{
		vector<Point2f> positions;
		Point2f point;
		Mat input;

		cout<<"calabrating camera-arm frames. Please stand by"<<endl;
		Robot.moveToPosZero();
		capture >> input;
        imwrite("gripper at home.jpg",input);
        while(positions.size()<20)
		{
			capture >> input;


			vector<Gripper> grippers;
			vector<HoldingBox> temp;
			Object_Detection(input,background, temp, grippers);
			for(int j=0; j<grippers.size(); j++ )
			{
							grippers.at(j).Draw(input);

				positions.push_back(grippers.at(j).getCenterPoint());
				//cout<<grippers.at(j).getCenterPoint().x<<", "<<grippers.at(j).getCenterPoint().y<<endl;
            }
            //namedWindow("6Socket",CV_WINDOW_FREERATIO);
            //imshow("6Socket",input);
            char key = (char)waitKey(3);
		}

		point = meanMeasurment(positions);
		//cout<<"mean:   "<<point.x<<", "<<point.y<<endl;
		gripper_img_start = {point.x, point.y};

        positions.clear();
		cout<<"Moving to x=200"<<endl;
		Robot.moveRelative(200,0);

		for (int i=0; i<500; i++)
		{
			char key = (char)waitKey(10);
			capture >> input;
		}

        cout<<"done waiting for movment"<<endl;

		while(positions.size()<20)
		{

			capture >> input;
			vector<Gripper> grippers;
			vector<HoldingBox> temp;
			Object_Detection(input,background, temp, grippers);
			for(int j=0; j<grippers.size(); j++ )
			{
							grippers.at(j).Draw(input);

                positions.push_back(grippers.at(j).getCenterPoint());
                //cout<<grippers.at(j).getCenterPoint().x<<", "<<grippers.at(j).getCenterPoint().y<<endl;
            }
            namedWindow("6Socket",CV_WINDOW_FREERATIO);
            imshow("6Socket",input);
            char key = (char)waitKey(3);

		}
		capture >> input;
        imwrite("gripper at 200x.jpg",input);
		point = meanMeasurment(positions);
        //cout<<"mean:   "<<point.x<<", "<<point.y<<endl;
		gripper_img_end = {point.x, point.y};

        Robot.moveToPosZero();

		ofstream outFile;
		outFile.open("GripperPositions.csv",fstream::trunc);
		outFile<<gripper_img_start.val[0]<<","<<gripper_img_start.val[1]<<","<<gripper_img_end.val[0]<<","<<gripper_img_end.val[1]<<","<<endl;
		outFile.close();


        cout<<"calabration done"<<endl;
	}

	void RoboticMotion(RobotShell& Robot,Point2f pinPoint)
	{


		//Robot.movefromZero(200,00); //change this line
		cout<<"moving to" <<pinPoint.x<<","<<pinPoint.y<<"from home "<<endl;
        if(sqrt((pinPoint.x*pinPoint.x)+(pinPoint.y*pinPoint.y)) < 350)
        {
            Robot.pickPin();
            Robot.movefromZero(pinPoint.x,pinPoint.y);
            Robot.placePin();
            Robot.moveToPosZero();
             //Robot.moveRelative(pinPoint.x,pinPoint.y); //change this line
        }
        else
            cout<<"too big"<<endl;


	}

	void read_cords()
	{
		ifstream locationFile ("GripperPositions.csv");
		string line;
		getline(locationFile,line);
		stringstream  lineStream(line);
		string	unit;

		getline(lineStream,unit,',');
		gripper_img_start.val[0]=atof(unit.c_str());
		getline(lineStream,unit,',');
		gripper_img_start.val[1]=atof(unit.c_str());
		getline(lineStream,unit,',');
		gripper_img_end.val[0]=atof(unit.c_str());
		getline(lineStream,unit,',');
		gripper_img_end.val[1]=atof(unit.c_str());

		float x =gripper_img_end.val[0]-gripper_img_start.val[0];
		float y= gripper_img_end.val[1]-gripper_img_start.val[1];

		float ratio_=sqrtf((x*x)+(y*y));
        RatioSingleton::GetInstance()->SetRatio(ratio_/200.0f);
        //cout<<ratio_<<endl;

	}

    int process(VideoCapture& capture) {
        Point2f griptemp;
        int n = 0;
        char filename[200];
        string window_name = "video | q or esc to quit";
		string second_window_name ="outputImage";
        cout << "press space to save a picture. q or esc to quit" << endl;

	    namedWindow(window_name, CV_WINDOW_KEEPRATIO); //resizable window;

        Mat input;
		Mat background;
		background = imread("background.jpg");
		if (!background.data)
        {
            capture>>background;
        }

        RobotShell Robot;
		Robot.moveToPosZero();
		read_cords();

		vector<Point2f> pinPoints;

        for (;;)
		{
			capture >> input;

            if (input.empty())
                break;

			vector<HoldingBox> holders;
			vector<Gripper> grippers;
			Object_Detection(input,background, holders, grippers);

			for(int j=0; j<grippers.size(); j++ )
			{
				grippers.at(j).Draw(input);
			}
			for(int i=0; i<holders.size(); i++ )
			{
				holders.at(i).Draw(input);
			}

			for(int i=0; i<holders.size(); i++ )
			{
				for(int j=0; j<grippers.size(); j++ )
				{
					vector<Point2f> temp= holders.at(i).getNextPoint();

					try
					{
					//getVector(temp.at(0), grippers.at(j).getCenterPoint(), input);
					//getVector(temp.at(1), grippers.at(j).getCenterPoint(), input);
                        griptemp=grippers.at(j).getCenterPoint();
						pinPoints.push_back(getVector(temp.at(2), grippers.at(j).getCenterPoint(), input));
					}catch(std::out_of_range e){}
				}
			}

			/****************** End of Swinburne modifications ****************/

            imshow(window_name, input);

            char key = (char)waitKey(5); //delay N millis, usually long enough to display and capture input
            switch (key)
			{
				case 'q':
				case 'Q':
				case 27: //escape key
					return 0;
				case ' ': //Save an image
					sprintf(filename,"filename%.3d.jpg",n++);
					imwrite(filename,input);
					cout << "Saved " << filename << endl;
					break;
				case 'y':
				case 'Y':
					capture >> background;
					imwrite("background.jpg", background);
					//threshold(background,background,MainThresh,255,0);
					//cvtColor(background,background,CV_BGR2GRAY,0);
					break;
				case 'm':
				case 'M':
					try
					{
					Point2f pin =meanMeasurment(pinPoints) ;
					Point2f pin2;
					//pin2.y=griptemp.y-pin.y;
					//pin2.x=pin.x+griptemp.x;
					circle(input,pin,3,cv::Scalar(255,0,255),1);
                    circle(input,pin,10,cv::Scalar(255,0,255),1);
                    circle(input,pin,50,cv::Scalar(255,0,255),1);

                    circle(input,Point2f(gripper_img_start.val[0],gripper_img_start.val[1]),3,cv::Scalar(255,0,255),3);
                    circle(input,Point2f(gripper_img_end.val[0],gripper_img_end.val[1]),3,cv::Scalar(255,0,255),3);

                    imwrite("pinpoint.jpg", input);

                    Point2f pin_world_pos;
                    //Point2f test(445,397);
                    ImageToWorld(Vec2f(pin.x, pin.y),200.0f,pin_world_pos);
                    RoboticMotion(Robot, pin_world_pos);
					}
					catch(out_of_range& e){cout<<e.what()<<endl;}
					break;

				case 'c':
				case 'C':
					Robot_calabrate(Robot, capture, background);
				default:
					break;
			}
		}
		return 0;
    }

}

int main(int ac, char** av) {
    if (ac != 2) {
        help(av);
        return 1;
    }

    std::string arg = av[1];
    VideoCapture capture(arg); //try to open string, this will attempt to open it as a video file
    if (!capture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
        capture.open(atoi(arg.c_str()));




    if (!capture.isOpened()) {
        cerr << "Failed to open a video device or video file!\n" << endl;
        help(av);
        return 1;
    }
    process(capture);

	return 0;
}


