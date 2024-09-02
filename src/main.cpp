#include <iostream>
#include <string>
#include <vector>

#include "renderer.hpp"

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "window.hpp"

window_t *main_window;

/*

    Drawing triangle

*/

std::vector<node*> game_objects;

/*

    Camera & input

*/

#include "controller.hpp"

camera_t *main_camera;
mouse_t *mouse;
game_t *main_game;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    mouse_callback_c(window, xposIn, yposIn, mouse, main_camera);
}

void ready() {
    init();
    main_window = create_window();
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, 1280, 720);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSwapInterval(0);

    initialize_controller(main_window->window, mouse_callback);


    game_objects.push_back(create_object(root_path + "/art/models/ferrari_550/scene.fbx"));
    game_objects.push_back(create_object(root_path + "/art/models/arm/Arms_M416_Assault_Rifle.fbx"));
    

    main_camera = new camera_t;
    main_camera->yaw = -90.0f;
    main_camera->pitch = 0.0f;

    main_camera->position = glm::vec3(0.0f, 0.0f, 3.0f);

    mouse = new mouse_t;
    mouse->last_x = 1280.0 / 2.0f;
    mouse->last_y = 720.0 / 2.0f;
    mouse->first_mouse = true;

    main_game = new game_t;
    main_game->delta_time = 0.0f;
    main_game->last_frame = 0.0f;
}

void update() {
    process_input(main_window->window, main_game, main_camera);
    update_input(static_cast<float>(glfwGetTime()), main_game);

    glm::mat4 objectTransform = glm::mat4(1.0f); // Start with an identity matrix

    // Get the camera's forward vector and position
    glm::vec3 forwardVector = get_camera_forward_vector(main_camera);
    glm::vec3 cameraPosition = main_camera->position;

    // Offset distance in front of the camera
    float offsetDistance = 0.5f; // Adjust this as needed

    // 1. Translate the object to the origin (camera position)
    objectTransform = glm::translate(objectTransform, cameraPosition);
    objectTransform = glm::translate(objectTransform, forwardVector * offsetDistance);

    objectTransform = glm::rotate(objectTransform, glm::radians(90.0f), glm::vec3(0, 1, 0));

    // 2. Apply rotations: yaw (Y-axis) and then pitch (X-axis)
    objectTransform = glm::rotate(objectTransform, glm::radians(-main_camera->yaw), glm::vec3(0, 1, 0));
    objectTransform = glm::rotate(objectTransform, glm::radians(-main_camera->pitch - 90), glm::vec3(1, 0, 0));
    objectTransform = glm::translate(objectTransform, glm::vec3(-0.2f, 0.0f, -0.2f)); // offset

    // Set the object's transformation matrix
    game_objects[1]->set_global_transformation(objectTransform);
}


void render() {
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(main_camera->position, 
                                 main_camera->position + get_camera_forward_vector(main_camera), 
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    for (node *object : game_objects) {
        render_object(object, view, projection);
    }
}

void clean() {
    destroy_glfw();
}

int main() {
    ready();

    while (!main_window->should_close) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        update();
        render();
        update_window(main_window);
    }

    clean();

    return 0;
}