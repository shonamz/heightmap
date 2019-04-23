/*
  CSCI 420 Computer Graphics, USC
  Assignment 1: Height Fields
  C++ starter code

  Student username: <namazian>
*/

#include <iostream>
#include <cstring>
#include "openGLHeader.h"
#include "glutHeader.h"

#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"
#include "chrono"

 #include <vector> 

#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#ifdef WIN32
  char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
  char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif

using namespace std;

int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, 0.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";

ImageIO * heightmapImage;

//set up modelView and projection matrices
OpenGLMatrix * matrix = new OpenGLMatrix();
float m[16];
float p[16];
const float fov=45;

// data for postions and colors
/*
std::vector<GLfloat> triangleVertices;
std::vector<GLfloat> triangleColors;


std::vector<GLfloat> wireframeVertices;
std::vector<GLfloat> wireframeColors;

std::vector<GLfloat> pointVertices;
std::vector<GLfloat> pointColors;

// VBO and VAO

GLuint terrianVBO;
GLuint terrianVAO;
 
GLuint WireframeVAO;
GLuint WireframeVBO;

GLuint PointframeVAO;
GLuint PointframeVBO;

GLuint imageWidth = 0;
GLuint imageHeight = 0;*/
 
 ///pipeline

BasicPipelineProgram  * g_pipeline = new BasicPipelineProgram();

GLuint program = 0;

 // Display handling
enum DisplayTypeEnum { TRIANGLE, WIREFRAME, POINTS };
DisplayTypeEnum g_displayType = TRIANGLE;

//translation varibale
const float TRANSLATION_MODIFIER = 40;

//animation variables
 
int MAXIMUM_SCREENSHOTS = 301;
int frameNum;
int g_screenshotCounter = 0;
float tacc=0;


 
//screen shot 

bool takeScreenshots = false;


    // initlize pipeline

void initPipeLine(){

    g_pipeline->Init("../openGLHelper-starterCode");
    g_pipeline->Bind();
    program = g_pipeline->GetProgramHandle();
       

 }
//initlize buffers for 3 mode
void initBuffer(){
// triangle VA and VB

glGenVertexArrays(1, &terrianVAO);
glBindVertexArray(terrianVAO);

glGenBuffers(1,&terrianVBO);
glBindBuffer(GL_ARRAY_BUFFER,terrianVBO);
glBufferData(GL_ARRAY_BUFFER, (triangleVertices.size()+triangleColors.size())*sizeof(float),NULL,GL_STATIC_DRAW);

//UPLOAD POSITION DATA

glBufferSubData(GL_ARRAY_BUFFER,0,triangleVertices.size()*sizeof(float),triangleVertices.data());

//UPLOAD COLOR DATA

glBufferSubData(GL_ARRAY_BUFFER ,triangleVertices.size()*sizeof(float), triangleColors.size() *sizeof(float),triangleColors.data());

// get location index of the “position” shader variable
GLuint loc = glGetAttribLocation(program, "position");
glEnableVertexAttribArray(loc); 
// enable the “position” attribute 
const void *offset = (const void*) 0;
  GLsizei stride = 0; 
  GLboolean normalized = GL_FALSE;
// set the layout of the “position” attribute data
glVertexAttribPointer(loc, 3, GL_FLOAT, normalized, stride, offset);

 // get the location index of the “color” shader variable
loc = glGetAttribLocation(program, "color");

glEnableVertexAttribArray(loc);
// enable the “color” attribute
offset = (const void*) (triangleVertices.size()*sizeof(float));
 
// set the layout of the “color” attribute data 
glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);

glBindVertexArray(0); // unbind the VAO


//wiireframe VA and VB

glGenVertexArrays(1,&WireframeVAO);
glBindVertexArray(WireframeVAO);

