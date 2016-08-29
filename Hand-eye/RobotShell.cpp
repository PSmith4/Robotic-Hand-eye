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
    PyRun_SimpleString("R.set_tool([[0,0,105], [1,0,0,0]])\n");
    PyRun_SimpleString("R.get_tool()\n");
	//have config CSV file
	pos0x=370;
	pos0y=550;
	pos0z=100;


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
		"],[0.05,0.01,0.76,0.65]])\n").c_str());


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

