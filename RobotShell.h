#include <Python.h>

class RobotShell{
	 
	 public:
	 RobotShell RobotShell();
	 bool moveToPosZero();
	 bool moveRelative(float x, float y);
	 bool moveRelative(float x, float y, float z);
	 bool moveTo(float x, float y, float z);
	 bool moveTo(float x, float y);
	 bool flipGripper();
	 
	private:
		float x;
		float y;
		float z;
		
	
}
