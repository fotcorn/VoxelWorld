#ifndef RENDER_LOOP_H
#define RENDER_LOOP_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>

#include "Mesh.h"
#include "ShaderProgram.h"
#include "ui/UI.h"
#include "voxel/World.h"

class RenderLoop {
public:
    RenderLoop();
    void mainLoop();

private:
    // methods
    void initGlfw();
    void initGlew();
    void initOpenGL();
    void initGui();
    void updateCamera(int viewportWidth, int viewportHeight);

    void handleInput();

    void mouseCursorPositionCallback(double xPosition, double yPosition);
    void mouseScrollCallback(double xOffset, double yOffset);

    World world;

    // members
    GLFWwindow* window = nullptr;

    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix2D = glm::mat4(1.0f);

    bool drawGui = false;
    bool showMouseCursor = false;

    // timing
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    float lastSimulation = 0.0f;

    // camera
    float cameraSpeed = 2.0f;

    glm::vec3 cameraPos = glm::vec3(60.0f, 10.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);

    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    bool firstMouse = true;

    float yaw = 0.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector
                      // pointing to the right so we initially rotate a bit to the left.
    float pitch = -90.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;

    std::shared_ptr<UI> ui;
};

#endif // !RENDER_LOOP_H
