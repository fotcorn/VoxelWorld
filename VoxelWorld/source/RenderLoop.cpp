#include "RenderLoop.h"

#include <iostream>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>

#include <fmt/format.h>

#include "TextureAtlas.h"
#include "voxel/WorldRenderer.h"

#include "debug_ui/imgui_impl_glfw.h"
#include "debug_ui/imgui_impl_opengl3.h"

static void openglErrorCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/, GLenum severity, GLsizei /*unused*/,
                                const GLchar* message, const void* /*unused*/) {
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

const int INITIAL_WINDOW_WIDTH = 1280;
const int INITIAL_WINDOW_HEIGHT = 800;

const float CAMERA_FOV = 45.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.0f;

const int CAMERA_CHUNK_DISTANCE = 15;

RenderLoop::RenderLoop() {
    this->initGlfw();
    this->initGlew();
    this->initOpenGL();
    this->initGui();
    ui = std::make_shared<UI>(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    this->updateCamera(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    ui->handleKeyUp(Key::ONE);
}

void RenderLoop::initGlfw() {
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
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(this->window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        auto loop = static_cast<RenderLoop*>(glfwGetWindowUserPointer(window));
        loop->updateCamera(width, height);
    });

    glfwSetWindowUserPointer(this->window, static_cast<void*>(this));
}

void RenderLoop::initGlew() {
    glewExperimental = GL_TRUE;
    glewInit();
}

void RenderLoop::initOpenGL() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openglErrorCallback, nullptr);

    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // smaller value is closer
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glViewport(0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
}

void RenderLoop::initGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, false);
    glfwSetMouseButtonCallback(this->window, ImGui_ImplGlfw_MouseButtonCallback);
    glfwSetCursorPosCallback(this->window, [](GLFWwindow* window, double xPosition, double yPosition) {
        auto loop = static_cast<RenderLoop*>(glfwGetWindowUserPointer(window));
        loop->mouseCursorPositionCallback(xPosition, yPosition);
    });
    glfwSetScrollCallback(this->window, [](GLFWwindow* window, double xPosition, double yPosition) {
        ImGui_ImplGlfw_ScrollCallback(window, xPosition, yPosition);
    });
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}

void RenderLoop::updateCamera(int viewportWidth, int viewportHeight) {
    this->projectionMatrix =
        glm::perspective(glm::radians(CAMERA_FOV),
                         static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight), NEAR_PLANE, FAR_PLANE);
    this->projectionMatrix2D =
        glm::ortho(0.0f, static_cast<float>(viewportWidth), 0.0f, static_cast<float>(viewportHeight), -1.0f, 1.0f);
    this->ui->windowChanged(viewportWidth, viewportHeight);
}

void RenderLoop::mainLoop() {
    bool wireframe = false;

    WorldRenderer worldRenderer(CAMERA_CHUNK_DISTANCE);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (currentFrame - lastSimulation >= 1.0f) {
            world.simulationTick();
            lastSimulation = currentFrame;
        }

        // input
        this->handleInput();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);

        // render world
        glm::mat4 vp = this->projectionMatrix * view;
        world.cameraChanged(this->cameraPos, this->cameraFront, CAMERA_CHUNK_DISTANCE);
        worldRenderer.render(world, vp, this->cameraPos, this->cameraFront, wireframe);

        // draw rect
        ui->render();

        // draw ImGui debug GUI
        if (drawGui) {

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::SliderFloat("Camera Speed", &this->cameraSpeed, 0.0f, 10.0f);

            ImGui::Text("%s",
                        fmt::format("Position: X: {:.2f}, Y: {:.2f}, Z: {:.2f}", cameraPos.x, cameraPos.y, cameraPos.z)
                            .c_str());
            ImGui::Text("%s", fmt::format("Front:    X: {:.2f}, Y: {:.2f}, Z: {:.2f}", cameraFront.x, cameraFront.y,
                                          cameraFront.z)
                                  .c_str());
            ImGui::Text("%s", fmt::format("Pitch: {:.2f}, Yaw: {:.2f}", pitch, yaw).c_str());

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

void RenderLoop::handleInput() {
    static bool ctrlDown = false;
    static bool leftMouseDown = false;
    static bool rightMouseDown = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += this->deltaTime * this->cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= this->deltaTime * this->cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (this->cameraSpeed * this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (this->cameraSpeed * this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += glm::vec3(0.0, 1.0f, 0.0) * (this->cameraSpeed * this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        cameraPos -= glm::vec3(0.0, 1.0f, 0.0) * (this->cameraSpeed * this->deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        ui->handleKeyUp(Key::ONE);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        ui->handleKeyUp(Key::TWO);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        ui->handleKeyUp(Key::THREE);
    }

    // handle mouse buttons
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
        leftMouseDown = true;
    } else if (state == GLFW_RELEASE && leftMouseDown) {
        BlockType blockType = ui->selectedBlockType();
        switch (blockType) {
        case BlockType::GRASS:
            world.addBlock(TextureAtlas::GROUND_EARTH);
            break;
        case BlockType::WATER:
            world.addBlock(TextureAtlas::WATER);
            break;
        case BlockType::STONE:
            world.addBlock(TextureAtlas::STONE_04);
            break;
        default:
            break;
        }

        leftMouseDown = false;
    }
    state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS) {
        rightMouseDown = true;
    } else if (state == GLFW_RELEASE && rightMouseDown) {
        world.removeBlock();
        rightMouseDown = false;
    }

    // toggle gui with Ctrl key, you also have to press Alt to have a cursor to interact with the GUI
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        ctrlDown = true;
    } else if (ctrlDown) {
        ctrlDown = false;
        this->drawGui = !drawGui;
    }

    // holding down the left Alt key shows a cursor and allows interaction with the GUI
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        // the Alt key has been pressed the first time in this frame (was not pressed before)
        if (!this->showMouseCursor && this->drawGui) {
            glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->showMouseCursor = true;
        }
        // alt key was pressed before, but is no longer pressed
    } else if (this->showMouseCursor) {
        this->showMouseCursor = false;
        glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        double mouseX;
        double mouseY;
        glfwGetCursorPos(this->window, &mouseX, &mouseY);
        this->lastX = static_cast<float>(mouseX);
        this->lastY = static_cast<float>(mouseY);
    }
}

void RenderLoop::mouseCursorPositionCallback(double xPositionDouble, double yPositionDouble) {
    if (this->showMouseCursor) {
        return;
    }

    float xPosition = static_cast<float>(xPositionDouble);
    float yPosition = static_cast<float>(yPositionDouble);

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
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
    front.y = static_cast<float>(sin(glm::radians(pitch)));
    front.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    cameraFront = glm::normalize(front);
}
