/*
  CSCI 420 Computer Graphics, Computer Science, USC
  Assignment 2: Simulating a Roller Coaster.
  C/C++ starter code

  Student username: heyumeng
*/

#include "openGLHeader.h"
#include "glutHeader.h"
#include "openGLMatrix.h"
#include "imageIO.h"
#include "pipelineProgram.h"
#include "vbo.h"
#include "vao.h"
#include "ebo.h"
#include "mathOperation.h"

#include <iostream>
#include <cstring>
#include <memory>
#include <chrono>

// HW2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32) || defined(_WIN32)
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#if defined(WIN32) || defined(_WIN32)
  char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
  char shaderBasePath[1024] = "../openGLHelper";
#endif

using namespace std;

int mousePos[2]; // x,y screen coordinates of the current mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// Transformations of the terrain.
float terrainRotate[3] = { 0.0f, 0.0f, 0.0f }; 
// terrainRotate[0] gives the rotation around x-axis (in degrees)
// terrainRotate[1] gives the rotation around y-axis (in degrees)
// terrainRotate[2] gives the rotation around z-axis (in degrees)
float terrainTranslate[3] = { 0.0f, 0.0f, 0.0f };
float terrainScale[3] = { 1.0f, 1.0f, 1.0f };

// Width and height of the OpenGL window, in pixels.
int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 Homework 1";

// Number of vertices in the single triangle (starter code).
int numVertices = 0;
int numIndices = 0;

// CSCI 420 helper classes.
OpenGLMatrix matrix;
PipelineProgram pipelineProgram;

// pipeline
GLuint programHandle;
VAO VAOVertice;
VBO VBOPosition, VBOColor;
EBO railEBO;

// for image
int width, height, bytesPerPixel;

// for vertices
std::vector<float> pointPosition, pointColor;
std::vector<unsigned int> railIndices;

// for tie
std::vector<float> tiePosition, tieNormal;
std::vector<unsigned int> tieIndices;
VAO VAOVerticeTie;
VBO VBOPositionTie, VBONormalTie;
EBO EBOIndexTie;
int numTieVertices = 0;
int numTieIndices = 0;

// take 900 screenshots
bool isCapturing = false;           // capturing status
int screenshotFrame = 0;            // count screenshots
const int totalFrames = 900;        // take 900 screenshots in total
const int fps = 15;                 // 15 screenshots per second
std::chrono::steady_clock::time_point lastFrameTime; // timer

// replacement for CTRL
bool isTranslatingKeyPressed = false;

// camera movement
float uGlobal = 0.0f;
float speed = 0.001f;
int currentSegment = 0;

// ---------- Ground texture ----------
std::vector<float> groundPositions = {
  // Positions         // Texture Coords
  -100.0f, -10.0f, -100.0f,  0.0f, 0.0f,  // Bottom-left
   100.0f, -10.0f, -100.0f,  1.0f, 0.0f,  // Bottom-right
   100.0f, -10.0f,  100.0f,  1.0f, 1.0f,  // Top-right
  -100.0f, -10.0f,  100.0f,  0.0f, 1.0f   // Top-left
};
std::vector<unsigned int> groundIndices = {
    0, 1, 2,
    2, 3, 0
};
int numGroundVertices = 4;
int numGroundIndices = 6;
GLuint groundTextureHandle;
PipelineProgram texturePipelineProgram;
VAO groundVAO;
VBO groundVBOPosition, groundVBOTexCoord;
EBO groundEBO;

float prevNormal[3] = { 0.0f, 1.0f, 0.0f };

// skybox
GLuint skyboxTextureHandles[6];
VAO skyboxVAO;
VBO skyboxVBO;
EBO skyboxEBO;

// Skybox vertex data
std::vector<float> skyboxVertices = {
  // Positions         // Texture Coords
  // Back face (Negative Z)
  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, // 0 - Top-left
  -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, // 1 - Bottom-left
   1.0f, -1.0f, -1.0f, 1.0f, 0.0f, // 2 - Bottom-right
   1.0f,  1.0f, -1.0f, 1.0f, 1.0f, // 3 - Top-right

  // Front face (Positive Z)
  -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // 4 - Top-left
  -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // 5 - Bottom-left
   1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // 6 - Bottom-right
   1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // 7 - Top-right

  // Left face (Negative X)
  -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, // 8 - Top-left
  -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // 9 - Bottom-left
  -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, // 10 - Bottom-right
  -1.0f,  1.0f, -1.0f, 1.0f, 1.0f, // 11 - Top-right

  // Right face (Positive X)
  1.0f,  1.0f, -1.0f, 0.0f, 1.0f, // 12 - Top-left
  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, // 13 - Bottom-left
  1.0f, -1.0f,  1.0f, 1.0f, 0.0f, // 14 - Bottom-right
  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, // 15 - Top-right

  // Bottom face (Negative Y)
  -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, // 16 - Top-left
   1.0f, -1.0f, -1.0f, 1.0f, 1.0f, // 17 - Top-right
   1.0f, -1.0f,  1.0f, 1.0f, 0.0f, // 18 - Bottom-right
  -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // 19 - Bottom-left

  // Top face (Positive Y)
  -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, // 20 - Bottom-left
   1.0f,  1.0f, -1.0f, 1.0f, 0.0f, // 21 - Bottom-right
   1.0f,  1.0f,  1.0f, 1.0f, 1.0f, // 22 - Top-right
  -1.0f,  1.0f,  1.0f, 0.0f, 1.0f  // 23 - Top-left
};

std::vector<unsigned int> skyboxIndices = {
  // Back face
  0, 1, 2,
  2, 3, 0,

  // Front face
  4, 5, 6,
  6, 7, 4,

  // Left face
  8, 9, 10,
  10,11, 8,

  // Right face
  12,13,14,
  14,15,12,

  // Bottom face
  16,17,18,
  18,19,16,

  // Top face
  20,21,22,
  22,23,20
};

// bird view
bool isBirdView = false;
float birdViewDistance = 100.0f;

// pillar
std::vector<float> pillarPosition;
std::vector<float> pillarNormal;
std::vector<unsigned int> pillarIndices;
int numPillarVertices = 0;
int numPillarIndices = 0;
VAO pillarVAO;
VBO pillarVBOPosition, pillarVBONormal;
EBO pillarEBO;



std::string spFilename;


