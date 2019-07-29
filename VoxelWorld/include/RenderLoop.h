#ifndef RENDER_LOOP_H
#define RENDER_LOOP_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
    float cameraSpeed = 0.05f;

    glm::vec3 cameraPos = glm::vec3(1.0f, 2.5f, 1.0f);
    glm::vec3 cameraFront = glm::vec3(-0.792112f, -0.235989f, -0.56291f);

    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
    float cameraDistance = 5.0f;
    bool firstMouse = true;
    float yaw = 215.399f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector
                          // pointing to the right so we initially rotate a bit to the left.
    float pitch = -13.6499;
    float lastX = 0.0f;
    float lastY = 0.0f;
};

#endif // !RENDER_LOOP_H
