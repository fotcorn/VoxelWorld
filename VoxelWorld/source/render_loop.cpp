#include "render_loop.h"

#include <iostream>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>

#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_opengl3.h"

#include "shader_program.h"

static void openglErrorCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/, GLenum severity, GLsizei /*unused*/,
                                const GLchar* message, const void* /*unused*/) {
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 800;

void Program::init() {
    this->initGlfw();
    this->initGlew();
    this->initOpenGL();
    this->initGui();
    this->initCube();
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

    this->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", nullptr, nullptr);

    if (!this->window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(this->window);

    glfwSetFramebufferSizeCallback(
        this->window, [](GLFWwindow* /*unused*/, int width, int height) { glViewport(0, 0, width, height); });

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
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
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

void Program::initCube() {
    this->cube = std::make_shared<Object>(Object(
        {
            // vertices
            // front
            glm::vec3(-1.0, -1.0, 1.0),
            glm::vec3(1.0, -1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(-1.0, 1.0, 1.0),
            // back
            glm::vec3(-1.0, -1.0, -1.0),
            glm::vec3(1.0, -1.0, -1.0),
            glm::vec3(1.0, 1.0, -1.0),
            glm::vec3(-1.0, 1.0, -1.0),
        },
        {
            // indices
            // front
            glm::vec3(0, 1, 2),
            glm::vec3(2, 3, 0),
            // right
            glm::vec3(1, 5, 6),
            glm::vec3(6, 2, 1),
            // back
            glm::vec3(7, 6, 5),
            glm::vec3(5, 4, 7),
            // left
            glm::vec3(4, 0, 3),
            glm::vec3(3, 7, 4),
            // bottom
            glm::vec3(4, 5, 1),
            glm::vec3(1, 0, 4),
            // top
            glm::vec3(3, 2, 6),
            glm::vec3(6, 7, 3),
        },
        {
            // colors
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(1.0, 1.0, 1.0),
        }));
    Shader fragmentShader = Shader::loadFromFile("resources/shaders/glsl/cube.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("resources/shaders/glsl/cube.vert", Shader::Type::Vertex);
    this->cubeShaderProgram = std::make_shared<ShaderProgram>();
    this->cubeShaderProgram->attachShader(vertexShader);
    this->cubeShaderProgram->attachShader(fragmentShader);
    this->cubeShaderProgram->setAttribLocation("vertex_position", 0);
    // this->cubeShaderProgram->setAttribLocation("vertex_color", 1);
    this->cubeShaderProgram->link();
}

void Program::initCamera() {
    this->projectionMatrix = glm::perspective(glm::radians(45.0f), 1024.0f / 800.0f, 0.1f, 100.0f);
}

void Program::mainLoop() {
    bool wireframe = false;

    glm::vec3 cameraPosition = glm::vec3(-7.0, 2.5, 0.0);
    glm::vec3 cubePosition = glm::vec3(-15.0, 15.0, 5.0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        this->handleInput();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 eye = glm::vec3(1.0, 1.0, 1.0);
        glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
        glm::mat4 view = glm::lookAt(eye, cameraPosition, up);

        // draw cube
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::scale(cubeModel, glm::vec3(0.1, 0.1, 0.1));
        cubeModel = glm::translate(cubeModel, cubePosition);
        auto mvp = this->projectionMatrix * view * cubeModel;
        this->cubeShaderProgram->use();
        this->cubeShaderProgram->setUniform("mvp", mvp);
        this->cube->draw(wireframe);

        if (drawGui) {
            // draw gui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::SliderFloat("Camera X", &cameraPosition.x, -10.0f, 10.0f);
            ImGui::SliderFloat("Camera Y", &cameraPosition.y, -10.0f, 10.0f);
            ImGui::SliderFloat("Camera Z", &cameraPosition.z, -10.0f, 10.0f);

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

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // this->spaceShipRotation =
        //     glm::rotate(this->spaceShipRotation, glm::radians(-40.0f * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // this->spaceShipRotation =
        //     glm::rotate(this->spaceShipRotation, glm::radians(40.0f * deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // this->spaceShipRotation =
        //     glm::rotate(this->spaceShipRotation, glm::radians(40.0f * deltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // this->spaceShipRotation =
        //     glm::rotate(this->spaceShipRotation, glm::radians(-40.0f * deltaTime), glm::vec3(1.0f, 0.0f, 0.0f));
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
    float yoffset = yPosition - lastY; // reversed since y-coordinates go from bottom to top
    lastX = xPosition;
    lastY = yPosition;

    float sensitivity = 0.1f; // change this value to your liking
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