// declare functions
struct Point;
struct Spline;
void loadSpline(char * argv);
int initTexture(const char * imageFilename, GLuint textureHandle);
void saveScreenshot(const char * filename);
void computeCameraFrame(float u, int segmentIndex);
void idleFunc();
void reshapeFunc(int w, int h);
std::vector<unsigned char> rescaleColorImage(int newWidth, int newHeight);
void mouseMotionDragFunc(int x, int y);
void mouseMotionFunc(int x, int y);
void mouseButtonFunc(int button, int state, int x, int y);
void keyboardFunc(unsigned char key, int x, int y);
void keyboardUpFunc(unsigned char key, int x, int y);
void displayFunc();
void updateCamera(const float* position, const float* tangent, const float* normal);
void readSpline();
void initVAOVBO();
void initScene(int argc, char *argv[]);
void setBirdViewCamera();

// ---------- HW2 helper functions ----------
// Represents one spline control point.
struct Point 
{
  float x, y, z;
};

// Contains the control points of the spline.
struct Spline 
{
  int numControlPoints;
  Point * points;
} spline;

void loadSpline(char * argv) 
{
  FILE * fileSpline = fopen(argv, "r");
  if (fileSpline == NULL) 
  {
    printf ("Cannot open file %s.\n", argv);
    exit(1);
  }

  // Read the number of spline control points.
  fscanf(fileSpline, "%d\n", &spline.numControlPoints);
  printf("Detected %d control points.\n", spline.numControlPoints);

  // Allocate memory.
  spline.points = (Point *) malloc(spline.numControlPoints * sizeof(Point));
  // Load the control points.
  for(int i=0; i<spline.numControlPoints; i++)
  {
    if (fscanf(fileSpline, "%f %f %f", 
        &spline.points[i].x, 
	      &spline.points[i].y, 
	      &spline.points[i].z) != 3)
    {
      printf("Error: incorrect number of control points in file %s.\n", argv);
      exit(1);
    }
  }
}

int initTexture(const char * imageFilename, GLuint textureHandle)
{
  // Read the texture image.
  ImageIO img;
  ImageIO::fileFormatType imgFormat;
  ImageIO::errorType err = img.load(imageFilename, &imgFormat);

  if (err != ImageIO::OK) 
  {
    printf("Loading texture from %s failed.\n", imageFilename);
    return -1;
  }

  // Check that the number of bytes is a multiple of 4.
  if (img.getWidth() * img.getBytesPerPixel() % 4) 
  {
    printf("Error (%s): The width*numChannels in the loaded image must be a multiple of 4.\n", imageFilename);
    return -1;
  }

  // Allocate space for an array of pixels.
  int width = img.getWidth();
  int height = img.getHeight();
  unsigned char * pixelsRGBA = new unsigned char[4 * width * height]; // we will use 4 bytes per pixel, i.e., RGBA

  // Fill the pixelsRGBA array with the image pixels.
  memset(pixelsRGBA, 0, 4 * width * height); // set all bytes to 0
  for (int h = 0; h < height; h++)
    for (int w = 0; w < width; w++) 
    {
      // assign some default byte values (for the case where img.getBytesPerPixel() < 4)
      pixelsRGBA[4 * (h * width + w) + 0] = 0; // red
      pixelsRGBA[4 * (h * width + w) + 1] = 0; // green
      pixelsRGBA[4 * (h * width + w) + 2] = 0; // blue
      pixelsRGBA[4 * (h * width + w) + 3] = 255; // alpha channel; fully opaque

      // set the RGBA channels, based on the loaded image
      int numChannels = img.getBytesPerPixel();
      for (int c = 0; c < numChannels; c++) // only set as many channels as are available in the loaded image; the rest get the default value
        pixelsRGBA[4 * (h * width + w) + c] = img.getPixel(w, h, c);
    }

  // Bind the texture.
  glBindTexture(GL_TEXTURE_2D, textureHandle);

  // Initialize the texture.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsRGBA);

  // Generate the mipmaps for this texture.
  glGenerateMipmap(GL_TEXTURE_2D);

  // Set the texture parameters.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Query support for anisotropic texture filtering.
  GLfloat fLargest;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
  printf("Max available anisotropic samples: %f\n", fLargest);
  // Set anisotropic texture filtering.
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.5f * fLargest);

  // Query for any errors.
  GLenum errCode = glGetError();
  if (errCode != 0) 
  {
    printf("Texture initialization error. Error code: %d.\n", errCode);
    return -1;
  }
  
  // De-allocate the pixel array -- it is no longer needed.
  delete [] pixelsRGBA;

  cout << "Successfully read " << imageFilename << endl;
  cout << "Texture size width is " << width << endl;

  return 0;
}

// ---------- HW1 functions ----------
// Write a screenshot to the specified filename.
void saveScreenshot(const char * filename)
{
  std::unique_ptr<unsigned char[]> screenshotData = std::make_unique<unsigned char[]>(windowWidth * windowHeight * 3);
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData.get());

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData.get());

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;
}

void idleFunc()
{
  // capture 900 screenshot
  // and name each screenshot from 000.jpg to 899.jpg
  // save all screenshot under "screenshot" folder
  if (isCapturing)
  {
    // Calculate the time elapsed since the last frame
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - lastFrameTime;

    // Calculate the frame duration (in seconds)
    float frameDuration = 1.0f / fps;

    if (elapsedTime.count() >= frameDuration)
    {
      // Time to capture the next frame

      // Construct the filename
      char filename[256];
      sprintf(filename, "screenshot/%03d.jpg", screenshotFrame);

      // Take and save the screenshot
      saveScreenshot(filename);

      std::cout << "Saved screenshot: " << filename << std::endl;

      // Update the frame counter and last frame time
      screenshotFrame++;
      lastFrameTime = currentTime;

      // Check if we've reached the total number of frames
      if (screenshotFrame >= totalFrames)
      {
        isCapturing = false;
        std::cout << "Finished capturing screenshots." << std::endl;
      }
    }
  }

  // bird view
  if (isBirdView)
  {
    setBirdViewCamera();
  }
  else
  {
    // update u
    uGlobal += speed;

    if (uGlobal > 1.0f)
    {
        uGlobal -= 1.0f;
        currentSegment++;
        if (currentSegment > spline.numControlPoints - 4)
        {
            currentSegment = 0; // Loop back to the start
        }
    }

    // compute position, tangent, normal, and update camera
    computeCameraFrame(uGlobal, currentSegment);
  }

  // Notify GLUT that it should call displayFunc.
  glutPostRedisplay();
}

void reshapeFunc(int w, int h)
{
  glViewport(0, 0, w, h);

  // When the window has been resized, we need to re-set our projection matrix.
  matrix.SetMatrixMode(OpenGLMatrix::Projection);
  matrix.LoadIdentity();
  // You need to be careful about setting the zNear and zFar. 
  // Anything closer than zNear, or further than zFar, will be culled.
  const float zNear = 0.1f;
  const float zFar = 10000.0f;
  const float humanFieldOfView = 60.0f;
  matrix.Perspective(humanFieldOfView, 1.0f * w / h, zNear, zFar);
}

