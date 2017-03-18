#include <string>

#include "gui.h"

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

void init_gl(int width, int height)
{
    // init OpenGL
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_ALPHA_TEST);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void update_fps(GLFWwindow *window)
{
    return;
    static double last_time = glfwGetTime();
    static int last_fps = 0;
    double current_time = glfwGetTime();
    ++last_fps;
    if (current_time - last_time >= 1.0)
    {
        char str[256];
#ifdef _WIN32
        sprintf_s(str, "Ray Tracer (FPS: %d)", last_fps);
#else
        snprintf(str, sizeof(str), "Ray Tracer (FPS: %d)", last_fps);
#endif
        glfwSetWindowTitle(window, str);
        last_time = glfwGetTime();
        last_fps = 0;
    }
}

void show_image(image &img)
{
#ifndef __APPLE__
#define COEFF 1
#else
#define COEFF 2 // retina
#endif

    //Initialize the library
    if (!glfwInit())
    {
        throw std::string("error glfwInit");
    }
    GLFWwindow *window;
    window = glfwCreateWindow(img.width / COEFF, img.height / COEFF,
                              "Ray Tracer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw std::string("Error glfwCreateWindow");
    }
    //Make the window's context current
    glfwMakeContextCurrent(window);
    init_gl(img.width / COEFF, img.height / COEFF);
    if(glGetError() != GL_NO_ERROR)
    {
        throw std::string("Error initing GL");
    }
    glfwSwapInterval(1);

    // Load texture.
    // Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
    size_t u2 = 1; while(u2 < img.width) u2 *= 2;
    size_t v2 = 1; while(v2 < img.height) v2 *= 2;
    // Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
    double u3 = (double)img.width / u2;
    double v3 = (double)img.height / v2;

    // Make power of two version of the image.
    std::vector<unsigned char> img2(u2 * v2 * 4);
    for(size_t y = 0; y < img.height; y++)
    {
        for(size_t x = 0; x < img.width; x++)
        {
            for(size_t c = 0; c < 4; c++)
            {
                img2[4 * u2 * y + 4 * x + c] = (*img.raw)[4 * img.width * y + 4 * x + c];
            }
        }
    }

    // Enable the texture for OpenGL.
    glEnable(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img2[0]);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glFlush();

        glBegin(GL_QUADS);
        glTexCoord2d( 0,  0); glVertex2f(                0,          0);
        glTexCoord2d(u3,  0); glVertex2f(img.width / COEFF,          0);
        glTexCoord2d(u3, v3); glVertex2f(img.width / COEFF, img.height / COEFF);
        glTexCoord2d( 0, v3); glVertex2f(                0, img.height / COEFF);
        glEnd();

        //Swap front and back buffers
        glfwSwapBuffers(window);

        update_fps(window);

        //Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
#undef COEFF
}