glGenBuffers(1,&WireframeVBO);
glBindBuffer(GL_ARRAY_BUFFER, WireframeVBO);
glBufferData(GL_ARRAY_BUFFER, (wireframeVertices.size()+wireframeColors.size())*sizeof(float),NULL,GL_STATIC_DRAW);
//UPLOAD POSITION DATA

glBufferSubData(GL_ARRAY_BUFFER,0,wireframeVertices.size()*sizeof(float),wireframeVertices.data());
//UPLOAD POSITION COLOR

glBufferSubData(GL_ARRAY_BUFFER,wireframeVertices.size()*sizeof(float),wireframeColors.size()*sizeof(float),wireframeColors.data());
// get location index of the “position” shader variable

GLuint loc1= glGetAttribLocation(program,"position");
glEnableVertexAttribArray(loc1);
const void *offset1 =(const void*) 0;
GLsizei stride1 =0;
GLboolean normalized1= GL_FALSE;
glVertexAttribPointer(loc1,3,GL_FLOAT,normalized1,stride1,offset1);
 // get the location index of the “color” shader variable

loc1= glGetAttribLocation(program,"color");
glEnableVertexAttribArray(loc1);
  // enable the “color” attribute

offset1=(const void *) (wireframeVertices.size()*sizeof(float));
// set the layout of the “color” attribute data 

glVertexAttribPointer(loc1,4,GL_FLOAT,normalized1,stride,offset1);  

glBindVertexArray(0); // unbind the VAO

 
//VAO and VBO of point 

glGenVertexArrays(1,&PointframeVAO);
glBindVertexArray(PointframeVAO);
glGenBuffers(1,&PointframeVBO);
glBindBuffer(GL_ARRAY_BUFFER, PointframeVBO);
glBufferData(GL_ARRAY_BUFFER, (pointVertices.size()+pointColors.size())*sizeof(float),NULL,GL_STATIC_DRAW);
//UPLOAD POSITION DATA

glBufferSubData(GL_ARRAY_BUFFER,0,pointVertices.size()*sizeof(float),pointVertices.data());
//UPLOAD POSITION COLOR

glBufferSubData(GL_ARRAY_BUFFER,pointVertices.size()*sizeof(float),pointColors.size()*sizeof(float),pointColors.data());
// get location index of the “position” shader variable

GLuint  loc2= glGetAttribLocation(program,"position");
glEnableVertexAttribArray(loc2);
const void *offset2 =(const void*) 0;
GLsizei stride2 =0;
GLboolean normalized2= GL_FALSE;
glVertexAttribPointer(loc2,3,GL_FLOAT,normalized2,stride2,offset2);
 // get the location index of the “color” shader variable

loc2= glGetAttribLocation(program,"color");
glEnableVertexAttribArray(loc2);
  // enable the “color” attribute

offset2=(const void *) (pointVertices.size()*sizeof(float));
// set the layout of the “color” attribute data 

glVertexAttribPointer(loc,4,GL_FLOAT,normalized2,stride2,offset2);
glBindVertexArray(0); // unbind the VAO

 
}
 
// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
  cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;

  delete [] screenshotData;
}