void mouseMotionDragFunc(int x, int y)
{
  // Mouse has moved, and one of the mouse buttons is pressed (dragging).

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState)
  {
    // translate the terrain
    case TRANSLATE:
      if (leftMouseButton)
      {
        // control x,y translation via the left mouse button
        terrainTranslate[0] += mousePosDelta[0] * 0.10f;
        terrainTranslate[1] -= mousePosDelta[1] * 0.10f;
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
        terrainTranslate[2] += mousePosDelta[1] * 0.10f;
      }
      break;

    // rotate the terrain
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        terrainRotate[0] += mousePosDelta[1]; // rotate around x-axis
        terrainRotate[1] += mousePosDelta[0]; // rotate around y-axis
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        terrainRotate[2] += mousePosDelta[1]; // rotate around z-axis
      }
      break;

    // scale the terrain
    case SCALE:
      if (leftMouseButton)
      {
        // control x,y scaling via the left mouse button
        terrainScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
        terrainScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z scaling via the middle mouse button
        terrainScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
  // Mouse has moved.
  // Store the new mouse position.
  mousePos[0] = x;
  mousePos[1] = y;
}

// detect mouse input and CTRL, SHIFT
void mouseButtonFunc(int button, int state, int x, int y)
{
  // A mouse button has has been pressed or depressed.

  // Keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables.
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

  // Keep track of whether CTRL and SHIFT keys are pressed.
  // switch (glutGetModifiers())
  // {
  //   case GLUT_ACTIVE_CTRL:
  //     controlState = TRANSLATE;
  //   break;

  //   case GLUT_ACTIVE_SHIFT:
  //     controlState = SCALE;
  //   break;

  //   // If CTRL and SHIFT are not pressed, we are in rotate mode.
  //   default:
  //     controlState = ROTATE;
  //   break;
  // }

  int modifiers = glutGetModifiers();

  if (isTranslatingKeyPressed)
  {
    controlState = TRANSLATE;
  }
  else if (modifiers & GLUT_ACTIVE_SHIFT)
  {
    controlState = SCALE;
  }
  else
  {
    controlState = ROTATE;
  }

  // Store the new mouse position.
  mousePos[0] = x;
  mousePos[1] = y;
}

// detect key input
void keyboardFunc(unsigned char key, int x, int y)
{ 
  GLint loc = glGetUniformLocation(pipelineProgram.GetProgramHandle(), "mode");
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

    case ' ':
      cout << "You pressed the spacebar." << endl;
    break;

    case 'x':
      // Take a screenshot.
      saveScreenshot("screenshot.jpg");
    break;

    case 'c':
      // capture 300 screenshot
      if (!isCapturing)
      {
        isCapturing = true;
        screenshotFrame = 0;
        lastFrameTime = std::chrono::steady_clock::now();
        std::cout << "Started capturing screenshots." << std::endl;
      }
      else
      {
        isCapturing = false;
        std::cout << "Stopped capturing screenshots." << std::endl;
      }
    break;

    case 't':
      // replacement for CTRL
      isTranslatingKeyPressed = true; // 't' key is pressed
    break;

    case 'b':
      // switch bird view
      isBirdView = !isBirdView;
    break;

    case '=':
    case '+':
      // zoom in camera in bird view
      if (isBirdView)
      {
        birdViewDistance -= 10.0f;
        if (birdViewDistance < 10.0f)
          birdViewDistance = 10.0f;
        glutPostRedisplay();
      }
    break;

    case '-':
    case '_':
      // zome out camera in bird view
      if (isBirdView)
      {
        birdViewDistance += 10.0f;
        if (birdViewDistance > 1000.0f)
          birdViewDistance = 1000.0f;
        glutPostRedisplay();
      }
    break;

    case 'w':
      speed += 0.001f;
      if (speed >= 0.1f) 
      {
        speed = 0.1f;
      }
    break;

    case 's':
      speed -= 0.001f;
      if (speed <= 0.001f) 
      {
        speed = 0.001f;
      }
    break;
  }
}

// detect release key 't', this is a replacement for CTRL
void keyboardUpFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 't':
      isTranslatingKeyPressed = false; // 't' key is released
    break;
  }
}

// ---------- level 1 functions ----------
// (milestone function)
void readSpline()
{
  // Clear previous data
  pointPosition.clear();
  pointColor.clear();

  // Catmull-Rom Spline Matrix
  // s = 0.5
  /* M = 
  [-0.5  1.5 -1.5  0.5
    1   -2.5  2   -0.5
    -0.5  0    0.5  0
    0    1    0    0  ]
  */
  float M[16] = {
    -0.5f,  1.0f, -0.5f,  0.0f,  // Column 0
     1.5f, -2.5f,  0.0f,  1.0f,  // Column 1
    -1.5f,  2.0f,  0.5f,  0.0f,  // Column 2
     0.5f, -0.5f,  0.0f,  0.0f   // Column 3
  };

  // Iterate over spline segments
  for (int i = 0; i < spline.numControlPoints - 3; i++)
  {
    // control points p0, p1, p2, p3
    Point p1 = spline.points[i];
    Point p2 = spline.points[i + 1];
    Point p3 = spline.points[i + 2];
    Point p4 = spline.points[i + 3];

    // Create control matrix C (4x3) in column-major order
    float C[12] = {
      p1.x, p2.x, p3.x, p4.x, // Column 0
      p1.y, p2.y, p3.y, p4.y, // Column 1
      p1.z, p2.z, p3.z, p4.z  // Column 2
    };

    // Generate points along the spline segment
    float u = 0.0f;
    float step = 0.001f;

    while (u <= 1.0f)
    {
      // parameter vector U = [u^3, u^2, u, 1]
      float U[4] = { u * u * u, u * u, u, 1.0f };

      // Compute UM = U * M (1x4 * 4x4 = 1x4)
      float UM[4];
      multiplyMatrices(1, 4, 4, U, M, UM);

      // Compute P = UM * C (1x4 * 4x3 = 1x3)
      float P[3];
      multiplyMatrices(1, 4, 3, UM, C, P);

      // Store the computed point
      pointPosition.push_back(P[0]); // X
      pointPosition.push_back(P[1]); // Y
      pointPosition.push_back(P[2]); // Z

      // Assign a color (e.g., red)
      pointColor.push_back(1.0f); // R
      pointColor.push_back(0.0f); // G
      pointColor.push_back(1.0f); // B
      pointColor.push_back(1.0f); // A

      u += step;
    }
  }
  // Update the number of vertices
  numVertices = static_cast<int>(pointPosition.size() / 3);
}

