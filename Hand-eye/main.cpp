#include "opencv2/core/core.hpp"
#include "RobotShell.h"
#include <iostream>
#include <string>
using namespace cv;
using namespace std;
int main()
{
string temp;
    //std::cout<<"test\n";
    RobotShell Robot;

   //std::cout<<"moving to zero\n";
    Robot.moveToPosZero();
     // std::cin>>temp;
    // std::cout<<"moving to new pos\n";

//    Robot.moveTo(266,526,140);
   //  std::cin>>temp;
  //  Robot.moveRelative(20,20);
   //  std::cin>>temp;
     //std::cout<<"moving to zero\n";
   // Robot.moveToPosZero();
}