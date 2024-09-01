#include <GLFW/glfw3.h>
#include <string>

typedef struct {
    GLFWwindow *window;
    unsigned int w, h;

    bool should_close;

    std::string title;
} window_t;

void init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

window_t *create_window(unsigned int width = 1280, unsigned int height = 720, const char *title = "basic window") {
    window_t *window = new window_t;
    window->w = width; window->h = height; window->title = title;
    window->should_close = false;

    window->window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(window->window);

    return window;
}

void update_window(window_t *window) {
    window->should_close = glfwWindowShouldClose(window->window);
    glfwSwapBuffers(window->window);
    glfwPollEvents();
}

void destroy_glfw() {
    // TODO: make this thing
}