// ---------- level 2 functions ----------
void updateCamera(const float* position, const float* tangent, const float* normal, const float* binormal)
{
  // offsets
  float heightOffset = 0.1f;
  float sideOffset = 0.0f;

  // camera position
  float cameraPosition[3] = {
    position[0] + normal[0] * heightOffset + binormal[0] * sideOffset,
    position[1] + normal[1] * heightOffset + binormal[1] * sideOffset,
    position[2] + normal[2] * heightOffset + binormal[2] * sideOffset
  };

  // look-at point
  float lookAt[3] = {
    cameraPosition[0] + tangent[0],
    cameraPosition[1] + tangent[1],
    cameraPosition[2] + tangent[2]
  };

  // modelview matrix
  matrix.SetMatrixMode(OpenGLMatrix::ModelView);
  matrix.LoadIdentity();
  matrix.LookAt(
    cameraPosition[0], cameraPosition[1], cameraPosition[2], // Camera position
    lookAt[0], lookAt[1], lookAt[2],                         // Look-at point
    normal[0], normal[1], normal[2]                          // Up vector
  );
}

void computeCameraFrame(float u, int segmentIndex)
{ 
  // bird view
  if (isBirdView)
  {
    return;
  }

  // control points for the current segment
  Point p0 = spline.points[segmentIndex];
  Point p1 = spline.points[segmentIndex + 1];
  Point p2 = spline.points[segmentIndex + 2];
  Point p3 = spline.points[segmentIndex + 3];

  // control matrix C
  float C[12] = {
      p0.x, p1.x, p2.x, p3.x,
      p0.y, p1.y, p2.y, p3.y,
      p0.z, p1.z, p2.z, p3.z
  };

  // position P
  float U[4] = { u * u * u, u * u, u, 1.0f };
  float UM[4];
  multiplyMatrices(1, 4, 4, U, M, UM);
  float position[3];
  multiplyMatrices(1, 4, 3, UM, C, position);

  // tangent T
  float tangent[3];
  computeTangent(u, M, C, tangent);

  // normal and binormal
  static float prevNormal[3] = { 0.0f, 1.0f, 0.0f }; // Initial normal vector
  float normal[3], binormal[3];
  computeBinormal(tangent, prevNormal, binormal);
  computeNormal(tangent, normal, binormal);

  // update previous normal for continuity
  prevNormal[0] = normal[0];
  prevNormal[1] = normal[1];
  prevNormal[2] = normal[2];

  // update camera
  updateCamera(position, tangent, normal, binormal);
}

void setBirdViewCamera()
{
  // set to 45 degree
  float elevationAngle = 45.0f * (M_PI / 180.0f);
  float azimuthAngle = -90.0f * (M_PI / 180.0f);

  // camera position
  float cameraX = birdViewDistance * cos(elevationAngle) * sin(azimuthAngle);
  float cameraY = birdViewDistance * sin(elevationAngle);
  float cameraZ = birdViewDistance * cos(elevationAngle) * cos(azimuthAngle);

  // modelview matrix
  matrix.SetMatrixMode(OpenGLMatrix::ModelView);
  matrix.LoadIdentity();
  matrix.LookAt(
    cameraX, cameraY, cameraZ,   // camera position
    0.0f, 0.0f, 0.0f,            // look-at point
    0.0f, 1.0f, 0.0f             // up vector
  );
}

// ---------- level 3 functions ----------
//compute rectangle
void computeCrossSectionVertices(const float* position, const float* normal, const float* binormal, float radius, int numSegments, std::vector<float>& vertices)
{
  //circle points
  for (int i = 0; i < numSegments; ++i)
  {
    float theta = (2.0f * M_PI * i) / numSegments;
    float x = radius * cosf(theta);
    float y = radius * sinf(theta);

    // vertex position
    float vx = position[0] + normal[0] * x + binormal[0] * y;
    float vy = position[1] + normal[1] * x + binormal[1] * y;
    float vz = position[2] + normal[2] * x + binormal[2] * y;

    vertices.push_back(vx);
    vertices.push_back(vy);
    vertices.push_back(vz);
  }
}

// (milestone function)
/*
void generateRailGeometry()
{
  // clear previous data
  pointPosition.clear();
  pointColor.clear();
  railIndices.clear();

  // Parameters
  float sideLength = 0.1f;
  float step = 0.1f;

  // Variables to hold previous normal and binormal
  float prevNormal[3] = { 0.0f, 1.0f, 0.0f };

  // Vertex counter
  int vertexCount = 0;

  // Iterate over spline segments
  for (int i = 0; i < spline.numControlPoints - 3; i++)
  {
    // control points
    Point p0 = spline.points[i];
    Point p1 = spline.points[i + 1];
    Point p2 = spline.points[i + 2];
    Point p3 = spline.points[i + 3];

    // control matrix C
    float C[12] = {
      p0.x, p1.x, p2.x, p3.x,
      p0.y, p1.y, p2.y, p3.y,
      p0.z, p1.z, p2.z, p3.z
    };
    
    float u = 0.0f;

    while (u <= 1.0f)
    {
      // U
      float U[4] = { u * u * u, u * u, u, 1.0f };

      // UM
      float UM[4];
      multiplyMatrices(1, 4, 4, U, M, UM);

      // P
      float position[3];
      multiplyMatrices(1, 4, 3, UM, C, position);

      // Compute tangent t(u)
      float tangent[3];
      computeTangent(u, M, C, tangent);

      // Compute normal and binormal
      float normal[3], binormal[3];
      computeBinormal(tangent, prevNormal, binormal);
      computeNormal(tangent, normal, binormal);

      // Update prevNormal for continuity
      prevNormal[0] = normal[0];
      prevNormal[1] = normal[1];
      prevNormal[2] = normal[2];

      // Compute cross-section vertices
      std::vector<float> crossSectionVertices;
      computeCrossSectionVertices(position, normal, binormal, sideLength, crossSectionVertices);

      // store vertices and colors
      for (int k = 0; k < 4; ++k)
      {
        // Position
        pointPosition.push_back(crossSectionVertices[k * 3 + 0]);
        pointPosition.push_back(crossSectionVertices[k * 3 + 1]);
        pointPosition.push_back(crossSectionVertices[k * 3 + 2]);

        // Color (set to the normal vector components)
        pointColor.push_back(normal[0]); // R
        pointColor.push_back(normal[1]); // G
        pointColor.push_back(normal[2]); // B
        // pointColor.push_back(1.0f);      // A
      }

      // store indices
      if (vertexCount >= 4)
      {
        int baseIndex = vertexCount - 4;

        // Connect the current cross-section with the previous one
        for (int side = 0; side < 4; ++side)
        {
          int current = vertexCount + side;
          int next = vertexCount + ((side + 1) % 4);
          int prev = vertexCount - 4 + side;
          int prevNext = vertexCount - 4 + ((side + 1) % 4);

          railIndices.push_back(prev);
          railIndices.push_back(prevNext);
          railIndices.push_back(current);

          railIndices.push_back(current);
          railIndices.push_back(prevNext);
          railIndices.push_back(next);
        }
      }

      // Update vertex count
      vertexCount += 4;

      // Increment u
      u += step;
    }
  }

  numVertices = vertexCount;
  cout << "num vertices is " << numVertices << endl;
  numIndices = static_cast<int>(railIndices.size());
  cout << "num indices is " << numIndices << endl;
}
*/

