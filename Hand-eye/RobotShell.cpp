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


	//have config CSV file
	pos0x=250;
	pos0y=500;
	pos0z=140;


}

RobotShell::~RobotShell()
{
	PyRun_SimpleString("R.close()");
	Py_Finalize();

}

void RobotShell::moveToPosZero()
{
	PyRun_SimpleString(("R.set_cartesian([["+
        to_string(pos0x) +"," +
		to_string(pos0y) +"," +
		to_string(pos0z) +
		"],[0.25,0.25,0.75,0.75]])\n").c_str());


}

void RobotShell::moveRelative(float x, float y)
{
	PyRun_SimpleString(("R.moveRelative2(" +
			to_string( double(x)) +","+
			to_string( double(y)) + ")\n").c_str());
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

