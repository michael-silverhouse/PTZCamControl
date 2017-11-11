/*
  Project: ptz camera http client and server
  File: main.cpp
  Written by: M. Taha Koroglu
  Code started on: 9/8/2016
  Synopsis: This program creates a connection w/ Axis Communications P5624-E network PTZ camera first.
  Then it sends http requests to start MJPEG stream. After initializing the stream, it displays 
  real-time video stream in a display provided by OpenCV C++ libraries.
  Run IPUtility.exe to view IP address of the network camera.
*/

#include "httpClient.h"
#include "miscellaneous_functions.h"
#include "GabrielTimer.h"
#include <iomanip>

using namespace std;
using namespace cv;

int main()
{
	httpClient* http = new httpClient("http://169.254.142.115"); // see camera's IP in "Axis IP Utility" program
	http->set_initial_conditions( (float)-100.0, (float)-10.0, 1 ); // arguments are (pan, tilt, zoom)
	http->go_to_init();
	http->set_image_resolution("800x450"); // {1280x720, 800x450, 480x270, 320x180}
	http->set_compression_JPEG(20); // [0-100] - 0 means no compression, 100 means max compression. FPS is proportional to compression.
	VideoCapture cap = http->get_motion_JPEG();
	if (!cap.isOpened())
	{
		cerr << "Problem with VideoCapture!!!" << endl;
		system("pause");
		return EXIT_FAILURE;
	}
	
	// Define the range of "blue" color in HueSaturationValue colorspace
	Scalar lowerLimit = Scalar(60, 100, 100);
	Scalar upperLimit = Scalar(180, 255, 255);
	//POINT mouse;
	size_t i = 0;
	GabrielTimer timer; // initializes tic and toc as 0
	Mat frame(Size(http->get_image_width(), http->get_image_height()), CV_8UC3);

	for (;;)
	{
		Mat hsvImg, mask, filteredImg;
		cap >> frame;
		i++;
		timer.capture_time(); // microsecond accuracy
		if (i > 1)
		{
			timer.differentiate();
			cout << "FPS = " << fixed << setprecision(2) << timer.fps();
		}
		//GetCursorPos(&mouse);
		cout << " Frame #"<< i << endl;
		cvtColor(frame, hsvImg, COLOR_BGR2HSV); // Convert to HSV colorspace. Note that OpenCV imports RGB images as BGR.
		// Threshold the HSV image to get only blue color
		inRange(hsvImg, lowerLimit, upperLimit, mask);
		// Compute bitwise-AND of input image and mask
		bitwise_and(frame, frame, filteredImg, mask = mask);
		// Run median filter on the output to smoothen it
		medianBlur(filteredImg, filteredImg, 9);
		Point2f center(0.0, 0.0);
		bool centerFound = find_center(filteredImg, center);
		//if (centerFound)
		//{
		//	drawMarker(frame, center, RED_COLOR, MARKER_CROSS, 20, 2, 8);
		//	cout << "Center location = (" << center.x << "," << center.y << ")" << endl;
		//	/*if (i % 10 == 0)
		//		http->set_center(800 - center.x, 450 - center.y);*/
		//}
		//else
		//	cout << "Center NOT FOUND!" << endl;

		//visualize_frame_number(frame, i, Point(620, 20)); // Point(1110, 20)
		
		//if (i % 50 == 25)
		//	//http->set_center(http->get_image_width() - 5, http->get_image_height() - 309);
		//	http->setPTZ(-20.0, -10.0, 1);
		//else if (i % 50 == 0)
		//	http->go_to_init();
		//else if (i > 300)
		//{
		//	//if (i % 100 == 0)
		//		http->set_center(800 - mouse.x, 450 - mouse.y);
		//}
		//drawMarker(frame, Point(frame.cols / 2, frame.rows / 2), Scalar(0, 255, 0), MARKER_CROSS, 20, 2, 8);
		//drawMarker(frame, Point(mouse.x, mouse.y), RED_COLOR, MARKER_TILTED_CROSS, 10, 2, LINE_8);
		////resize(frame, frame, Size(), scalingFactor, scalingFactor, INTER_AREA);
		//imshow("Motion JPEG stream", frame);
		//moveWindow("Motion JPEG stream", 680, 20);
		imshow("Filtered output", filteredImg);
		moveWindow("Filtered output", 680, 420);
		if (waitKey(30) == ESCAPE) break;
	}

	delete http;
	return EXIT_SUCCESS;
}