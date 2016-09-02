#include <Python.h>

class RobotShell{

	 public:
        RobotShell();
        ~RobotShell();
        void moveToPosZero();
        void moveRelative(float x, float y);
        void moveRelative(float x, float y, float z);
        void moveTo(float x, float y, float z);
        void moveTo(float x, float y);
        void flipGripper();
        void pickPin();
    private:
        double pos0x;
        double pos0y;
        double pos0z;
};