void displayFunc()
{
 
glClearColor(0, 0, 0, 1);
glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
GLint h_modelViewMatrix =glGetUniformLocation(program, "modelViewMatrix");
  // compute modelview matrix
matrix->SetMatrixMode(OpenGLMatrix::ModelView);
matrix->LoadIdentity();
//set the camera
matrix->LookAt(-150, 100, -150, 
               0,0,0,
               0,1,0);  
// start translation
matrix->Translate(landTranslate[0]*TRANSLATION_MODIFIER,landTranslate[1]*TRANSLATION_MODIFIER,landTranslate[2]*TRANSLATION_MODIFIER);
//strat rotation
matrix->Rotate(landRotate[0], 1,0,0);
matrix->Rotate(landRotate[1], 0,1,0);
matrix->Rotate(landRotate[2], 0,0,1);
//start scaling
matrix->Scale(landScale[0],landScale[1],landScale[2]);


matrix->GetMatrix(m);// fill "m" with the matrix entries 
// upload m to the GPU
g_pipeline->Bind(); // must do (once) before glUniformMatrix4fv
GLboolean isRowMajor=GL_FALSE;
glUniformMatrix4fv(h_modelViewMatrix, 1, isRowMajor, m);
matrix->SetMatrixMode(OpenGLMatrix::Projection);
 // get a handle to the projectionMatrix shader variable
GLint h_projectionMatrix = glGetUniformLocation(program, "projectionMatrix"); 
matrix->GetMatrix(p);
// upload p to the GPU
glUniformMatrix4fv(h_projectionMatrix, 1, isRowMajor, p);
// Use the VAO
g_pipeline->Bind(); // bind the pipeline program
GLint first = 0;
GLsizei count; 
// switch between different mode
 
  switch(g_displayType)
  {
    case(TRIANGLE):
    glBindVertexArray(terrianVAO); // bind the VAO
    count = triangleVertices.size();
    glDrawArrays(GL_TRIANGLES, first, count);
    glBindVertexArray(0); // unbind the VAO
    break;

    case(WIREFRAME):
    glBindVertexArray(WireframeVAO);
    count = wireframeVertices.size();
    glDrawArrays(GL_LINES,first,count);
    glBindVertexArray(0); // unbind the VAO
    break;

    case(POINTS):
    glBindVertexArray(PointframeVAO);
    count = pointVertices.size();
    glDrawArrays(GL_POINTS,first,count);
    glBindVertexArray(0); // unbind the VAO
    break;
  }
  //switch buffer


  glutSwapBuffers();
}

void idleFunc()
{
// animation. save files
 if (g_screenshotCounter <  MAXIMUM_SCREENSHOTS && takeScreenshots) {

      auto start =chrono::steady_clock::now();

    std::string filename = std::to_string(frameNum) + ".jpg";
            if (frameNum < 100) {
                filename = "0" + filename;
            }
            if (frameNum < 10) {
                filename = "0" + filename;
            }
            filename = "Recording/" + filename;
            saveScreenshot(filename.c_str());
            ++frameNum; 
            g_screenshotCounter++;

            auto end =chrono::steady_clock::now();
            // find elapse time

            float timeElaspe = chrono::duration_cast<chrono::microseconds>(end -start).count();

            tacc=  timeElaspe /1000000 + tacc;

            cout << "tacc........" << tacc << endl;
            // check the frame rate dump out if over 
            if (tacc>0.0666){

              //dump out an image
              MAXIMUM_SCREENSHOTS =MAXIMUM_SCREENSHOTS+1;
              frameNum--;

               tacc=0;


            }

   
}
    // make the screen update

  glutPostRedisplay();
}
//reshape function to set up perspective function
void reshapeFunc(int w, int h)
{
  glViewport(0, 0, w, h);
  //set up  projection matrix
  GLfloat aspect =(GLfloat)1280/(GLfloat)720;
  matrix->SetMatrixMode(OpenGLMatrix::Projection);
  matrix->LoadIdentity();
  //set camera to perspective
  matrix->Perspective(fov, aspect, 0.01, 1000.0);
  matrix->GetMatrix (p);
// Send matrix to the pipeline
 g_pipeline->SetProjectionMatrix (p);
//set back to model view
 matrix->SetMatrixMode(OpenGLMatrix::ModelView);
}

void mouseMotionDragFunc(int x, int y)
{
  // mouse has moved and one of the mouse buttons is pressed (dragging)
 

   
  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState){
    // translate the landscape
    case TRANSLATE:
      if (leftMouseButton)
      {
        // control x,y translation via the left mouse button
        landTranslate[0] += mousePosDelta[0] * 0.01f;
        landTranslate[1] -= mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
        landTranslate[2] += mousePosDelta[1] * 0.01f;
      }

      break;
    // rotate the landscape
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        landRotate[0] += mousePosDelta[1];
        landRotate[1] += mousePosDelta[0];
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        landRotate[2] += mousePosDelta[1];
      }

      break;
    // scale the landscape 
    case SCALE:
      if (leftMouseButton) {
        // control x,y scaling via the left mouse button
        landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
        landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z scaling via the middle mouse button
        landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      
      break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;


}

