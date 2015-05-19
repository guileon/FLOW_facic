#include "ColorScreen.h"



ColorScreen::ColorScreen()
{
	rc = openni::OpenNI::initialize();

	rc = device.open(openni::ANY_DEVICE);


	

	if (device.getSensorInfo(openni::SENSOR_COLOR) != NULL)
	{
		rc = color.create(device, openni::SENSOR_COLOR);
	}
	rc = color.start();
	printf("Color Screen started\n");
}


ColorScreen::~ColorScreen(void)
{
	exit(0);
	/*
	color.stop();
	color.destroy();
	device.close();

	openni::OpenNI::shutdown();
	
	printf("Color Screen ended.\n");
	*/

}

bool ColorScreen::getImage(openni::VideoFrameRef* depthFrame)
{
		int streamNumber;
		pStream= &color;
		rc = openni::OpenNI::waitForAnyStream(&pStream, 1, &streamNumber);
		rc = color.readFrame(depthFrame);
		return true;
}

int ColorScreen::getWidth()
{
	return color.getVideoMode().getResolutionX();
}
int ColorScreen::getHeight()
{
	return color.getVideoMode().getResolutionY();
}
