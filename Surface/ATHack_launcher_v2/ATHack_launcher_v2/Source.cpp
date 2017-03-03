#include "opencv2/opencv.hpp" //opencv
#include <cv.h> //opencv
#include <highgui.h> //opencv
#include <math.h>       /* sin, cos */
#include <iostream> // cout, endl, ...

#include "wtypes.h"  // needed to get the screen size
#include <algorithm>    // std::min

using namespace std;
using namespace cv;


#define PI 3.14159265

using namespace cv;

/// Global Variables
const int distance_max = 100;
const int angle_max = 100;

int distance_var;
int angle = angle_max / 2; //angle as shown on the slider

int frame_width;
int frame_height;
double cannon_length, cannon_start_length;
int thickness = 40;
int lineType = CV_AA;
Point cannon_start, cannon_end, cannon_rotation_center;
Scalar cannon_color = Scalar(150, 150, 150);
double angle_range = 90; //range of cannon motion in degrees
double true_angle; //geometric angle in degrees for the aiming direction

double cannon_angle = 45; // angle between the ground and the cannon. this is used to compute the projection of the cannon into the plane of the viewing device; assuming that there is a 90degree angle between ground and camera


Point3d cannon_tip3D;
Point3d cannon_start3D;

/// Matrices to store images
Mat frame;

// launch button stuff
Point button_position;
double button_size = 0;
int button_offset = 20; // distance from image border to button
Scalar button_color = Scalar(170, 0, 0);

//exit button stuff
Point exit_button_position; //center position
Scalar exit_button_color = Scalar(0, 0, 170);
int exit_button_line_thickness = 10;

Point exit_button_TR; //Top Right corner of exit button
Point exit_button_TL;
Point exit_button_BR;
Point exit_button_BL;
bool exit_program = false;

// callback to track mouse clicks
void mouseCallback(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN) // Left button of the mouse is clicked 
	{
		//check whether the click was on the launch button
		if ((button_position.x - x)*(button_position.x - x) + (button_position.y - y)*(button_position.y - y) <(button_size / 2)*(button_size / 2)){

			std::cout << "Launch: (distance " << distance_var << ", angle " << angle << ")" << std::endl;

		}
		else if (abs(exit_button_position.x - x) <= button_size / 2 && abs(exit_button_position.y - y) <= button_size / 2)
		{
			exit_program = true;
		}

	}
	else if (event == EVENT_RBUTTONDOWN) //Right button of the mouse is clicked
	{

	}
	else if (event == EVENT_MBUTTONDOWN) // Middle button of the mouse is clicked 
	{

	}
	else if (event == EVENT_MOUSEMOVE) // Mouse move over the window
	{

	}
}


/**
* @function distance_trackbar
* @brief Callback for trackbar
*/
void distance_trackbar(int, void*)
{

}
void angle_trackbar(int, void*)
{
	true_angle = angle_range / angle_max * (angle - angle_max / 2);

	//cannon end point projection
	cannon_tip3D.x = cannon_length*cos(true_angle*PI / 180)*cos(cannon_angle*PI / 180);
	cannon_tip3D.y = cannon_length*sin(true_angle*PI / 180)*cos(cannon_angle*PI / 180);
	cannon_tip3D.z = cannon_length*sin(cannon_angle*PI / 180);
	cannon_end.x = int(cannon_rotation_center.x + cannon_tip3D.y);
	cannon_end.y = int(cannon_rotation_center.y - cannon_tip3D.z);

	//cannon start point projection
	cannon_start3D.x = cannon_start_length*cos(true_angle*PI / 180)*cos(cannon_angle*PI / 180);
	cannon_start3D.y = cannon_start_length*sin(true_angle*PI / 180)*cos(cannon_angle*PI / 180);
	cannon_start3D.z = cannon_start_length*sin(cannon_angle*PI / 180);
	cannon_start.x = int(cannon_rotation_center.x + cannon_start3D.y);
	//cannon_start.y = int(cannon_rotation_center.y - cannon_start3D.z);
	cannon_start.y = frame_height;

	/*
	//draw cannon shadow
	line(frame,
	cannon_start,
	cannon_end,
	Scalar(100, 100, 100),
	thickness + 10,
	lineType);
	*/

	//draw cannon outline
	line(frame,
		cannon_start,
		cannon_end,
		Scalar(0, 0, 0),
		thickness + 4,
		lineType);

	//draw cannon
	line(frame,
		cannon_start,
		cannon_end,
		cannon_color,
		thickness,
		lineType);

	/*
	//draw cannon base
	line(frame,
	cannon_rotation_center,
	cannon_rotation_center,
	cannon_color,
	thickness+50,
	lineType);
	*/

	//draw launch button
	line(frame,
		button_position,
		button_position,
		Scalar(0, 0, 0),
		button_size + 4,
		lineType);
	line(frame,
		button_position,
		button_position,
		button_color,
		button_size,
		lineType);

	// xx make overlay with alpha blending for faster buttons
	//draw exit button
	rectangle(frame,
		exit_button_TL,
		exit_button_BR,
		exit_button_color,
		exit_button_line_thickness,
		lineType);

	line(frame,  //xx offset this cross to make this prettier
		exit_button_TL,
		exit_button_BR,
		exit_button_color,
		exit_button_line_thickness,
		lineType);
	line(frame,
		exit_button_BL,
		exit_button_TR,
		exit_button_color,
		exit_button_line_thickness,
		lineType);


	imshow("CameraView", frame);
}

