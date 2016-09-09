#include "RobotShell.h"
#include <Python.h>
#include <string>
#include <iostream>

using namespace std;
RobotShell::RobotShell()
{
    Py_SetProgramName(0);
	Py_Initialize();
	PyRun_SimpleString("import sys\n");
	PyRun_SimpleString("sys.path.append('/home/phil/Robotic-Hand-eye/Hand-eye')\n");
	PyRun_SimpleString("import abb\n");

	PyRun_SimpleString("R= abb.Robot(ip='192.168.125.1')\n");
    PyRun_SimpleString("R.set_tool([[0,2,108], [1,0,0,0]])\n");
    //moveToPosZero();
    //PyRun_SimpleString("R.set_joints([32.4,66.6,17.1,-57.7,-86.9, 94.7])\n");

	//have config CSV filemove
	pos0x=370;
	pos0y=400;
	//pos0y=550;
	pos0z=90;


	pinPlaces.push_back( {273, 740} );
    pinPlaces.push_back( {273, 732} );
	pinPlaces.push_back( {273, 723} );

	//stop wire tangle
    //PyRun_SimpleString("R.set_joints([85,65,-19,-86.1,-16,173.5])\n");
   // PyRun_SimpleString("R.open_gripper()\n");
    PyRun_SimpleString("R.close_gripper()\n");

    cout<<"Robot setup"<<endl;
}

RobotShell::~RobotShell()
{
	PyRun_SimpleString("R.close()");
	Py_Finalize();

}

void RobotShell::movefromZero(float x, float y)
{
string s="R.set_cartesian([["+
        to_string(pos0x+x) +"," +
		to_string(pos0y+y) +"," +
		to_string(pos0z) +
		//"],[0.05,0.01,0.76,0.65]])\n").c_str());
        "],[0,1,0,0]])\n";
        cout<<s;
        PyRun_SimpleString(s.c_str());
}
void RobotShell::moveToPosZero()
{
	PyRun_SimpleString(("R.set_cartesian([["+
        to_string(pos0x) +"," +
		to_string(pos0y) +"," +
		to_string(pos0z) +
		//"],[0.05,0.01,0.76,0.65]])\n").c_str());
        "],[0.00,0.00,0.75,0.75]])\n").c_str());

    PyRun_SimpleString("R.set_joints([31.5,71.8,14.9,-58.5,-88.3,93.8])\n");

    PyRun_SimpleString(("R.set_cartesian([["+
        to_string(pos0x) +"," +
		to_string(pos0y) +"," +
		to_string(pos0z) +
		//"],[0.05,0.01,0.76,0.65]])\n").c_str());
        "],[0.00,0.00,0.75,0.75]])\n").c_str());
}

void RobotShell::moveRelative(float x, float y)
{
	PyRun_SimpleString(("R.moveRelative2(" +
			to_string( double(x)) +","+
			to_string( double(y)) + ")\n").c_str());
}

void RobotShell::moveRelative_0pose(float x, float y)
{
	PyRun_SimpleString(("R.moveRelative2_0pose(" +
			to_string( double(x)) +","+
			to_string( double(y)) + ")\n").c_str());
}

void RobotShell::speed(float speed)
{
    PyRun_SimpleString(("R.set_speed(["
                        +to_string(speed)+
                        ",50,50,50])\n").c_str());
}

void RobotShell::moveRelative(float x, float y, float z)
{
	PyRun_SimpleString(("R.moveRelative3(" +
			to_string( double(x)) +","+
			to_string( double(y)) +","+
			to_string( double(z)) +")\n").c_str());

}

void RobotShell::moveTo(float x, float y, float z)
{
		PyRun_SimpleString((
			"R.set_cartesian([["+
			to_string( double(x)) +"," +
			to_string( double(y)) +"," +
			to_string( double(z)) +
			"],[0,0,1,0]])\n").c_str());

}

void RobotShell::moveTo(float x, float y)
{
	PyRun_SimpleString((
			"R.set_cartesian2([["+
			to_string(double(x)) +"," +
			to_string( double(y)) +"," +
			"0],[0,0,1,0]])\n").c_str());

}

void RobotShell::flipGripper()
{
	PyRun_SimpleString("R.flip_gripper()\n");
}

 void RobotShell::pickPin()
 {

speed(100);
PyRun_SimpleString(("R.set_cartesian([["+to_string(pinPlaces.back()[0])+","+ to_string(pinPlaces.back()[1])+","+to_string(60)+"],[0,1,0,0]])\n").c_str());
PyRun_SimpleString("R.open_gripper()\n");
speed(50);
PyRun_SimpleString(("R.set_cartesian([["+to_string(pinPlaces.back()[0])+","+ to_string(pinPlaces.back()[1])+","+to_string(17)+"],[0,1,0,0]])\n").c_str());
PyRun_SimpleString("R.close_gripper()\n");
speed(100);
PyRun_SimpleString(("R.set_cartesian([["+to_string(pinPlaces.back()[0])+","+ to_string(pinPlaces.back()[1])+","+to_string(60)+"],[0,1,0,0]])\n").c_str());

pinPlaces.pop_back();

 }

 void RobotShell::placePin()
 {
            speed(20);
            moveRelative(00,00,-27);
            PyRun_SimpleString("R.open_gripper()\n");
            moveRelative(00,00,15);
            moveRelative(-10,-5);
            moveRelative(00,00,-12);
            speed(100);
            moveRelative(00,00,20);
            PyRun_SimpleString("R.close_gripper()\n");
 }
