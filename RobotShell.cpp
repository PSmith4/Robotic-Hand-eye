#include "RobotShell.h"
#include <Python.h>

RobotShell RobotShell::RobotShell()
{
	Py_Initialize();
	PyRun_SimpleString("import abb\n");
	PyRun_SimpleString("R= abb.Robot(ip='192.168.125.1')\n");
	
	//have config CSV file
	pos0x=500;
	pos0y=450;
	pos0z=500;
}

RobotShell::~RobotShell
{
	yRun_SimpleString("R.close()");
	Py_Finalize();
	
}

bool RobotShell::moveToPosZero()
{
	PyRun_SimpleString(
		"R.set_cartesian([["+
		to_string(long double (pos0x)) +"," +
		to_string(long double (pos0y)) +"," +
		to_string(long double (pos0z)) +
		"],[0,0,1,0]])\n");
	
	PyRun_SimpleString(""R.set_workobject([["+
		to_string(long double (pos0x)) +"," +
		to_string(long double (pos0y)) +"," +
		to_string(long double (pos0z)) +
		"],[0,0,1,0]])\n");
}

bool RobotShell::moveRelative(float x, float y)
{
	PyRun_SimpleString("R.moveRelative2(" +
			to_string(long double (x)) +
			to_string(long double (y)) + ")\n");
}

bool RobotShell::moveRelative(float x, float y, float z)
{
	PyRun_SimpleString("R.moveRelative3(" +
			to_string(long double (x)) +
			to_string(long double (y)) + 
			to_string(long double (z)) +")\n");
}

bool RobotShell::moveTo(float x, float y, float z)
{
		PyRun_SimpleString(
			"R.set_cartesian([["+
			to_string(long double (x)) +"," +
			to_string(long double (y)) +"," +
			to_string(long double (z)) +
			"],[0,0,1,0]])\n");
}

bool RobotShell::moveTo(float x, float y)
{
	PyRun_SimpleString(
			"R.set_cartesian2([["+
			to_string(long double (x)) +"," +
			to_string(long double (y)) +"," +
			"],[0,0,1,0]])\n");
}

bool RobotShell::flipGripper()
{
	PyRun_SimpleString("R.flip_gripper()"\n);
}

