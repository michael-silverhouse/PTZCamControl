#include "miscellaneous_functions.h"

/* for jpg --> pass [0-100] quality values, for png --> [0-9] compression parameters */
vector<int> set_image_write_quality(const char* suffix, const int quality)
{
	vector<int> compressionParameters;
	if (suffix == "jpg")
	{
		compressionParameters.push_back(IMWRITE_JPEG_QUALITY);
		compressionParameters.push_back(quality);
	}
	else if (suffix == "png")
	{
		compressionParameters.push_back(IMWRITE_PNG_COMPRESSION);
		compressionParameters.push_back(quality);
	}
	else
	{
		cerr << "Unknown suffix for writing image!" << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	return compressionParameters;
}

void open_file_stream_to_read(ifstream& fin, const string& filename)
{
	fin.open(filename, ios::in);
	if (!fin.is_open())
	{
		cerr << "Error in reading from " << filename << " file!" << endl;
		exit(EXIT_FAILURE);
	}
}

void open_file_stream_to_write(ofstream& fout, const string& filename)
{
	fout.open(filename, ios::out);
	if (!fout.is_open())
	{
		cerr << "Error in opening txt file to write!" << endl;
		exit(EXIT_FAILURE);
	}
}

void display_OpenCV_version()
{
	cout << "Compiled with OpenCV version " << CV_VERSION << endl; // thnx to Shervin Emami for this label.
}

void visualize_frame_number(Mat img, const size_t i, Point& location)
{
	char frameNumberTemplate[20] = "Frame #%i", frameNumber[20] = { '\0' };
	sprintf(frameNumber, frameNumberTemplate, i);
	putText(img, frameNumber, location, FONT_HERSHEY_SIMPLEX, 0.7, GREEN_COLOR, 2, LINE_8, false);
}

void save_results(const Mat img, const size_t i, const bool saveResultsJPEG)
{
	static vector<int> compressionParameters = set_image_write_quality("jpg", 100);
	if (saveResultsJPEG)
	{
		char outputName[50] = { '\0' };
		sprintf(outputName, "results_images/img%04d.jpg", i);
		imwrite(outputName, img, compressionParameters);
	}
}

bool find_center(Mat img, Point2f& center)
{
	bool centerFound;
	size_t n = 0;
	for (size_t y = 0; y < img.rows; y++)
		for (size_t x = 0; x < img.cols; x++)
			if (img.at<Vec3b>(y, x) != Vec3b(0, 0, 0))
			{
				n++;
				center.x += (float)x;
				center.y += (float)y;
			}
	if (n != 0)
	{
		center.x = center.x / (float)n;
		center.y = center.y / (float)n;
		centerFound = true;
	}
	else
		centerFound = false;
	return centerFound;
}