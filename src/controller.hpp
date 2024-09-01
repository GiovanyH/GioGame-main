#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

typedef struct {
    float yaw, pitch;
    glm::vec3 position;
} camera_t;

typedef struct {
    float delta_time;
    float last_frame;
} game_t;

typedef struct {
    float last_x, last_y;
    bool first_mouse;
} mouse_t;

void initialize_controller(GLFWwindow *window, GLFWcursorposfun fun) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, fun);
}

void update_input(float current_frame, game_t *main_game) {
    main_game->delta_time = current_frame - main_game->last_frame;
    main_game->last_frame = current_frame;
}

glm::vec3 get_camera_forward_vector(camera_t *camera) {
    glm::vec3 direction;
    direction.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    direction.y = sin(glm::radians(camera->pitch));
    direction.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    return glm::normalize(direction);
}

void mouse_callback_c(GLFWwindow* window, double xposIn, double yposIn, mouse_t *mouse, camera_t *main_camera)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (mouse->first_mouse)
    {
        mouse->last_x = xpos;
        mouse->last_y = ypos;
        mouse->first_mouse = false;
    }

    float xoffset = xpos - mouse->last_x;
    float yoffset = mouse->last_y - ypos; // reversed since y-coordinates go from bottom to top
    mouse->last_x = xpos;
    mouse->last_y = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    main_camera->yaw += xoffset;
    main_camera->pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (main_camera->pitch > 89.0f)
        main_camera->pitch = 89.0f;
    if (main_camera->pitch < -89.0f)
        main_camera->pitch = -89.0f;
}

void process_input(GLFWwindow *window, game_t *game, camera_t *main_camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec3 forward = get_camera_forward_vector(main_camera);
    // Project forward vector onto the horizontal plane (set y component to 0)
    forward.y = 0.0f;

    glm::vec3 right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 direction = glm::vec3(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction += forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction -= forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction += right;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        direction.y += 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        direction.y -= 1;

    // Normalize the direction vector to prevent diagonal movement from being faster
    if (glm::length(direction) > 0.0f) {
        direction = glm::normalize(direction);
    }

    float speed = static_cast<float>(2.5 * game->delta_time);
    main_camera->position += direction * speed;
}