void computeTieCylinder(const float* position, const float* axis, const float* normal, const float* binormal, float radius, float height, int numSegments, std::vector<float>& vertices, std::vector<float>& normals)
{
  float halfHeight = height / 2.0f;

  // compute bottom and top centers
  float bottomCenter[3], topCenter[3];
  for (int i = 0; i < 3; ++i)
  {
    bottomCenter[i] = position[i] - axis[i] * halfHeight;
    topCenter[i] = position[i] + axis[i] * halfHeight;
  }

  // vertices
  for (int i = 0; i <= numSegments; ++i)
  {
    float theta = (2.0f * M_PI * i) / numSegments;
    float x = radius * cosf(theta);
    float y = radius * sinf(theta);

    // direction vector
    float dir[3];
    for (int j = 0; j < 3; ++j)
    {
      dir[j] = normal[j] * x + binormal[j] * y;
    }

    // bottom
    vertices.push_back(bottomCenter[0] + dir[0]);
    vertices.push_back(bottomCenter[1] + dir[1]);
    vertices.push_back(bottomCenter[2] + dir[2]);

    normals.push_back(dir[0]);
    normals.push_back(dir[1]);
    normals.push_back(dir[2]);

    // top
    vertices.push_back(topCenter[0] + dir[0]);
    vertices.push_back(topCenter[1] + dir[1]);
    vertices.push_back(topCenter[2] + dir[2]);

    normals.push_back(dir[0]);
    normals.push_back(dir[1]);
    normals.push_back(dir[2]);
  }
}

void computePillarCylinder(const float* startPosition, float groundY, float radius, float height, int numSegments, std::vector<float>& vertices, std::vector<float>& normals, std::vector<unsigned int>& indices, int& vertexCount)
{
  // compute bottom and top centers
  float bottomCenter[3] = { startPosition[0], groundY, startPosition[2] };
  float topCenter[3] = { startPosition[0], startPosition[1], startPosition[2] };

  // axis
  float axis[3] = { 0.0f, 1.0f, 0.0f };

  // vertices
  for (int i = 0; i <= numSegments; ++i)
  {
    float theta = (2.0f * M_PI * i) / numSegments;
    float x = radius * cosf(theta);
    float z = radius * sinf(theta);

    // bottom
    vertices.push_back(bottomCenter[0] + x);
    vertices.push_back(bottomCenter[1]);
    vertices.push_back(bottomCenter[2] + z);

    normals.push_back(x);
    normals.push_back(0.0f);
    normals.push_back(z);

    // top
    vertices.push_back(topCenter[0] + x);
    vertices.push_back(topCenter[1]);
    vertices.push_back(topCenter[2] + z);

    normals.push_back(x);
    normals.push_back(0.0f);
    normals.push_back(z);
  }

  // indices
  int baseIndex = vertexCount;
  for (int i = 0; i < numSegments; ++i)
  {
    int idx0 = baseIndex + i * 2;
    int idx1 = baseIndex + i * 2 + 1;
    int idx2 = baseIndex + ((i + 1) % (numSegments + 1)) * 2;
    int idx3 = baseIndex + ((i + 1) % (numSegments + 1)) * 2 + 1;

    indices.push_back(idx0);
    indices.push_back(idx2);
    indices.push_back(idx1);

    indices.push_back(idx1);
    indices.push_back(idx2);
    indices.push_back(idx3);
  }

  // update vertex count
  vertexCount += (numSegments + 1) * 2;
}

