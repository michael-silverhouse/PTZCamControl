#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp" // putText()
#include "opencv2/imgcodecs/imgcodecs.hpp" // imwrite()
#include <iostream> // cout(), cerr()
#include <fstream> // required to write left upper corner coordinate of bounding box
#include <Windows.h> // POINT data structure, GetCursorPos()
#define RED_COLOR Scalar(0, 0, 255)
#define GREEN_COLOR Scalar(0, 255, 0)
#define YELLOW_COLOR Scalar(0, 255, 255)
#define WHITE_COLOR Scalar(255, 255, 255)
#define BLUE_COLOR Scalar(255, 0, 0)
#define _CRT_SECURE_NO_WARNINGS // to avoid sprintf() error. sprintf() is used in visualize_trajectory()
#define ESCAPE 27

using namespace std;
using namespace cv;

vector<int> set_image_write_quality(const char* suffix, const int quality); /* for jpg --> pass [0-100] quality values, for png --> [0-9] compression parameters */
void open_file_stream_to_read(ifstream& fin, const string& filename);
void open_file_stream_to_write(ofstream& fout, const string& filename);
void display_OpenCV_version();
void visualize_frame_number(Mat img, const size_t i, Point& location);
void save_results(const Mat img, const size_t i, const bool saveResultsJPEG);
bool find_center(Mat img, Point2f& center);