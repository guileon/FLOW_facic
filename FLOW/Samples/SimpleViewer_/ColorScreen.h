#pragma once
#include <OpenNI.h>
class ColorScreen
{
public:
	ColorScreen();
	~ColorScreen(void);
	bool getImage(openni::VideoFrameRef* depthFrame);
	int ColorScreen::getWidth();
	int ColorScreen::getHeight();
private:
	openni::Status rc;
	openni::Device device;
	openni::VideoStream color;	
	openni::VideoFrameRef frame;
	openni::VideoStream* pStream;
};