void generateRailGeometry()
{
  // clear vertex data
  pointPosition.clear();
  pointColor.clear();
  railIndices.clear();

  // clear tie data
  tiePosition.clear();
  tieNormal.clear();
  tieIndices.clear();

  // clear pillar data
  pillarPosition.clear();
  pillarNormal.clear();
  pillarIndices.clear();

  // parameters
  float sideLength = 0.05f;              // thickness of rail
  float step = 0.001f;                   // step size
  float halfRailSpacing = 0.25f;         // half the distance between rails
  float tieRadius = 0.02f;               // tie radius
  float tieHeight = 2 * halfRailSpacing; // distance between left rail and right rail
  float pillarRadius = 0.05f;            // pillar radius
  float groundY = -100.0f;               // Y position of the ground
  int numSegments = 16;                  // number of segments for circular cross-sections

  // spacing for tie and pillar
  float desiredTieSpacing = 2.0f;
  float desiredPillarSpacing = desiredTieSpacing * 3;

  // Counters and accumulators
  int vertexCount = 0;
  int tieVertexCount = 0;
  int pillarVertexCount = 0;
  float cumulativeTieDistance = 0.0f;
  float cumulativePillarDistance = 0.0f;

  // Variables for previous position
  float previousPosition[3] = {0.0f, 0.0f, 0.0f};
  bool isFirstPoint = true;

  // Variables for previous normal and binormal
  float prevNormal[3] = {0.0f, 1.0f, 0.0f};

  // Iterate over spline segments
  for (int i = 0; i < spline.numControlPoints - 3; i++)
  {
    // control points
    Point p0 = spline.points[i];
    Point p1 = spline.points[i + 1];
    Point p2 = spline.points[i + 2];
    Point p3 = spline.points[i + 3];

    // control matrix C
    float C[12] = {
      p0.x, p1.x, p2.x, p3.x,
      p0.y, p1.y, p2.y, p3.y,
      p0.z, p1.z, p2.z, p3.z
    };

    float u = 0.0f;

    while (u <= 1.0f)
    {
      // U
      float U[4] = { u * u * u, u * u, u, 1.0f };

      // UM = U * M
      float UM[4];
      multiplyMatrices(1, 4, 4, U, M, UM);

      // P
      float position[3];
      multiplyMatrices(1, 4, 3, UM, C, position);

      // tangent T
      float tangent[3];
      computeTangent(u, M, C, tangent);

      // compute normal and binormal
      float normal[3], binormal[3];
      computeBinormal(tangent, prevNormal, binormal);
      computeNormal(tangent, normal, binormal);

      // update prevNormal for continuity
      memcpy(prevNormal, normal, sizeof(prevNormal));

      // ----- rail -----
      // compute positions for left and right rails
      float position_left[3], position_right[3];
      for (int j = 0; j < 3; ++j)
      {
        position_left[j] = position[j] - binormal[j] * halfRailSpacing;
        position_right[j] = position[j] + binormal[j] * halfRailSpacing;
      }

      // cross-section vertices for left and right rails
      std::vector<float> crossSectionVerticesLeft;
      computeCrossSectionVertices(position_left, normal, binormal, sideLength, numSegments, crossSectionVerticesLeft);

      std::vector<float> crossSectionVerticesRight;
      computeCrossSectionVertices(position_right, normal, binormal, sideLength, numSegments, crossSectionVerticesRight);

      // left rail
      for (int k = 0; k < numSegments; ++k)
      {
        pointPosition.push_back(crossSectionVerticesLeft[k * 3 + 0]);
        pointPosition.push_back(crossSectionVerticesLeft[k * 3 + 1]);
        pointPosition.push_back(crossSectionVerticesLeft[k * 3 + 2]);
        pointColor.push_back(normal[0]);
        pointColor.push_back(normal[1]);
        pointColor.push_back(normal[2]);
      }

      // right rail
      for (int k = 0; k < numSegments; ++k)
      {
        pointPosition.push_back(crossSectionVerticesRight[k * 3 + 0]);
        pointPosition.push_back(crossSectionVerticesRight[k * 3 + 1]);
        pointPosition.push_back(crossSectionVerticesRight[k * 3 + 2]);
        pointColor.push_back(normal[0]);
        pointColor.push_back(normal[1]);
        pointColor.push_back(normal[2]);
      }

      // left rail indices
      if (vertexCount >= 2 * numSegments)
      {
        int baseIndexLeft = vertexCount - 2 * numSegments;
        
        for (int side = 0; side < numSegments; ++side)
        {
          int current = vertexCount + side;
          int next = vertexCount + ((side + 1) % numSegments);
          int prev = baseIndexLeft + side;
          int prevNext = baseIndexLeft + ((side + 1) % numSegments);

          // triangle 1
          railIndices.push_back(prev);
          railIndices.push_back(prevNext);
          railIndices.push_back(current);

          // triangle 2
          railIndices.push_back(current);
          railIndices.push_back(prevNext);
          railIndices.push_back(next);
        }
      }

      // right rail indices
      if (vertexCount >= 2 * numSegments)
      {
        int baseIndexRight = vertexCount - numSegments;
        
        for (int side = 0; side < numSegments; ++side)
        {
          int current = vertexCount + numSegments + side;
          int next = vertexCount + numSegments + ((side + 1) % numSegments);
          int prev = baseIndexRight + side;
          int prevNext = baseIndexRight + ((side + 1) % numSegments);

          // triangle 1
          railIndices.push_back(prev);
          railIndices.push_back(prevNext);
          railIndices.push_back(current);

          // triangle 2
          railIndices.push_back(current);
          railIndices.push_back(prevNext);
          railIndices.push_back(next);
        }
      }

      // update vertex count
      vertexCount += 2 * numSegments; // numSegments vertices for each rail

      // ----- tie and pillar -----
      if (!isFirstPoint)
      {
        float dx = position[0] - previousPosition[0];
        float dy = position[1] - previousPosition[1];
        float dz = position[2] - previousPosition[2];
        float segmentDistance = sqrt(dx * dx + dy * dy + dz * dz);

        // update cumulative distances
        cumulativeTieDistance += segmentDistance;
        cumulativePillarDistance += segmentDistance;

        // place tie
        if (cumulativeTieDistance >= desiredTieSpacing)
        {
          // ----- Tie -----
          int tieSegments = 16;

          // generate tie cylinder
          std::vector<float> tieVertices;
          std::vector<float> tieNormals;
          computeTieCylinder(position, binormal, normal, tangent, tieRadius, tieHeight, tieSegments, tieVertices, tieNormals);

          // add vertices and normals
          tiePosition.insert(tiePosition.end(), tieVertices.begin(), tieVertices.end());
          tieNormal.insert(tieNormal.end(), tieNormals.begin(), tieNormals.end());

          // indices
          int baseIndex = tieVertexCount;
          for (int i = 0; i < tieSegments; ++i)
          {
            int idx0 = baseIndex + i * 2;
            int idx1 = baseIndex + i * 2 + 1;
            int idx2 = baseIndex + ((i + 1) % (tieSegments)) * 2;
            int idx3 = baseIndex + ((i + 1) % (tieSegments)) * 2 + 1;

            tieIndices.push_back(idx0);
            tieIndices.push_back(idx2);
            tieIndices.push_back(idx1);
            tieIndices.push_back(idx1);
            tieIndices.push_back(idx2);
            tieIndices.push_back(idx3);
          }

        // update tie vertex count
        tieVertexCount += (tieSegments + 1) * 2;

        // reset tie distance
        cumulativeTieDistance = 0.0f;

        // ----- pillar -----
        if (cumulativePillarDistance >= desiredPillarSpacing)
        {
          // position
          float position_left[3], position_right[3];
          for (int j = 0; j < 3; ++j)
          {
            position_left[j] = position[j] - binormal[j] * halfRailSpacing;
            position_right[j] = position[j] + binormal[j] * halfRailSpacing;
          }

          // ddjust pillar position
          float pillarStartLeft[3] = { position_left[0], position_left[1], position_left[2] };
          float pillarStartRight[3] = { position_right[0], position_right[1], position_right[2] };

          // compute pillar length
          float pillarHeightLeft = pillarStartLeft[1] - groundY;
          float pillarHeightRight = pillarStartRight[1] - groundY;

          // left pillar
          computePillarCylinder(pillarStartLeft, groundY, pillarRadius, pillarHeightLeft, numSegments, pillarPosition, pillarNormal, pillarIndices, pillarVertexCount);

          // right pillar
          computePillarCylinder(pillarStartRight, groundY, pillarRadius, pillarHeightRight, numSegments, pillarPosition, pillarNormal, pillarIndices, pillarVertexCount);

          // reset pillar distance
          cumulativePillarDistance = 0.0f;
          }
        }
      }
      else
      {
        isFirstPoint = false;
      }

      // update previous position
      previousPosition[0] = position[0];
      previousPosition[1] = position[1];
      previousPosition[2] = position[2];

      // increment u
      u += step;
    }
  }

  // update vertex count
  numVertices = vertexCount;
  numIndices = static_cast<int>(railIndices.size());

  // update tie count
  numTieVertices = tieVertexCount;
  numTieIndices = static_cast<int>(tieIndices.size());

  // update pillar count
  numPillarIndices = static_cast<int>(pillarIndices.size());
}

