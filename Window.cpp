#include "Window.h"



Window::Window(int width, int height, const char* title)
    : camera(width,height,glm::vec3(-2.0f,3.0f,6.0f)) {
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void Window::Init() {
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, camera.getWidth(),camera.getHeight());

  
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::Terminate()
{
    glfwDestroyWindow(window);
    
}


void Window::PollEvents() {

    static 	int width, height, widthPrev, heightPrev;
   
    glfwGetWindowSize(window, &width, &height);
    if (width != widthPrev || height != heightPrev)
    {
        resizeWindow(width, height);
        widthPrev = width;
        heightPrev = height;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

GLFWwindow* Window::GetWindow() {
    return window;
}


void Window::resizeWindow(int width, int height)
{
    camera.setWidth(width);
    camera.setHeight(height);
    camera.setProjectionMatrix(glm::perspective(glm::radians(45.0f), float(camera.getWidth()) / float(camera.getHeight()), 0.1f, 100.0f));
    glViewport(0,0, width, height);
}

Camera& Window::getCamera() { return camera; }