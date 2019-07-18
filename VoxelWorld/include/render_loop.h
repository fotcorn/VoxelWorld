#ifndef RENDER_LOOP_H
#define RENDER_LOOP_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>

#include "mesh.h"
#include "object.h"
#include "shader_program.h"

class Program {
public:
    void init();
    void mainLoop();

private:
    // methods
    void initGlfw();
    void initGlew();
    void initOpenGL();
    void initGui();
    void initCamera();

    void handleInput();

    void mouseCursorPositionCallback(double xPosition, double yPosition);
    void mouseScrollCallback(double xOffset, double yOffset);

    // members
    GLFWwindow* window = nullptr;

    glm::mat4 projectionMatrix = glm::mat4(1.0f);

    bool drawGui = false;

    // timing
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    // camera
    float cameraSpeed = 0.1f;

    glm::vec3 cameraPos = glm::vec3(4.83289f, 4.07179f, 4.32109f);
    glm::vec3 cameraFront = glm::vec3(-0.476543f, -0.543907f, -0.690704f);

    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
    float cameraDistance = 5.0f;
    bool firstMouse = true;
    float yaw = 235.397f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector
                          // pointing to the right so we initially rotate a bit to the left.
    float pitch = -32.95f;
    float lastX = 0.0f;
    float lastY = 0.0f;
};

#endif // !RENDER_LOOP_H
