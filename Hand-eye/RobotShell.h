#include <Python.h>
#include <vector>
#include <array>
using namespace std;
class RobotShell{

	 public:
        RobotShell();
        ~RobotShell();
        void moveToPosZero();
        void moveRelative(float x, float y);
        void moveRelative_0pose(float x, float y);

        void movefromZero(float x, float y);
        void moveRelative(float x, float y, float z);
        void moveTo(float x, float y, float z);
        void moveTo(float x, float y);
        void flipGripper();
        void pickPin();
        void placePin();
        void speed(float speed);
    private:
        double pos0x;
        double pos0y;
        double pos0z;
        vector<array<double, 2>> pinPlaces;

};
