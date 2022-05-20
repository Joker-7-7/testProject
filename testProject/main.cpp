

//#include "GL/glut.h""       // for using gluLookAt( ... )
#include "glad/glad.h"
#include "GLFW/glfw3.h"

//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//#include "glm/gtc/type_ptr.hpp"
// include files

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <list>
#include <sstream>
#include <iomanip>
#include <locale>
#include <cstdlib>

#include "structs_and_classes.h"

// function prototypes
// the following functions are used by main(), but defined in other source files (.cpp files)

extern int getGeometryInput(char* argv[],
    std::vector<triangle>& facet,
    double& x_min, double& x_max,
    double& y_min, double& y_max,
    double& z_min, double& z_max);

extern void drawGeometry(char* argv[], GLFWwindow* window, float scale, int time,
    float* vertices, int NUM_VERTICES,
    float move_x, float move_y, float move_z,
    float rot_x, float rot_y, float rot_z);

void createVertexArray(std::vector<triangle>& facet, float* vertices);

// variables and constants used in main.cpp

// input geometry

std::vector<triangle> facet;        // define a vector object (container) called "facet" of type "triangle"
double x_min, x_max, delta_x;       // geometry extents along X
double y_min, y_max, delta_y;       // geometry extents along Y
double z_min, z_max, delta_z;       // geometry extents along Z


// state variables tied to keyboard input
static bool running = true;
static bool pause = false;
static float rotate_x = 0.0, drot_x = 0.0;
static float rotate_y = 0.0, drot_y = 0.0;
static float rotate_z = 0.0, drot_z = 0.0;

// keyboard input callback function
static void key_callback(GLFWwindow* window, int key, int scancode,
    int action, int mods)
{
    // quit application when the user presses ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        running = false;
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // pause application when user presses SPACEBAR and keeps
    // it pressed
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        pause = true;
    }

    // continue running the application when the user releases
    // the SPACEBAR
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        pause = false;
    }

    // rotate about Z axis if user presses the RIGHT arrow key 
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        drot_z = 1.0;
    }
    // stop rotation when the key is released
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
        drot_z = 0.0;
    }

    // rotate about -Z axis if user presses the LEFT arrow key 
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        drot_z = -1.0;
    }
    // stop rotation when the key is released
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
        drot_z = 0.0;
    }

    // rotate about X axis if user presses the UP arrow key 
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        drot_x = -1.0;
    }
    // stop rotation when the key is released
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
        drot_x = 0.0;
    }

    // rotate about -X axis if user presses the DOWN arrow key 
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        drot_x = 1.0;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
        drot_x = 0.0;
    }
}

// entry point
int main(int argc, char* argv[])
{
    // check if all command line arguments were supplied
    //const int CARGS = 1;
    //if (argc < 1 + CARGS)
    //{
    //    std::cout << "Please provide the following:\n\n";
    //    std::cout << "     [name of the STL file]  \n";
    //    return 1;
    //}

    //--------------------------------------------------
    // read STL geometry (triangle vertices and normals)
    //--------------------------------------------------
    int error = getGeometryInput(NULL, facet,
        x_min, x_max,
        y_min, y_max,
        z_min, z_max);

    if (error == 1) return(1);       // exit main returning an error

    //-------------------------------------------------------
    // calculate translation parameters to move the center of 
    // the geometry approximately to the origin
    //-------------------------------------------------------
    float x_center = (x_min + x_max) / 2.0;
    float y_center = (y_min + y_max) / 2.0;
    float z_center = (z_min + z_max) / 2.0;

    float move_x = 0.0 - x_center;   // translation along X
    float move_y = 0.0 - y_center;   // translation along Y
    float move_z = 0.0 - z_center;   // translation along Z

    std::cout << "Need to translate the geometry by: " << move_x
        << " " << move_y
        << " " << move_z << std::endl;

    // calculate scaling factors along X, Y and Z
    float scale_x = (x_max - x_min) / 2.0;
    float scale_y = (y_max - y_min) / 2.0;
    float scale_z = (z_max - z_min) / 2.0;

    float scale_0 = scale_x;
    if (scale_y > scale_0) scale_0 = scale_y;
    if (scale_z > scale_0) scale_0 = scale_z;

    std::cout << "Need to scale the geometry by: " << 1.0 / scale_0 << std::endl;

    //------------------------------------------
    // create a vertex array based on facet data
    //------------------------------------------
    float* vertices = new float[facet.size() * 30];

    createVertexArray(facet, vertices);

    //--------------------------------
    //   Create a WINDOW using GLFW
    //--------------------------------

    glfwInit();                                                     // Инициализируем GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // Версия OpenGL - 3.3, основной и дополнительный номера версии равны 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Использовать основной режим (нет необходимости в обратной совместимости)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Если вы используете Mac OS X, добавьте эту строку
    glfwWindowHint(GLFW_RESIZABLE, 0);
    // window size for displaying graphics
    int WIDTH = 1440; //atoi(argv[1]);
    int HEIGHT = 960; //atoi(argv[2]);

    // set the window's display mode
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "STL viewer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // make the windows context current
    glfwMakeContextCurrent(window);

    // Инициализируем GLAD, загружаем функцию адреса указателя функции OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // register the keyboard input callback function defined at the top
    glfwSetKeyCallback(window, key_callback);

    // create a Vertex Buffer Object (VBO) and bind the vertex array to it
    // makes rendering faster because data is copied to GPU memory
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, facet.size() * 30 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    // fill mode or wireframe mode
    glPolygonMode(GL_FRONT,  // options: GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
        GL_FILL);           // options: GL_POINT, GL_LINE, GL_FILL (default)

// shading model
    glShadeModel(GL_SMOOTH);

    // initialize animation parameters 
    float scale = 5.0 * scale_0;    // initial scale
    float delta = 0.010 * scale_0;  // change in scale
    int frame = 0;

    while (running)
    {
        // render objects in the window
        drawGeometry(argv, window, scale, frame,
            vertices, 3 * facet.size(),
            move_x, move_y, move_z,
            rotate_x, rotate_y, rotate_z);

        // animate view if SPACEBAR is not pressed
        if (!pause)
        {
            // adjust scale (zoom-in and zoom-out)
            scale += delta;
            if ((scale > 10.0 * scale_0) || (scale < 0.1 * scale_0)) delta = -delta;

            // changes viewing angle
            rotate_x += drot_x;
            rotate_y += drot_y;
            rotate_z += drot_z;
        }

        // swap front and back buffers
        glfwSwapBuffers(window);

        // poll for and processs events
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;    // main program is successful
}