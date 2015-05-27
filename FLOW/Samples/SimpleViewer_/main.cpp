
#include <stdio.h>
#include <fstream>
#include <streambuf>
#include <GL/glew.h>


#include <OpenNI.h>
#include <GL/glut.h>
#include <Screen.h>
#include <ColorScreen.h>
#include <math.h>


/*
GLEW AND SHADER LOADING....
*/




Screen dScreen;
ColorScreen cScreen;


openni::RGB888Pixel*	screen;
int * fscreen;
openni::VideoFrameRef frame;
openni::VideoFrameRef frameColor;



int result[640][640];

GLint eyePosLocation;
GLint lightPosLocation;
GLint zstrengthLocation;

int depthw = 640;
int depthh = 480;

int mouse[2];

float cameraX = 10000.0f;
float cameraY = 0.0f;
float cameraZ = 0.0f;

// DATA ACESSING FUNCTIONS
#define D(i,j) (200+(float)-fscreen[i+j*depthw])
#define C(i,j) (screen[i+j*depthw])

// VARIABLES THIS PART IS RIDICULOUS, EVERYTHING IS
float viewAngle = 0;

bool isDrawFace = true;
bool isDrawColor = true;
float cval =18000;
float zstrength =20; // normal z strength
int maxDistance= 100000;
int offset_ = 3;
#define MAX_DISTANCE maxDistance // max viewing distance
#define OFFSET offset_ // offset in point sampling
#define DISTANCE 10 // distance in camera movement
float lightPosition[4] = { -10000,-10000,-100, 1 };
#define CVALUE cval
// CONSTANTS
#define X_DIRECTION 1 
#define Y_DIRECTION 2
#define X 0
#define Y 1
#define Z 2
#define W 3
#define ZSTRENGTH zstrength


// function that updates the sensor data
void draw()
{
	
	if(dScreen.getImage(&frame) && cScreen.getImage(&frameColor))
	{
		openni::DepthPixel* pDepthRow = (openni::DepthPixel*)frame.getData();
		openni::RGB888Pixel* pColorRow = (openni::RGB888Pixel*)frameColor.getData();
		int rowSize = frame.getStrideInBytes() / sizeof(openni::DepthPixel);
		int rowSizeColor = frameColor.getStrideInBytes() / sizeof(openni::RGB888Pixel);

		openni::RGB888Pixel* pScreen = screen;
		int * pfscreen = fscreen;
		for (int y = 0; y < frame.getHeight(); ++y)
		{
			const openni::RGB888Pixel* pColor = pColorRow;
			const openni::DepthPixel* pDepth = pDepthRow;

			for (int x = 0; x < frame.getWidth(); ++x, ++pDepth , ++pColor, ++pScreen, ++pfscreen)
			{
				*pfscreen = (int)*pDepth;
				*pScreen = *pColor;
			}

			pDepthRow += rowSize;
			pColorRow += rowSizeColor;
		}
	}

}

// function to acess the color sensor data
openni::RGB888Pixel getColor(int i, int j)
{
	if(i<depthw && i >=0 && j<depthh && j>=0)
	{
		return C(i,j);
	}
	else
	{
		return C(0,0);
	}
}

// gets the depth and interpolate if it is a floating number
float getDepth(float i, float j)
{
	float di,fi;
	float dj,fj;
	float daux1, daux2;
	if(i<depthw && i >=0 && j<depthh && j>=0)
	{
		di = floor(i);
		fi = i-di;
		dj = floor(j);
		fj = j-dj;
		if(fi>0)
			if(fj>0) //i and j are float
			{
				daux1 = 
					fi*D((int)i,(int)j)+
					(1-fi)*D((int)i+1,(int)j);
				daux2 =
					fi*D((int)i,(int)j+1)+
					(1-fi)*D((int)i+1,(int)j+1);
				return 
					fj*daux1 + 
					(1-fj)*daux2;

			}
			else // i is float
			{
				return
					fi*D((int)i,(int)j) +
					(1-fi)*D((int)i+1,(int)j);
			}
		else if(fj>0) // j is float
		{
			return 
				fj*D((int)i,(int)j) +
				(1-fj)*D((int)i,(int)j+1);
		}
		else // nonse is float
		{
			return D((int)i,(int)j);
		}
	}
	else
	{
		return 0;
	}
}

// function to acess the depth sensor data
float getDepth(int i, int j)
{
	if(i<depthw-1 && i >0 && j<depthh && j>0)
	{
		return D(i,j);
	}
	else
	{
		return 0;
	}
}

// funtction to apply sobel filtering
float applySobel(int dir, int x, int y)
{
	float retvalue = 0;
	if(dir == X_DIRECTION)
	{
		retvalue = 
			-1*getDepth(x-1,y-1)+
			1*getDepth(x+1,y-1)+
			-2*getDepth(x-1,y)+
			2*getDepth(x+1,y)+
			-1*getDepth(x-1,y+1)+
			1*getDepth(x+1,y+1);
	}
	else if(dir == Y_DIRECTION)
	{
		retvalue = 
			1*getDepth(x-1,y-1)+
			2*getDepth(x,y-1)+
			1*getDepth(x+1,y-1)+
			-1*getDepth(x-1,y+1)+
			-2*getDepth(x,y+1)+
			-1*getDepth(x+1,y+1);

	}
	return retvalue;
}

