#ifndef _GUI_H_
#define _GUI_H_

#ifdef GUI

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "image.h"

void init_gl(int width, int height);
void update_fps(GLFWwindow *window);
void show_image(image &img);

#endif

#endif // _GUI_H_