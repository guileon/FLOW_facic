
import SimpleOpenNI.*;


SimpleOpenNI  context;
int[][] lastScreen;
void setup()
{
  size(640, 480, P2D);
  lastScreen  = new int[640][480];
   for(int i=0 ; i< 640 ; i++)
   for(int j=0 ; j< 480 ; j++)
   {
     lastScreen[i][j] = 0;
   }
  colorMode(HSB);
  context = new SimpleOpenNI(this);
  if (context.isInit() == false)
  {
    println("Can't init SimpleOpenNI, maybe the camera is not connected!"); 
    exit();
    return;
  }

  // mirror is by default enabled
  //context.setMirror(true);

  // enable depthMap generation 
  context.enableDepth();

  // enable ir generation
  context.enableRGB();
}
  int max = -1000000;
  int min = 1000000;;
  int time = 0;
void draw()
{
  time++;
  // update the cam
  context.update();

  
  background(0);
  
  // draw depthImageMap
  //image(context.depthImage(), 0, 0);
  for(int i=0 ; i<640 ; i+=10)
  {
    for(int j=0 ; j<480 ; j+=10)
    {

      int p = -context.depthImage().get(i,j);

      //println(p);
      if(p < 1000000)
      {
        
        int p2 = lastScreen[i][j];
        strokeWeight(10);
        stroke((p2)/5772,255,255,50);
        point(i,j);
        
        if (time > 1)
        {
          time = 0;
          lastScreen[i][j] = p;
        }
      }

    }
  }
}