// normalize vector
void normalize(float * vector)
{
	float length = sqrt(
		(vector[X]*vector[X])+
		(vector[Y]*vector[Y])+
		(vector[Z]*vector[Z]));
	vector[X] = vector[X]/length;
	vector[Y] = vector[Y]/length;
	vector[Z] = vector[Z]/length;
}

#define SHADER_SOBEL true

// draw a vertex, calculating its normal
void drawVertex(int i, int j, int offset=1)
{
	//openni::RGB888Pixel c = getColor(i,j);
	float d = getDepth(i,j);
	if(-d<MAX_DISTANCE && -d>0)
	{
		//float normal[3];
		float point[3] = {(i-depthw/2),(depthh/2-j),d};

		//if(SHADER_SOBEL)
		//{
			glColor4f(getDepth(i-offset,j-offset),getDepth(i-offset,j+offset),getDepth(i+offset,j-offset),getDepth(i+offset,j+offset));
			glNormal3f(getDepth(i-offset,j),getDepth(i+offset,j),getDepth(i,j-offset));
		//}
		/*else
		{
			normal[X] = applySobel(X_DIRECTION,i,j);
			normal[Y] = applySobel(Y_DIRECTION,i,j);
			normal[Z] = ZSTRENGTH;
			normalize(normal);
			glNormal3f(normal[X],normal[Y],normal[Z]);
		}*/

		glVertex4f(
			point[X],
			point[Y],
			point[Z],
			getDepth(i,j+offset));
	}
}

int step = 0;
#define MAX_STEP 60
#define KDEPTH 1000

bool aquiredBG = false;


//unsigned char * data;

void setupGlew()
{
/*
NEED TO HAVE GLEW WORKING, OBVIOUSLY
*/
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

GLuint program;
void useShader(char * fragPath, char * vertPath)
{

	const GLchar* fragmentSource;
	const GLchar* vertexSource;
/*
NEED TO INCLUDE:
#include <fstream>
#include <streambuf>
*/


	GLuint fragmentShader;
	GLuint vertexShader;

	GLint compiled;
	GLint blen = 0;	
	GLsizei slen = 0;
	GLint linked;

/*
LOADING SOURCES
*/

	std::ifstream fragt(fragPath);
	std::string fragStr((std::istreambuf_iterator<char>(fragt)),
					 std::istreambuf_iterator<char>());

	fragmentSource = fragStr.c_str();


	std::ifstream vertt(vertPath);
	std::string vertStr((std::istreambuf_iterator<char>(vertt)),
					 std::istreambuf_iterator<char>());

	vertexSource = vertStr.c_str();

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

/* 
COMPILING FRAGMENT SHADER
*/
	glShaderSourceARB(fragmentShader,1,&fragmentSource,0);
	glCompileShaderARB(fragmentShader);

	glGetObjectParameterivARB(fragmentShader,GL_COMPILE_STATUS,&compiled);

	if (!compiled)
	{
		glGetShaderiv(fragmentShader,GL_INFO_LOG_LENGTH ,&blen);
		printf("Problem compiling fragment shader...\n");
		if (blen > 1)
		{
			 GLchar* compiler_log = (GLchar*)malloc(blen);
			 glGetInfoLogARB(fragmentShader, blen, &slen, compiler_log);
			 printf("%s\n",compiler_log);
			 free (compiler_log);
		}
	}  

/*
COMPILING VERTEX SHADER
*/
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSourceARB(vertexShader,1,&vertexSource,0);
	glCompileShaderARB(vertexShader);

	glGetObjectParameterivARB(vertexShader,GL_COMPILE_STATUS,&compiled);

	if (!compiled)
	{
		glGetShaderiv(vertexShader,GL_INFO_LOG_LENGTH ,&blen);
		printf("Problem compiling vertex shader...\n");
		if (blen > 1)
		{
			 GLchar* compiler_log = (GLchar*)malloc(blen);
			 glGetInfoLogARB(vertexShader, blen, &slen, compiler_log);
			 printf("%s\n",compiler_log);
			 free (compiler_log);
		}
	}  

/*
LINKING
*/

	program = glCreateProgram();
	glAttachShader(program,fragmentShader);
	glAttachShader(program,vertexShader);
	glLinkProgram(program);

	glGetProgramiv(program,GL_LINK_STATUS,&linked);
	if(!linked)
	{
		printf("Problem linking vertex shader...\n");
	}

}


GLuint textureName;

/**************************************************************************************************************

LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI LEIA AQUI

***************************************************************************************************************/


/*
LE ESSA PARTE, TEM TUDO QUE PRECISA PRA USAR
AQUI ELE TA LENDO A PROFUNDIDADE E A COR PRA DESENHAR NA TELA
É SÓ REUTILIZAR
BEIJOS
*/
void display()
{
	// Clear Screen and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		     
	glLoadIdentity();
	glLightfv(GL_LIGHT0,GL_POSITION,lightPosition);
	// Define a viewing transformation
	cameraX = 500*sin(viewAngle);
	cameraZ = -500 + 500*cos(viewAngle);
	gluLookAt( 
		cameraX,cameraY,cameraZ, 
		0,0,-500, 
		0,1,0);	

	lightPosition[X] = (mouse[X]-320);
	lightPosition[Y] = -(mouse[Y]-240);

	draw();

	//glUseProgram(program);
	/*
	glUniform3f(eyePosLocation,cameraX,cameraY,cameraZ);
	glUniform3f(lightPosLocation,lightPosition[X],lightPosition[Y],lightPosition[Z]);
	glUniform1f(zstrengthLocation,zstrength);
	*/
	glBegin(GL_POINTS);
	for(int i=0 ; i<depthw-1 ; i+=OFFSET)
	{
		for(int j=0 ; j<depthh-1 ; j+=OFFSET)
		{
			float point[3] = {(i-depthw/2),(depthh/2-j),getDepth(i,j)};

			glColor3f(
				((float)getColor(i,j).r)/255,
				((float)getColor(i,j).g)/255,
				((float)getColor(i,j).b)/255);
			glVertex3f(point[X],point[Y],point[Z]);
		}
		
	}
	glEnd();
	
	
	glUseProgram(0);
	glutSwapBuffers();
}
int frameCount =0;
int currentTime=0;
int previousTime =0;
float fps =0;

void calculateFPS()
{
	previousTime = currentTime;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	fps = 1000/(float)(currentTime-previousTime);
}
void glutIdle()
{
	calculateFPS();
	//system("CLS");
	//printf("ZStrength: %.2f\nFPS: %.f\n",zstrength,fps);
	glutPostRedisplay();
}

void glutDisplay()
{

	display();
}

void glutKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{

	case 'a':
		viewAngle-=0.05;
		break;
	case 'd':
		viewAngle+=0.05;
		break;
	case 'w':
		cameraY+=DISTANCE;
		break;
	case 's':
		cameraY-=DISTANCE;
		break;
	case 'z':
		cameraZ+=DISTANCE;
		//CVALUE += 500;
		break;
	case 'x':
		//CVALUE -= 500;
		cameraZ-=DISTANCE;
		break;
	case 'f':
		offset_++;
		break;
	case 'g':
		if(offset_ > 1)
			offset_--;
		break;
	case 'c':
		zstrength/=2;
		break;
	case 'v':
		zstrength*=2;
		break;
	case 'l':
		isDrawFace = !isDrawFace;
		break;
	case 'k':
		isDrawColor = !isDrawColor;
		break;
	}
	int k = x+y;
	k = k+1;
	printf("%f\n",zstrength);
}