// ----------- level 4 functions: background -----------
// I commented the line for generating the ground since I added the skybox
void initGroundTexture(const char* textureFilename)
{
  // generate texture handle
  glGenTextures(1, &groundTextureHandle);

  // bind
  glBindTexture(GL_TEXTURE_2D, groundTextureHandle);

  // init
  initTexture(textureFilename, groundTextureHandle);
}

void initGroundVAO()
{
  // Generate and bind VAO
  groundVAO.Gen();
  groundVAO.Bind();

  // Create VBO for Positions and Texture Coordinates
  groundVBOPosition.Gen(groundPositions.size(), 5, groundPositions.data(), GL_STATIC_DRAW);
  groundVBOPosition.Bind();

  // Positions (location = 0)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

  // Texture Coordinates (location = 1)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  // Create and Bind EBO
  groundEBO.Gen(groundIndices.size() * sizeof(unsigned int), groundIndices.data(), GL_STATIC_DRAW);
  groundEBO.Bind();

  // Unbind VAO
  glBindVertexArray(0);
}

// ----------- skybox ----------
void initSkyboxTextures()
{
  const char* filenames[6] = {
    "skybox/right.jpg",
    "skybox/left.jpg",
    "skybox/front.jpg",
    "skybox/back.jpg",
    "skybox/bottom.jpg",
    "skybox/top.jpg"
  };

  // generate texture handles
  glGenTextures(6, skyboxTextureHandles);

  // init
  for (int i = 0; i < 6; ++i)
  {
    if (initTexture(filenames[i], skyboxTextureHandles[i]) != 0)
    {
      cout << "Failed to initialize skybox texture: " << filenames[i] << endl;
      throw 1;
    }
  }
}

void initSkyboxVAO()
{
  // VAO
  skyboxVAO.Gen();
  skyboxVAO.Bind();

  // VBO
  skyboxVBO.Gen(skyboxVertices.size(), 5, skyboxVertices.data(), GL_STATIC_DRAW);
  skyboxVBO.Bind();

  // Positions (location = 0)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

  // Texture Coordinates (location = 1)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  // EBO
  skyboxEBO.Gen(skyboxIndices.size() * sizeof(unsigned int), skyboxIndices.data(), GL_STATIC_DRAW);
  skyboxEBO.Bind();

  // Unbind VAO
  glBindVertexArray(0);
}

// ---------- initializing ----------
// initialize VAO and VBO
void initVAOVBO()
{
  // VAO
  VAOVertice.Gen();
  VAOVertice.Bind();

  // VBO for position
  VBOPosition.Gen(pointPosition.size(), 3, pointPosition.data(), GL_STATIC_DRAW);
  VBOPosition.Bind();

  // Enable "position" attribute (location = 0)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // VBO for color
  VBOColor.Gen(pointColor.size(), 3, pointColor.data(), GL_STATIC_DRAW);
  VBOColor.Bind();

  // enable "normal" attribute in vertex shader (location = 1)
  // original was "color" attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // EBO
  railEBO.Gen(railIndices.size() * sizeof(unsigned int), railIndices.data(), GL_STATIC_DRAW);
  railEBO.Bind();

  // Unbind VAO
  glBindVertexArray(0);

  cout << "Successfully built the VAO, VBOs, and EBO for the rail." << endl;
}

void initTieVAOVBO()
{
  // VAO
  VAOVerticeTie.Gen();
  VAOVerticeTie.Bind();

  // VBO for position
  VBOPositionTie.Gen(tiePosition.size(), 3, tiePosition.data(), GL_STATIC_DRAW);
  VBOPositionTie.Bind();

  // Enable "position" attribute (location = 0)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // VBO for normal
  VBONormalTie.Gen(tieNormal.size(), 3, tieNormal.data(), GL_STATIC_DRAW);
  VBONormalTie.Bind();

  // Enable "normal" attribute (location = 1)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // EBO
  EBOIndexTie.Gen(tieIndices.size() * sizeof(unsigned int), tieIndices.data(), GL_STATIC_DRAW);
  EBOIndexTie.Bind();

  // Unbind VAO
  glBindVertexArray(0);

  cout << "Successfully built the VAO, VBOs, and EBO for the ties." << endl;
}

void initPillarVAOVBO()
{
  // VAO
  pillarVAO.Gen();
  pillarVAO.Bind();

  // VBO for position
  pillarVBOPosition.Gen(pillarPosition.size(), 3, pillarPosition.data(), GL_STATIC_DRAW);
  pillarVBOPosition.Bind();

  // Enable "position" attribute (location = 0)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // VBO for normal
  pillarVBONormal.Gen(pillarNormal.size(), 3, pillarNormal.data(), GL_STATIC_DRAW);
  pillarVBONormal.Bind();

  // Enable "normal" attribute (location = 1)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // EBO
  pillarEBO.Gen(pillarIndices.size() * sizeof(unsigned int), pillarIndices.data(), GL_STATIC_DRAW);
  pillarEBO.Bind();

  // Unbind VAO
  glBindVertexArray(0);

  cout << "Successfully built the VAO, VBOs, and EBO for the pillars." << endl;
}

