#pragma once
#include <OpenNI.h>
class Screen
{
public:
	Screen();
	~Screen(void);
	bool getImage(openni::VideoFrameRef* depthFrame);
	int Screen::getWidth();
	int Screen::getHeight();
private:
	openni::Status rc;
	openni::Device device;
	openni::VideoStream depth;	
	openni::VideoFrameRef frame;
	openni::VideoStream* pStream;
};

