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
float ratio; //global (i know, it just works nicely ok)
RNG rng(12345);

//hide the local functions in an anon namespace
namespace {

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
		namedWindow("out1",CV_WINDOW_KEEPRATIO);
		namedWindow("out2",CV_WINDOW_KEEPRATIO);
		namedWindow("out3",CV_WINDOW_KEEPRATIO);
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
		capture >> background;

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
					
					if (hierarchy[i][3]==-1 && hierarchy[i][2]!=-1 ) //only make parents with children map. as these are our box
					{	
						RotatedRect PotentialHoldingBox(minAreaRect(contours[i]));
						cout<<PotentialHoldingBox.size.area()<<endl;
						try{
							if( PotentialHoldingBox.size.area() > 6000 && PotentialHoldingBox.size.area()<8000)
								holders.push_back( HoldingBox(PotentialHoldingBox,output4) );
							
						}
						catch(std::invalid_argument e)
						{
							// if at this point, there was a large block, but it had no red corner.... so its the gripper?
							try{
									if( PotentialHoldingBox.size.area() > 3000 && PotentialHoldingBox.size.area()<5000)
											grippers.push_back( Gripper(PotentialHoldingBox,output4) );
								}
							catch(std::invalid_argument e)
							{}

						}
						cv::drawContours(output4,contours,i,cv::Scalar(0,225,255));
					}

			
					
						
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
									putText(output4, to_string(double(angle*180.0/M_PI))+" deg", midLine+Point2f(0,20*ratio),FONT_HERSHEY_PLAIN,2,cv::Scalar(0,255,0));


									outFile<<dist<<","<<angle<<endl;

									//send code to robot here.
								}catch(std::out_of_range e){}
								outFile.close();
							}
					}

			   }






			/*End of modifying pixel values*/

			/****************** End of Swinburne modifications ****************/

            imshow(window_name, input);
			imshow("out1", output1);
			imshow("out2", output2);
			imshow("background",background);
			imshow("out4",output4);
			imshow("out3",output3);

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

			//threshold(background,background,MainThresh,255,0);
			//cvtColor(background,background,CV_BGR2GRAY,0);
			break;
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


