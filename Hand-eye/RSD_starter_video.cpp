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

    /* //vector projection http://www.maths.usyd.edu.au/u/MOW/vectors/vectors-10/v-10-2.html
        image_point: pixel point to convert into world space
        image_x_start: pixel point of gripper base position
        image_x_end: pixel point of gripper moved 'world_x_distance' along the world x-axis 
        out_world_pos: the resultant position in world horizontal plane
    */
    void ImageToWorld(Vec2f image_point,Vec2f image_x_start, Vec2f image_x_end,
                      float world_x_distance, Vec2f & out_world_pos)
    {
       Vec2f im_poi_delta = image_point - image_x_start; 
       Vec2f im_x_delta = image_x_end - image_x_start;
       float pixel_dist = sqrtf(im_x_delta.val[0]*im_x_delta.val[0] + im_x_delta.val[1]*im_x_delta.val[1]);
       Vec2f x_val = (im_x_delta.dot(im_poi_delta) / (pixel_dist*pixel_dist)) * im_x_delta;
       Vec2f y_val = im_poi_delta - x_val;
       float image2world  = world_x_distance / pixel_dist;
       float w_x = mag(x_val) * image2world;
       float w_y = mag(y_val) * image2world;
       cout << "world poi position (" << w_x << ", " << w_y << ")" << endl;
       out_world_pos.val[0] = w_x;
       out_world_pos.val[1] = w_y;
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

    int process(VideoCapture& capture) {
        int n = 0;
        char filename[200];
        string window_name = "video | q or esc to quit";
		string second_window_name ="outputImage";
        cout << "press space to save a picture. q or esc to quit" << endl;
        namedWindow(window_name, CV_WINDOW_KEEPRATIO); //resizable window;
		//namedWindow("out1",CV_WINDOW_KEEPRATIO);
		//namedWindow("out2",CV_WINDOW_KEEPRATIO);
		//namedWindow("out3",CV_WINDOW_KEEPRATIO);
		namedWindow("out4",CV_WINDOW_KEEPRATIO);

		namedWindow("background",CV_WINDOW_KEEPRATIO);
        Mat input;
		Mat background;
		Mat output1;
		Mat output2;
		Mat output3;
		Mat output4;
		Mat output5;
    int Cannythresh=20;
		int MainThresh=10;
		background = imread("background.jpg");
        RobotShell Robot;
        Robot.moveToPosZero();

        if (!background.data)
        {
            capture>>background;
        }
        vector<Point2f> travel;
		//threshold(background,background,MainThresh,255,0);
		//cvtColor(background,background,CV_BGR2GRAY,0);
        for (;;) {
            capture >> input;
			capture>> output3;
			capture>> output4;
			capture>> output1;
			capture>> output5;

				capture>> output2;
            if (input.empty())
                break;

			/****************** Entry point for Swinburne RSD students ****************/


			/*The code contained here reads and outputs a single pixel value at (10,15)*/
			/*Vec3b intensity = frame.at<Vec3b>(10,15);
			int blue = intensity.val[0];
			int green = intensity.val[1];
			int red = intensity.val[2];
			cout << "Intensity = " << endl << " " << blue << " " << green << " " << red << endl << endl;
			/*End of modifying pixel values*/

			/*The code contained here modifies the output pixel values*/
			/* Modify the pixels of the RGB image */
			output1=input-background;
			//cvtColor(input,output,CV_BGR2GRAY,0);
			//normalize(output1,output2);
			threshold(output1,output1,MainThresh,255,0);
			cvtColor(output1,output2,CV_BGR2GRAY,0);





			//blur( output2, output2, Size(3,3) );
			erode(output2,output2,Mat());
			erode(output2,output2,Mat());

			dilate(output2,output2,Mat());
			dilate(output2,output2,Mat());
			//dilate(output2,output2,Mat());
			Canny(output2,output3, Cannythresh,Cannythresh*3); // Maybe use int threshold = 200;    cv::Mat mask = output > threshold;
			dilate(output3,output3,Mat());


			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;

			  findContours( output3, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

			  if(contours.size()>200 && Cannythresh<255)
				   Cannythresh++;
			   if(contours.size()==0 && Cannythresh>0)
				   Cannythresh--;

			   vector<HoldingBox> holders;
			   vector<Gripper> grippers;
			 for (int i = 0; i < contours.size(); i++)
       {
					RotatedRect PotentialHoldingBox(minAreaRect(contours[i]));
					//cout<<PotentialHoldingBox.size.area()<<endl;
					try{
						if (hierarchy[i][3]==-1 && hierarchy[i][2]!=-1 ) //only make parents with children map. as these are our box
{
							if( PotentialHoldingBox.size.area() > 6000 && PotentialHoldingBox.size.area()<8000)
								holders.push_back( HoldingBox(PotentialHoldingBox,output4) );
							else if( PotentialHoldingBox.size.area() > 1000 && PotentialHoldingBox.size.area()<3000)
								grippers.push_back( Gripper(PotentialHoldingBox,output4) );
						}
					}
					catch(std::invalid_argument e)
					{
						// if at this point, there was a large block, but it had no red corner.... so its the gripper?
						try{
								if( PotentialHoldingBox.size.area() > 1000 && PotentialHoldingBox.size.area()<3000)
										grippers.push_back( Gripper(PotentialHoldingBox,output4) );
							}
						catch(std::invalid_argument e)
						{}
					}
					//cv::drawContours(output4,contours,i,cv::Scalar(0,225,255));
			   }
			      for(int j=0; j<grippers.size(); j++ ) //del me
					{

						 grippers.at(j).Draw(output4);
				  }


			   cv::inRange(output5,cv::Scalar(0, 0, 0),cv::Scalar(255, 125,125),output5);
			   for(int i=0; i<holders.size(); i++ )
			   {
				  holders.at(i).Draw(output4);
				    for(int j=0; j<grippers.size(); j++ )
					{

						 grippers.at(j).Draw(output4);
						//cout<<holders.at(i).getNextPoint().x<<" "<<holders.at(i).getNextPoint().y<<endl;
						vector<Point2f> temp= holders.at(i).getNextPoint();
						//for (int k=0; k<temp.size(); k++)
							{
								ofstream outFile;
									outFile.open("distanceOutput.csv",fstream::app);
								try{
                  /*Get pin world position*/
                  Vec2f gripper_img_start = {266.4f,350.2f}; //these we're generated by inspection
                  Vec2f gripper_img_end = {450.5f,448.5f};    //we can automate this later...
                  Vec2f pin_world_pos = {};
                  ImageToWorld(temp.at(2),gripper_img_start,gripper_img_end,200.0f,pin_world_pos);
                  std::cout << "pin world position = { " << pin_world_pos.val[0] << " , " << pin_world_pos.val[1] << " }" << std::endl;
                  /*----------------------*/

									line(output4, grippers.at(j).getCenterPoint(), temp.at(2),cv::Scalar(255,0,255),1);
									//line(output4, grippers.at(j).getCenterPoint(), temp.at(k),cv::Scalar(255,0,255),1);
									//line(output4, Point2f(0,0), temp.at(k),cv::Scalar(255,0,255),1);
									Point2f midLine=(grippers.at(j).getCenterPoint()+temp.at(2));
									midLine.y=midLine.y/2.0;
									midLine.x=midLine.x/2.0;
									float dist = norm(grippers.at(j).getCenterPoint()-temp.at(2));
									float angle= atan2((grippers.at(j).getCenterPoint().y-temp.at(2).y),(temp.at(2).x)-grippers.at(j).getCenterPoint().x);
									//if (angle<0.0)
									//	angle=angle+M_PI;
									putText(output4, to_string( double(dist)), midLine,FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));
									putText(output4, to_string(double(angle*180.0/M_PI))+" deg", midLine+Point2f(0,20*RatioSingleton::GetInstance()->GetRatio()),FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));


									outFile<<dist<<","<<angle<<endl;

									travel.push_back(Point2f(dist*cos(angle), dist*sin(angle)));

									//send code to robot here.
								}catch(std::out_of_range e){}
								outFile.close();
							}
					}

			   }




			/*End of modifying pixel values*/

			/****************** End of Swinburne modifications ****************/

            imshow(window_name, input);
			//imshow("out1", output1);
			//imshow("out2", output2);
			imshow("background",background);
			imshow("out4",output4);
			//imshow("out3",output3);

            char key = (char)waitKey(5); //delay N millis, usually long enough to display and capture input
            switch (key) {
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
            if (travel.size() >100)
            {
                float max_x=-9^9;
                float max_y=-9^9;
                float min_x=9^9;
                float min_y=9^9;
                for (int i=0; i< travel.size(); i++)
                {
                    if (travel.at(i).x < min_x)
                        min_x= travel.at(i).x;
                    if (travel.at(i).x > max_x)
                        max_x= travel.at(i).x;
                    if (travel.at(i).y < min_y)
                        min_y= travel.at(i).y;
                    if (travel.at(i).y > max_y)
                        max_y= travel.at(i).y;
                }
                float av_x = (min_x+max_x)/2.0;
                float av_y = (min_y+max_y)/2.0;

            }
            Robot.pickPin();
            Robot.moveToPosZero();
            Robot.moveRelative_0pose(200,00);
            Robot.placePin();
            //Robot.placePin();


            break;
        default:
            break;
            }
        }
        return 0;
    }

}

int amain(int ac, char** av) {
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