// actual rendering
void displayFunc()
{
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // read the current modelview and projection matrices
  float modelViewMatrix[16];
  matrix.SetMatrixMode(OpenGLMatrix::ModelView);
  matrix.GetMatrix(modelViewMatrix);

  float projectionMatrix[16];
  matrix.SetMatrixMode(OpenGLMatrix::Projection);
  matrix.GetMatrix(projectionMatrix);

  // ----- skybox -----
  // save the current modelview matrix
  float originalModelViewMatrix[16];
  memcpy(originalModelViewMatrix, modelViewMatrix, sizeof(float) * 16);

  // remove the translation component from the modelview matrix
  modelViewMatrix[12] = 0.0f;
  modelViewMatrix[13] = 0.0f;
  modelViewMatrix[14] = 0.0f;

  // disable depth writing and set depth function to less or equal
  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);

  // bind pipeline
  texturePipelineProgram.Bind();

  // set uniforms
  texturePipelineProgram.SetUniformVariableMatrix4fv("modelViewMatrix", GL_FALSE, modelViewMatrix);
  texturePipelineProgram.SetUniformVariableMatrix4fv("projectionMatrix", GL_FALSE, projectionMatrix);
  texturePipelineProgram.SetUniformVariablei("textureSampler", 0); // Texture unit 0

  // bind VAO
  skyboxVAO.Bind();

  // render
  for (int i = 0; i < 6; ++i)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyboxTextureHandles[i]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * i * 6));
  }

  // unbind VAO
  glBindVertexArray(0);

  // restore depth function and enable depth writing
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  // restore original modelview matrix
  memcpy(modelViewMatrix, originalModelViewMatrix, sizeof(float) * 16);

  // ----- ground pipeline -----
  texturePipelineProgram.Bind();

  // Set uniforms
  texturePipelineProgram.SetUniformVariableMatrix4fv("modelViewMatrix", GL_FALSE, modelViewMatrix);
  texturePipelineProgram.SetUniformVariableMatrix4fv("projectionMatrix", GL_FALSE, projectionMatrix);
  texturePipelineProgram.SetUniformVariablei("textureSampler", 0); // Texture unit 0

  // Bind the ground texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, groundTextureHandle);

  // Bind the ground VAO
  groundVAO.Bind();

  // Draw the ground plane (level 4)
  // glDrawElements(GL_TRIANGLES, numGroundIndices, GL_UNSIGNED_INT, 0);

  // Unbind the ground VAO
  glBindVertexArray(0);

  // ----- rail pipeline -----
  // bind pipelineProgram
  pipelineProgram.Bind();

  // set uniform variable
  pipelineProgram.SetUniformVariableMatrix4fv("modelViewMatrix", GL_FALSE, modelViewMatrix);
  pipelineProgram.SetUniformVariableMatrix4fv("projectionMatrix", GL_FALSE, projectionMatrix);

  // ----- phong -----
  // vertex shader
  float normalMatrix[9];
  computeNormalMatrix(modelViewMatrix, normalMatrix);
  pipelineProgram.SetUniformVariableMatrix3fv("normalMatrix", GL_FALSE, normalMatrix);
  
  // fragment shader
  pipelineProgram.SetUniformVariable4f("La", 0.5f, 0.5f, 0.5f, 1.0f);
  pipelineProgram.SetUniformVariable4f("Ld", 0.7f, 0.7f, 0.7f, 1.0f);
  pipelineProgram.SetUniformVariable4f("Ls", 0.9f, 0.9f, 0.9f, 1.0f);
  pipelineProgram.SetUniformVariable3f("viewLightDirection", 0.0f, 0.0f, -1.0f);

  pipelineProgram.SetUniformVariable4f("ka", 0.8f, 0.2f, 0.2f, 1.0f);
  pipelineProgram.SetUniformVariable4f("kd", 0.8f, 0.5f, 0.5f, 1.0f);
  pipelineProgram.SetUniformVariable4f("ks", 0.3f, 0.3f, 0.3f, 1.0f);
  pipelineProgram.SetUniformVariablef("alpha", 1.0f);

  // bind VAOVertice
  VAOVertice.Bind();

  // draw spline (milestone)
  // glDrawArrays(GL_LINE_STRIP, 0, numVertices);

  // draw rail
  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
  
  // unbind VAO
  glBindVertexArray(0);

  VAOVerticeTie.Bind();

  // draw tie
  glDrawElements(GL_TRIANGLES, tieIndices.size(), GL_UNSIGNED_INT, 0);

  // unbind VAO
  glBindVertexArray(0);

  // do not display pillar for circular.sp
  if (spFilename != "splines/circular.sp"){
    pillarVAO.Bind();
    glDrawElements(GL_TRIANGLES, numPillarIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  // Swap the double-buffers.
  glutSwapBuffers();
}

void initScene(int argc, char *argv[])
{ 
  // Load the spline data
  readSpline();

  // Generate the rail geometry
  generateRailGeometry();

  // Set the background color.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black color.

  // Enable z-buffering (i.e., hidden surface removal using the z-buffer algorithm).
  glEnable(GL_DEPTH_TEST);
  
  // ----- rail pipeline -----
  // Create a pipeline program. This operation must be performed BEFORE we initialize any VAOs.
  // A pipeline program contains our shaders. Different pipeline programs may contain different shaders.
  // In this homework, we only have one set of shaders, and therefore, there is only one pipeline program.
  // In hw2, we will need to shade different objects with different shaders, and therefore, we will have
  // several pipeline programs (e.g., one for the rails, one for the ground/sky, etc.).
  // Load and set up the pipeline program, including its shaders.
  // if (pipelineProgram.BuildShadersFromFiles(shaderBasePath, "vertexShader.glsl", "fragmentShader.glsl") != 0)
  if (pipelineProgram.BuildShadersFromFiles(shaderBasePath, "phongVertexShader.glsl", "phongFragmentShader.glsl") != 0)
  {
    cout << "Failed to build the pipeline program." << endl;
    throw 1;
  } 
  cout << "Successfully built the pipeline program." << endl;
    
  // Bind the pipeline program that we just created. 
  // The purpose of binding a pipeline program is to activate the shaders that it contains, i.e.,
  // any object rendered from that point on, will use those shaders.
  // When the application starts, no pipeline program is bound, which means that rendering is not set up.
  // So, at some point (such as below), we need to bind a pipeline program.
  // From that point on, exactly one pipeline program is bound at any moment of time.
  pipelineProgram.Bind();

  // create VAO and VBO
  initVAOVBO();
  initTieVAOVBO(); 
  initPillarVAOVBO();

  // ----- texture pipeline -----

  // Initialize the texture pipeline program
  if (texturePipelineProgram.BuildShadersFromFiles(shaderBasePath, "textureVertexShader.glsl", "textureFragmentShader.glsl") != 0)
  {
    cout << "Failed to build the texture pipeline program." << endl;
    throw 1;
  }
  cout << "Successfully built the texture pipeline program." << endl;
  
  // bind texture pipeline
  texturePipelineProgram.Bind();

  // Initialize the ground VAO and texture
  initGroundVAO();
  initGroundTexture("textures/grass1024.jpg");

  // Initialize the skybox textures
  initSkyboxTextures();
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  // Initialize the skybox VAO and VBO
  initSkyboxVAO();

  // Check for any OpenGL errors.
  cout << "GL error status is: " << glGetError() << endl;
}


int main(int argc, char *argv[])
{ 
  // check usage
  if (argc < 2)
  {  
    printf ("Usage: %s <spline file>\n", argv[0]);
    exit(0);
  }

  // save file name
  spFilename = argv[1];

  // Load spline from the provided filename.
  loadSpline(argv[1]);

  printf("Loaded spline with %d control point(s).\n", spline.numControlPoints);

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

  #ifdef __APPLE__
    // This is needed on recent Mac OS X versions to correctly display the window.
    glutReshapeWindow(windowWidth - 1, windowHeight - 1);
  #endif

  // Tells GLUT to use a particular display function to redraw.
  glutDisplayFunc(displayFunc);
  // Perform animation inside idleFunc.
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
  // callback for unpressing the key 't'
  glutKeyboardUpFunc(keyboardUpFunc);

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

  // Perform the initialization.
  initScene(argc, argv);

  // Sink forever into the GLUT loop.
  glutMainLoop();
}