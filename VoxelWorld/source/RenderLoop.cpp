#include "RenderLoop.h"

#include <iostream>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>

#include <fmt/format.h>

#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_opengl3.h"

#include "VoxelWorld.h"

static void openglErrorCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/, GLenum severity, GLsizei /*unused*/,
                                const GLchar* message, const void* /*unused*/) {
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

const int INITIAL_WINDOW_WIDTH = 1280;
const int INITIAL_WINDOW_HEIGHT = 800;

const float CAMERA_FOV = 45.0;
const float NEAR_PLANE = 0.1;
const float FAR_PLANE = 100.0;

void Program::init() {
    this->initGlfw();
    this->initGlew();
    this->initOpenGL();
    this->initGui();
    this->initCamera();
}

void Program::initGlfw() {
    glfwSetErrorCallback(
        [](int /*unused*/, const char* message) { std::cerr << "GLFW error:" << message << std::endl; });

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW3");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    this->window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Voxel World", nullptr, nullptr);

    if (!this->window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(this->window);

    glfwSetFramebufferSizeCallback(this->window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Program* program = (Program*)glfwGetWindowUserPointer(window);
        program->projectionMatrix =
            glm::perspective(glm::radians(CAMERA_FOV), (float)width / (float)height, NEAR_PLANE, FAR_PLANE);
    });

    glfwSetWindowUserPointer(this->window, (void*)this);
}

void Program::initGlew() {
    glewExperimental = GL_TRUE;
    glewInit();
}

void Program::initOpenGL() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openglErrorCallback, nullptr);

    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // smaller value is closer
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glViewport(0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
}

void Program::initGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, false);
    glfwSetMouseButtonCallback(this->window, ImGui_ImplGlfw_MouseButtonCallback);
    glfwSetCursorPosCallback(this->window, [](GLFWwindow* window, double xPosition, double yPosition) {
        Program* program = (Program*)glfwGetWindowUserPointer(window);
        program->mouseCursorPositionCallback(xPosition, yPosition);
    });
    glfwSetScrollCallback(this->window, [](GLFWwindow* window, double xPosition, double yPosition) {
        Program* program = (Program*)glfwGetWindowUserPointer(window);
        program->mouseScrollCallback(xPosition, yPosition);
    });
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}

void Program::initCamera() {
    this->projectionMatrix = glm::perspective(
        glm::radians(CAMERA_FOV), (float)INITIAL_WINDOW_WIDTH / (float)INITIAL_WINDOW_HEIGHT, NEAR_PLANE, FAR_PLANE);
}

void Program::mainLoop() {
    bool wireframe = false;

    VoxelWorld world;
    world.init();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        std::cout << 1.0f / deltaTime << std::endl;

        this->handleInput();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);

        // draw cube
        glm::mat4 vp = this->projectionMatrix * view;
        world.render(vp, this->cameraPos, wireframe);

        if (drawGui) {
            // draw gui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::SliderFloat("Camera Speed", &this->cameraSpeed, 0.0f, 10.0f);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

void Program::handleInput() {
    static bool ctrlDown = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += this->cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= this->cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * this->cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * this->cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += glm::vec3(0.0, 1.0f, 0.0) * this->cameraSpeed;
        // fmt::print("{} {}\n", yaw, pitch);
        // fmt::print("{}f, {}f, {}f\n", cameraPos.x, cameraPos.y, cameraPos.z);
        // fmt::print("{}f, {}f, {}f\n", cameraFront.x, cameraFront.y, cameraFront.z);
        // fmt::print("-----\n");
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        ctrlDown = true;
    } else if (ctrlDown) {
        ctrlDown = false;
        this->drawGui = !drawGui;
        if (this->drawGui) {
            glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            double mouseX, mouseY;
            glfwGetCursorPos(this->window, &mouseX, &mouseY);
            this->lastX = mouseX;
            this->lastY = mouseY;
        }
    }
}

void Program::mouseCursorPositionCallback(double xPosition, double yPosition) {
    if (this->drawGui) {
        return;
    }

    if (firstMouse) {
        lastX = xPosition;
        lastY = yPosition;
        firstMouse = false;
    }

    float xoffset = xPosition - lastX;
    float yoffset = lastY - yPosition; // reversed since y-coordinates go from bottom to top
    lastX = xPosition;
    lastY = yPosition;

    float sensitivity = 0.05f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void Program::mouseScrollCallback(double xOffset, double yOffset) {
    if (this->drawGui) {
        ImGui_ImplGlfw_ScrollCallback(this->window, xOffset, yOffset);
    } else {
        cameraDistance -= yOffset;
        if (cameraDistance < 2) {
            cameraDistance = 1;
        }
    }
}
