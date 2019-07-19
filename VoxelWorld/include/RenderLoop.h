#ifndef RENDER_LOOP_H
#define RENDER_LOOP_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>

#include "Mesh.h"
#include "Object.h"
#include "ShaderProgram.h"

class RenderLoop {
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
    float cameraSpeed = 0.5f;

    glm::vec3 cameraPos = glm::vec3(85.4998f, 25.5396f, 61.9887f);
    glm::vec3 cameraFront = glm::vec3(0.794214f, -0.316475f, 0.518718f);

    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
    float cameraDistance = 5.0f;
    bool firstMouse = true;
    float yaw = 33.1494f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector
                          // pointing to the right so we initially rotate a bit to the left.
    float pitch = -18.4499f;
    float lastX = 0.0f;
    float lastY = 0.0f;
};

#endif // !RENDER_LOOP_H