// callback to get screen size from https://cppkid.wordpress.com/2009/01/07/how-to-get-the-screen-resolution-in-pixels/
// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}



int main(int argc, char** argv)
{
	// get the size of the screen
	int screen_horizontal = 0;
	int screen_vertical = 0;
	GetDesktopResolution(screen_horizontal, screen_vertical);

	// initialize camera
	VideoCapture cap;
	// open the default camera, use something different from 0 otherwise;
	if (!cap.open(0)){ // fail safe
	
		std::cout << "no camera" << std::endl;
		return 0;

	}

	//get the size of the image delivered from the camera
	cap >> frame;
	frame_height = frame.size().height;
	frame_width = frame.size().width;

	/// Create display window
	namedWindow("CameraView", CV_WINDOW_NORMAL);

	int screen_ratio = floor(min(screen_horizontal / frame_width, screen_vertical / frame_height));

	if (screen_ratio >= 1){
		resizeWindow("CameraView", screen_ratio*frame_width, screen_ratio*frame_height);
	}
	else{			// dont do anything if the camera frame is larger than the screen - let openCV handle this xx should maybe be taken care of

	}




	//set the length of the animated cannon to a third of the frame height
	cannon_length = frame_height / 2.5;
	cannon_start_length = cannon_length / 3;
	cannon_rotation_center.x = frame_width / 2;
	cannon_rotation_center.y = frame_height;
	thickness = frame_width / 15;

	//buttons
	button_size = frame_height / 5;
	//launch button
	button_position.x = 0 + button_offset + button_size / 2;
	button_position.y = frame_height - button_offset - button_size / 2;
	//exit button
	exit_button_position.x = frame_width - button_offset - button_size / 2; //this is the center of the button
	exit_button_position.y = 0 + button_offset + button_size / 2;

	exit_button_TL.x = exit_button_position.x - button_size / 2;
	exit_button_TL.y = exit_button_position.y - button_size / 2;
	exit_button_TR.x = exit_button_position.x - button_size / 2;
	exit_button_TR.y = exit_button_position.y + button_size / 2;
	exit_button_BL.x = exit_button_position.x + button_size / 2;
	exit_button_BL.y = exit_button_position.y - button_size / 2;
	exit_button_BR.x = exit_button_position.x + button_size / 2;
	exit_button_BR.y = exit_button_position.y + button_size / 2;

	// create trackbar for distance
	createTrackbar("Distance", "CameraView", &distance_var, distance_max, distance_trackbar);
	// create trackbar for angle
	createTrackbar("Angle", "CameraView", &angle, angle_max, angle_trackbar);

	setMouseCallback("CameraView", mouseCallback, NULL);

	for (;;)
	{
		cap >> frame;
		if (frame.empty()) break; // end of video stream

		distance_trackbar(distance_var, 0);
		angle_trackbar(distance_var, 0);


		if (waitKey(10) == 27 || exit_program) break; // stop capturing by pressing ESC 
	}
	// the camera will be closed automatically upon exit
	// cap.close();
	return 0;
}