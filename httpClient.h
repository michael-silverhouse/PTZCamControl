#pragma once
#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPCookie.h>
#include <Poco/Net/NameValueCollection.h>
#include "Poco/Net/HTTPBasicCredentials.h"
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

#include "opencv2/videoio/videoio.hpp" // VideoCapture class
#include "opencv2/imgcodecs/imgcodecs.hpp" // imread()
#include "opencv2/highgui/highgui.hpp"

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPBasicCredentials;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPCookie;
using Poco::Net::NameValueCollection;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;

class httpClient
{
private:
	URI* uri;
	HTTPClientSession* session;
	HTTPRequest* request;
	HTTPResponse* response;
	bool PTZcookie;
	std::vector<HTTPCookie>* cookies;
	NameValueCollection* nvc;
	float panInit, tiltInit;
	int zoomInit;
	int imageWidth, imageHeight;
	int compression;
public:
	httpClient(const char* bla);
	//void sendRequest(std::map<std::string, std::string>* param);
	void sendRequest(std::string req);
	void sendPTZRequest(std::string req);
	void go_to_init();
	void set_initial_conditions(float p, float t, int z);
	std::string getInitPos();
	void setPTZ(float x, float y, int z);
	void set_center(int x, int y);
	void setAreaZoom(int x, int y, int zoom);
	void setDirection(std::string dir);
	void set_image_resolution(const char* resolution);
	int get_image_width() const;
	int get_image_height() const;
	void set_compression_JPEG(const int compressionLevel);
	cv::Mat getJPEG();
	cv::VideoCapture get_motion_JPEG();
};

#endif