void mouseMotionFunc(int x, int y)
{
  // mouse has moved
  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // a mouse button has has been pressed or depressed

  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      leftMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_MIDDLE_BUTTON:
      middleMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_RIGHT_BUTTON:
      rightMouseButton = (state == GLUT_DOWN);
    break;
  }

  // keep track of whether CTRL and SHIFT keys are pressed
  switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      controlState = TRANSLATE;
    break;

    case GLUT_ACTIVE_SHIFT:
      controlState = SCALE;
    break;
    // if CTRL and SHIFT are not pressed, we are in rotate mode
    default:
      controlState = ROTATE;
    break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

    case ' ':
      cout << "You pressed the spacebar." << endl;
    break;

    case 'x':
      // take a screenshot
      saveScreenshot("screenshot.jpg");
    break;
    case '1': //display tringle mesh
     g_displayType = TRIANGLE;
      break;

     case '2': ///display wireframe mesh
      g_displayType = WIREFRAME;
      break;

      case '3': //dispaly points mesh
      g_displayType = POINTS;
      break;

      case 'q':// start animation
      takeScreenshots = true;
      std::cout << "animation!." << std::endl;
      break;
//Transformation
      case 't':
            controlState = TRANSLATE;

      break;

      }
}

 /*
 void generateHeight (ImageIO* image) {

  imageWidth = image->getWidth();
  imageHeight = image->getHeight();
  GLuint numVertices = imageWidth * imageHeight;
  float terrainScale=0.015 ;
  terrainScale *= image->getHeight() / 50;
int i1 = -(imageHeight / 2);
int j1 = -(imageWidth / 2);

int k= (imageHeight / 2 )- 1;

int l= (imageWidth / 2 )- 1;
///making basic mesh
  for (int i=i1; i < l; i++) {
    for ( int j=j1; j < l; j++) { 
        
      // Bottom left corner
      GLfloat y = terrainScale * image->getPixel(i + imageHeight / 2, j + imageWidth/ 2, 0);
      GLfloat bl[3] = { i, y, -j };
      // top left corner
      y = terrainScale * image->getPixel(i + imageHeight / 2, j + imageWidth / 2 + 1, 0);
      GLfloat tl[3] = { i, y, -(j + 1) };
      // Top left corner
      y = terrainScale * image->getPixel(i + imageHeight / 2 + 1, j + imageWidth / 2 + 1, 0);
      GLfloat tr[3] = { i + 1, y, -(j + 1) };
      // Bottem right corner
      y = terrainScale * image->getPixel(i + imageHeight / 2 + 1, j + imageWidth / 2, 0);
      GLfloat br[3] = { i + 1, y, -j };
      // Push our coordinates into the vertex buffer as two triangles (clockwise)
      triangleVertices.insert (triangleVertices.end(), tl, tl + 3);
      triangleVertices.insert (triangleVertices.end(), tr, tr + 3);
      triangleVertices.insert (triangleVertices.end(), bl, bl + 3);
      triangleVertices.insert (triangleVertices.end(), bl, bl + 3);
      triangleVertices.insert (triangleVertices.end(), tr, tr + 3);
      triangleVertices.insert (triangleVertices.end(), br, br + 3);
 // Calculate colors
      GLfloat color = (float)image->getPixel(i + imageHeight / 2, j + imageWidth / 2, 0) / (float)255.0f;
      GLfloat colorBL[4] = { color, color, color, 1.0f };

      color = (float)image->getPixel(i + imageHeight / 2, j + imageWidth / 2 + 1, 0) / (float)255.0f;
      GLfloat colorTL[4] = { color, color, color, 1.0f };

      color = (float)image->getPixel(i + imageHeight / 2 + 1, j + imageWidth/ 2 + 1, 0) / (float)255.0f;
      GLfloat colorTR[4] = { color, color, color, 1.0f };

      color = (float)image->getPixel(i + imageHeight/ 2 + 1, j + imageWidth / 2, 0) / (float)255.0f;
      GLfloat colorBR[4] = { color, color, color, 1.0f };
      
     // Push our color data to the buffer For triangle
      triangleColors.insert (triangleColors.end(), colorTL, colorTL + 4);
      triangleColors.insert (triangleColors.end(), colorTR, colorTR + 4);
      triangleColors.insert (triangleColors.end(), colorBL, colorBL + 4);
      triangleColors.insert (triangleColors.end(), colorBL, colorBL + 4);
      triangleColors.insert (triangleColors.end(), colorTR, colorTR + 4);
      triangleColors.insert (triangleColors.end(), colorBR, colorBR + 4);
     // Push our data postion to the buffer For wireframe
      wireframeVertices.insert (wireframeVertices.end(), tl, tl + 3);
      wireframeVertices.insert (wireframeVertices.end(), tr, tr + 3);
      wireframeVertices.insert (wireframeVertices.end(), tr, tr + 3);
      wireframeVertices.insert (wireframeVertices.end(), bl, bl + 3);
      wireframeVertices.insert (wireframeVertices.end(), bl, bl + 3);
      wireframeVertices.insert (wireframeVertices.end(), tl, tl + 3);
      wireframeVertices.insert (wireframeVertices.end(), tr, tr + 3);
      wireframeVertices.insert (wireframeVertices.end(), br, br + 3);
      wireframeVertices.insert (wireframeVertices.end(), br, br + 3);
      wireframeVertices.insert (wireframeVertices.end(), bl, bl + 3);
     // Push our color data to the buffer For wireframe

      GLfloat colorF[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);
      wireframeColors.insert (wireframeColors.end(), colorF, colorF + 4);

     // Push our position data to the buffer For points
       pointVertices.insert (pointVertices.end(), tl, tl + 3);
       pointVertices.insert (pointVertices.end(), tr, tr + 3);
       pointVertices.insert (pointVertices.end(), bl, bl + 3);
       pointVertices.insert (pointVertices.end(), br, br + 3);
  
     // Push our color data to the buffer For points

      GLfloat colorW[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
      pointColors.insert (pointColors.end(), colorW, colorW + 4);
      pointColors.insert (pointColors.end(), colorW, colorW + 4);
      pointColors.insert (pointColors.end(), colorW, colorW + 4);
      pointColors.insert (pointColors.end(), colorW, colorW + 4);
  }

}

  
}
*/
void initScene(int argc, char *argv[])
{
  // load the image from a jpeg disk file to main memory
  heightmapImage = new ImageIO();
  if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
  {
    cout << "Error reading image " << argv[1] << "." << endl;
    exit(EXIT_FAILURE);
  }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
     //init Pipeline Program

    initPipeLine();
     //genertae height field
    generateHeight (heightmapImage);
    //initilize buffer
    initBuffer();

    //enable depth buffer
    glEnable(GL_DEPTH_TEST);

}


int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cout << "The arguments are incorrect." << endl;
    cout << "usage: ./hw1 <heightmap file>" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);
  glutCreateWindow(windowTitle);

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;


  // tells glut to use a particular display function to redraw


  glutDisplayFunc(displayFunc);
  // perform animation inside idleFunc
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);


  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
      cout << "error: " << glewGetErrorString(result) << endl;
      exit(EXIT_FAILURE);
    }
  #endif



  // do initialization
  initScene(argc, argv);

  // sink forever into the glut loop
  glutMainLoop();
}
