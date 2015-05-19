 #include "Screen.h"



Screen::Screen()
{
	rc = openni::OpenNI::initialize();

	rc = device.open(openni::ANY_DEVICE);


	

	if (device.getSensorInfo(openni::SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, openni::SENSOR_DEPTH);
	}
	rc = depth.start();
	printf("Depth Screen started\n");
}


Screen::~Screen(void)
{
	exit(0);
	/*
	depth.stop();
	depth.destroy();
	device.close();

	openni::OpenNI::shutdown();
	
	printf("Depth Screen ended.\n");
	*/
}

bool Screen::getImage(openni::VideoFrameRef* depthFrame)
{
		int streamNumber;
		pStream= &depth;
		rc = openni::OpenNI::waitForAnyStream(&pStream, 1, &streamNumber);
		rc = depth.readFrame(depthFrame);
		return true;
}

int Screen::getWidth()
{
	return depth.getVideoMode().getResolutionX();
}
int Screen::getHeight()
{
	return depth.getVideoMode().getResolutionY();
}