void glutMousePassive(int x, int y)
{
	mouse[X] = x;
	mouse[Y] = y;
}

void initOpenGL(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow ("Spogr Spohr");

	glutSetCursor(GLUT_CURSOR_NONE);

	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);
	glutKeyboardFunc(glutKeyboard);
	glutPassiveMotionFunc(glutMousePassive);
	glDisable(GL_DEPTH_TEST);
	

    glMatrixMode(GL_PROJECTION);												
 
	// set the viewport
    glViewport(0, 0, 640,480);									
 
	// set matrix mode
    glMatrixMode(GL_PROJECTION);												
 
	// reset projection matrix
    glLoadIdentity();															
    //GLfloat aspect = (GLfloat) 640/480;
 
	// set up a perspective projection matrix
	
	float vfov = 45;
	float aspect = 1;
	gluPerspective(vfov,aspect,0.5,20000);		
 
	// specify which matrix is the current matrix
	glMatrixMode(GL_MODELVIEW);													
    glShadeModel( GL_SMOOTH );
 
	// specify the clear value for the depth buffer
	glClearDepth( 1.0f );														
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

	glClearColor(0.0, 0.0, 0.0, 1.0);

	
	setupGlew();


	useShader("fragShader.txt","vertShader.txt");


	eyePosLocation = glGetUniformLocation(program, "worldEyePos");
	lightPosLocation = glGetUniformLocation(program, "worldLightPos");
	zstrengthLocation = glGetUniformLocation(program, "zstrength");

	glUseProgram(program);

	glutMainLoop();

}



int main(int argc, char** argv)
{

	depthw = dScreen.getWidth();
	depthh = dScreen.getHeight();
	
	screen = new openni::RGB888Pixel[depthw * depthh];
	fscreen = new int[depthw * depthh];


	for(unsigned int i=0 ; i<(unsigned int)depthw * depthh ; i++)
	{
		screen[i].r = 20;
		screen[i].g = 0;
		screen[i].b = 0;
		fscreen[i] = 0;
	}
	initOpenGL(argc,argv);

	printf("Camera left(a) right(d) \nup(w) down(s) \nforward(s) backward(z) \nSampling (f)less fine (g)more fine \nMax Distance increase(c) decrease(v)\nToggle Bleeding Removal(l)\nToggle Draw With Color(k)"); 
	
	glutMainLoop();

	return 0;
}

