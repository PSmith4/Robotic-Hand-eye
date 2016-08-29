#include "opencv2/core/core.hpp"
using namespace cv;
class PinHole{
	public:
		PinHole(Point2f _location, Mat& container);


		inline bool isFilled(){return filled;}

		inline void request(){ requested = true;}
		inline void unrequest(){requested=false;}
		inline bool isRequested(){return requested;}

		inline Point2f getLocation(){return location;}
		inline void setLocation(Point2f _location){location=_location;}
		void draw();
	private:
		bool filled;
		bool requested;
		Point2f location;
		Mat image;
		float radius;

};
