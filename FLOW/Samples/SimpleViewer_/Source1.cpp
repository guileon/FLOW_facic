// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <OpenNI.h>
#include <GL/glut.h>

#define MIN_NUM_CHUNKS(data_size, chunk_size)	((((data_size)-1) / (chunk_size) + 1))
#define MIN_CHUNKS_SIZE(data_size, chunk_size)	(MIN_NUM_CHUNKS(data_size, chunk_size) * (chunk_size))


openni::VideoStream**	streams;
openni::RGB888Pixel*	texMap;
openni::RGB888Pixel*	screen;
openni::DepthPixel*		dscreen;
openni::VideoStream depth, color;
openni::VideoFrameRef depthf,colorf;

void display()
{
	int changedIndex;
	openni::Status rc = openni::OpenNI::waitForAnyStream(streams, 2, &changedIndex);
	switch (changedIndex)
	{
	case 0:
		depth.readFrame(&depthf); break;
	case 1:
		color.readFrame(&colorf); break;
	default:
		printf("Error in wait\n");
	}

	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 640, 480, 0, -1.0, 1.0);

}



void glutIdle()
{
	glutPostRedisplay();
}
void glutDisplay()
{
	display();
}


void initOpenGL(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow ("Kinect");
	// 	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);


}



int _tmain(int argc, char** argv)
{
	openni::Status rc = openni::STATUS_OK;
	openni::Device device;
	const char* deviceURI = openni::ANY_DEVICE;

	// openNI start

	rc = openni::OpenNI::initialize();

	rc = device.open(deviceURI);

	rc = depth.create(device, openni::SENSOR_DEPTH);
	rc = depth.start();

	rc = color.create(device, openni::SENSOR_COLOR);
	rc = color.start();

	// working

	openni::VideoMode depthVideoMode = depth.getVideoMode();
	openni::VideoMode colorVideoMode = color.getVideoMode();

	int depthw = depthVideoMode.getResolutionX();
	int depthh = depthVideoMode.getResolutionY();
	int colorw = colorVideoMode.getResolutionX();
	int colorh = colorVideoMode.getResolutionY();

	streams = new openni::VideoStream*[2];
	streams[0] = &color;
	streams[1] = &depth;

	unsigned int m_nTexMapX = MIN_CHUNKS_SIZE(depthw, 512);
	unsigned int m_nTexMapY = MIN_CHUNKS_SIZE(depthh, 512);

	texMap = new openni::RGB888Pixel[m_nTexMapX * m_nTexMapY];
	screen = new openni::RGB888Pixel[m_nTexMapX * m_nTexMapY];
	dscreen = new openni::DepthPixel[m_nTexMapX * m_nTexMapY];

	for(unsigned int i=0 ; i<m_nTexMapX * m_nTexMapY ; i++)
	{
		screen[i].r = 20;
		screen[i].g = 0;
		screen[i].b = 0;
		dscreen[i] = 0;
	}
	initOpenGL(argc,argv);
	glutMainLoop();
	return 0;
}

