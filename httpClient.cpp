#include "httpClient.h"

httpClient::httpClient(const char* bla)
{
	PTZcookie = false;
	panInit = 0.0;
	tiltInit = 180.0;
	zoomInit = 1;

	uri = new URI(bla);
	std::string path(uri->getPathAndQuery());
	if (path.empty()) path = "/";
	
	session = new HTTPClientSession(uri->getHost(), uri->getPort()); // This class implements the client-side of a HTTP session.
	// Create a HTTPRequest object, fill it accordingly, and pass it as argument to the sendRequest() method. 
	request = new HTTPRequest(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
	request->setHost(uri->getHost());
	HTTPBasicCredentials cred("root", "pass");
	cred.authenticate(*request);
	// https://msdn.microsoft.com/en-us/library/ms537503(v=vs.85).aspx
	//request->set("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:49.0) Gecko/20100101 Firefox/49.0");
	//request->set("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"); // tells the server what media types are okay to send
	//request->set("Accept-Language", "en-us"); // "fr,en-us;q=0.7,en;q=0.3"
	//request->set("Accept-Encoding", "gzip,deflate");
	//request->set("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
	request->set("Connection", "keep-alive");
	//request->set("Cookie","ptz_ctl_id=41881");
	response = new HTTPResponse();
	

	sendRequest(path);
	if (response->getStatus() == 200)
		std::cout << "Camera " << uri->getHost() << " is available" << std::endl;
	else if (response->getStatus() == 401)
	{
		std::cout << "response->getStatus() = " << response->getStatus() << std::endl;
		std::cout << "response->getReason() = " << response->getReason() << std::endl;
		std::cout << "response->getReason() = " << response->hasToken("WWW-Authenticate", "Basic") << std::endl;
		std::cout << "The server rejects the request with a 401 status, indicating that the user needs to provide his username and password.";
	}
	else
	{
		std::cout << "response->getStatus() = " << response->getStatus() << std::endl;
		std::cout << "response->getReason() = " << response->getReason() << std::endl;
	}
}

//void httpClient::sendRequest(std::map<std::string, std::string>* param)
//{
//	std::string req = "axis-cgi/com/ptz.cgi?";
//	std::map<std::string, std::string>::iterator idx_param;
//	for (idx_param = param->begin(); idx_param != param->end(); idx_param++)
//		req += idx_param->first + "=" + idx_param->second + "&";
//	sendRequest(req);
//}

void httpClient::sendRequest(std::string req)
{
	try
	{
		request->setURI(req);
		std::string scheme, authInfo;
		request->getCredentials(scheme, authInfo);
		session->sendRequest(*request);
		session->receiveResponse(*response);
	}
	catch (Exception& exc)
	{
		std::cerr << "Taha DEBUG#" << std::endl;
		std::cerr << exc.displayText() << std::endl;
	}
}

void httpClient::sendPTZRequest(std::string req)
{
	try
	{
		/*http request*/
		request->setURI("/axis-cgi/com/ptz.cgi?camera=1" + req);
		/*get ptz cookies and complete http request*/
		if(!PTZcookie)
		{
			cookies = new std::vector<HTTPCookie>;
			session->sendRequest(*request);
			session->receiveResponse(*response);
			response->getCookies(*cookies);
			PTZcookie = true;
			NameValueCollection nvc;
			std::vector<HTTPCookie>::iterator it = cookies->begin();
			while (it != cookies->end())
			{
				nvc.add((*it).getName(), (*it).getValue());
				it++;
			}
			request->setCookies(nvc);
		}
		/*send full http request*/
		session->sendRequest(*request);
		session->receiveResponse(*response);
	}
	catch (Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}
}

void httpClient::go_to_init()
{
	std::stringstream out;
	out << "&pan=" << panInit << "&tilt=" << tiltInit << "&zoom=" << zoomInit;
	std::string req = out.str();
	sendPTZRequest(req);

	/*if(response->getStatus() == 204)
		std::cout << "Move OK" << std::endl;
	else
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;*/
}

void httpClient::set_initial_conditions(float p, float t, int z)
{
	panInit = p;
	tiltInit = t;
	zoomInit = z;
}

std::string httpClient::getInitPos()
{
	std::stringstream out;
	out << "(" << panInit << "," << tiltInit << "," << zoomInit << ")";
	std::string initPos = out.str();
	return initPos;
}

void httpClient::setPTZ(float x, float y, int z)
{
	std::stringstream out;
	out << "pan=" << x << "&tilt=" << y << "&zoom=" << z;
	std::string req = "&" + out.str();
	sendPTZRequest(req);

	/*if(response->getStatus() == 204)
		std::cout << "Move OK - pan=" << x << ",tilt=" << y << ",zoom=" << z << std::endl;
	else
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;*/
}

void httpClient::set_center(int x, int y)
{
	std::string s;
	std::stringstream out;
	char widthText[5] = { '\0' }, heightText[5] = { '\0' };
	sprintf(widthText, "%i", this->imageWidth);
	sprintf(heightText, "%i", this->imageHeight);
	out << x << "," << y << "&speed=100&imagewidth=" << widthText <<"&imageheight=" << heightText;
	s = out.str();
	std::string req = "&center=" + s;

	sendPTZRequest(req);

	/*if(response->getStatus() == 204)
		std::cout << "Move OK - center : " << s << std::endl;
	else
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;*/
}

void httpClient::setAreaZoom(int x, int y, int zoom)
{
	std::string s;
	std::stringstream out;
	out << x << "," << y << "," << zoom;
	s = out.str();
	std::string req = "&areazoom=" + s;
	sendPTZRequest(req);
	if (response->getStatus() == 204)
		std::cout << "Move OK - areaZoom : " << s << std::endl;
	else
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;
}

void httpClient::setDirection(std::string dir)
{
	std::string req = "&move=" + dir;
	sendPTZRequest(req);
	if(response->getStatus() == 204)
		std::cout << "Move OK" << std::endl;
	else
		std::cout << response->getStatus() << " " << response->getReason() << std::endl;
}

void httpClient::set_image_resolution(const char* resolution)
{
	char widthText[5] = { '\0' }, heightText[5] = { '\0' };
	int i = 0, j = 0;
	while (resolution[i] != 'x')
	{
		widthText[i] = resolution[i];
		i++;
	}
	i++;
	while (resolution[i] != '\0')
	{
		heightText[j] = resolution[i];
		i++;
		j++;
	}
	this->imageWidth = atoi(widthText);
	this->imageHeight = atoi(heightText);
}

int httpClient::get_image_width() const
{
	return imageWidth;
}

int httpClient::get_image_height() const
{
	return imageHeight;
}

void httpClient::set_compression_JPEG(const int compressionLevel)
{
	compression = compressionLevel;
}
/* Video Streaming API, page 11: jpg/image.cgi is used to request a JPEG image (snapshot). A JPEG image (snapshot) should only be used 
   when requiring less than 1 fps. */

cv::Mat httpClient::getJPEG()
{
	std::stringstream out;
	char imgResolutionText[10] = { '\0' };
	sprintf(imgResolutionText, "%ix%i", this->imageWidth, this->imageHeight);
	out << "http://root:pass@" + uri->getHost() + "/axis-cgi/jpg/image.cgi?resolution=" + imgResolutionText;
	std::string fileName = out.str();
	cv::Mat img = cv::imread(fileName.c_str());
	return img;
}

/* Motion JPEG Video CGI Request */
cv::VideoCapture httpClient::get_motion_JPEG()
{
	std::stringstream out;
	char imgResolutionText[10] = { '\0' }, compressionText[4] = {'\0'};
	sprintf(imgResolutionText, "%ix%i", this->imageWidth, this->imageHeight);
	sprintf(compressionText, "%i", this->compression);
	/* <scheme>://<user>:<password>@<host>:<port>/<path>;<params>?<query>#<frag> */
	out << "http://root:pass@" + uri->getHost() + "/axis-cgi/mjpg/video.cgi?resolution=" + imgResolutionText + 
		   "&rotation=180&compression=" + compressionText + "&fps=0";
	std::string fileName = out.str();
	std::cout << fileName << std::endl;
	cv::VideoCapture record = cv::VideoCapture(fileName.c_str());
	//record.set(CV_CAP_PROP_BUFFERSIZE, 3);
	return record;
}