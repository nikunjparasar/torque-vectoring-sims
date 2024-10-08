// main.cpp

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>

// Include stb_easy_font.h for text rendering
// Download from: https://github.com/nothings/stb/blob/master/stb_easy_font.h
#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"

// Constants
const float PI = 3.14159265358979323846f;
const float DEG2RAD = PI / 180.0f;
const float RAD2DEG = 180.0f / PI;
const float GRAVITY = 9.81f;

// Define the initial position and orientation of the car
struct Car {
    // Position and orientation
    float x, y, z;            // Position in world coordinates
    float heading;            // Heading angle (radians)
    float velocity;           // Speed (m/s)
    float acceleration;       // Acceleration along the car's axis (m/s^2)
    float steerAngle;         // Steering angle (radians)
    float yawRate;            // Yaw rate (radians per second)

    // Vehicle parameters
    float mass;               // Mass of the car (kg)
    float length;             // Total length of the car (m)
    float width;              // Width of the car (m)
    float wheelbase;          // Distance between front and rear axles (m)
    float lf;                 // Distance from CG to front axle (m)
    float lr;                 // Distance from CG to rear axle (m)
    float Iz;                 // Yaw moment of inertia (kg·m²)
    float Cf;                 // Cornering stiffness front (N/rad)
    float Cr;                 // Cornering stiffness rear (N/rad)
    float maxSteer;           // Maximum steering angle (radians)
    float maxAcceleration;    // Maximum acceleration (m/s²)
    float maxDeceleration;    // Maximum deceleration (m/s²)
    float h_cg;               // Height of the center of gravity (m)
    float trackWidth;         // Width between left and right wheels (m)
} car = {
    // Initial position and orientation
    0.0f, 0.5f, 0.0f,         // x, y, z
    0.0f,                     // heading
    0.0f,                     // velocity
    0.0f,                     // acceleration
    0.0f,                     // steerAngle
    0.0f,                     // yawRate

    // Vehicle parameters
    1500.0f,                  // mass (kg)
    4.5f,                     // length (m)
    1.8f,                     // width (m)
    2.5f,                     // wheelbase (m)
    1.25f,                    // lf (m)
    1.25f,                    // lr (m)
    2250.0f,                  // Iz (kg·m²)
    80000.0f,                 // Cf (N/rad)
    80000.0f,                 // Cr (N/rad)
    30.0f * DEG2RAD,          // maxSteer (radians)
    5.0f,                     // maxAcceleration (m/s²)
    -10.0f,                   // maxDeceleration (m/s²)
    0.55f,                    // h_cg (m)
    1.6f                      // trackWidth (m)
};

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Function to process input
void processInput(GLFWwindow* window) {
    // Close window on ESC
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Handle steering
    float steerInput = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        steerInput = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        steerInput = -1.0f;
    }

    // Update steering angle
    float steerSpeed = 1.5f; // Steering speed (radians per second)
    car.steerAngle += steerSpeed * steerInput * deltaTime;
    if (car.steerAngle > car.maxSteer)
        car.steerAngle = car.maxSteer;
    if (car.steerAngle < -car.maxSteer)
        car.steerAngle = -car.maxSteer;

    // Handle acceleration and braking
    float accelerationInput = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        accelerationInput = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        accelerationInput = -1.0f;
    }

    // Update acceleration
    if (accelerationInput > 0.0f) {
        car.acceleration = car.maxAcceleration * accelerationInput;
    } else if (accelerationInput < 0.0f) {
        car.acceleration = -car.maxDeceleration * accelerationInput;
    } else {
        // Apply rolling resistance and aerodynamic drag when no input
        float rollingResistance = -0.015f * car.velocity;
        float aerodynamicDrag = -0.001f * car.velocity * fabsf(car.velocity);
        car.acceleration = rollingResistance + aerodynamicDrag;
    }
}

// Function to set up basic lighting
void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = { 5.0f, 5.0f, 5.0f, 1.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

// Function to draw a simple cube centered at the origin
void drawCube() {
    glBegin(GL_QUADS);

    // Front face (z = 1.0f)
    glColor3f(0.8f, 0.0f, 0.0f);     // Dark Red
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(0.5f, -0.5f,  0.5f);
    glVertex3f(0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);

    // Back face (z = -1.0f)
    glColor3f(0.8f, 0.0f, 0.0f);     // Dark Red
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(0.5f,  0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Left face (x = -1.0f)
    glColor3f(0.8f, 0.0f, 0.0f);     // Dark Red
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);

    // Right face (x = 1.0f)
    glColor3f(0.8f, 0.0f, 0.0f);     // Dark Red
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f,  0.5f, -0.5f);
    glVertex3f(0.5f,  0.5f,  0.5f);
    glVertex3f(0.5f, -0.5f,  0.5f);

    // Top face (y = 1.0f)
    glColor3f(0.9f, 0.1f, 0.1f);     // Lighter Red
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f,  0.5f);
    glVertex3f(0.5f, 0.5f,  0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Bottom face (y = -1.0f)
    glColor3f(0.6f, 0.0f, 0.0f);     // Darker Red
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);

    glEnd();
}

// Function to draw an arrow representing normal load
void drawArrow(float x, float y, float z, float load) {
    float scale = 0.0005f;
    float arrowHeight = load * scale;

    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 1.0f); // Cyan color for load arrows
    glVertex3f(x, y, z);
    glVertex3f(x, y + arrowHeight, z);
    glEnd();

    // Arrowhead
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 1.0f); // Same color
    glVertex3f(x - 0.05f, y + arrowHeight, z - 0.05f);
    glVertex3f(x + 0.05f, y + arrowHeight, z - 0.05f);
    glVertex3f(x, y + arrowHeight + 0.1f, z);
    glEnd();
}

// Function to draw normal load arrows at front and rear axles
void drawNormalLoadArrows(float frontLoad, float rearLoad) {
    // Positions of the axles relative to the car's center
    float frontZ = car.lf;
    float rearZ = -car.lr;
    float centerX = 0.0f;

    // Front Axle
    drawArrow(centerX, 0.5f, frontZ, frontLoad);

    // Rear Axle
    drawArrow(centerX, 0.5f, rearZ, rearLoad);
}

// Function to render text on the screen
void renderText(float x, float y, const char* text) {
    char buffer[99999]; // ~500 chars
    int num_quads;

    num_quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, sizeof(buffer));

    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1); // White color
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
}

// Main function
int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Remove OpenGL version hints to use default (compatibility profile)
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "3D Car Controller with Realistic Physics", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW (optional for modern OpenGL)
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    // Output OpenGL version
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Set a clear color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Set up basic lighting
    setupLighting();

    // Set up 2D orthographic projection for text rendering
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Update vehicle dynamics using the bicycle model
        float beta = 0.0f; // Slip angle at vehicle center of gravity
        if (fabsf(car.velocity) > 0.1f) {
            beta = atan2f((car.lr * tanf(car.steerAngle)) / (car.lf + car.lr), 1.0f);
        }

        // Lateral acceleration
        float a_lat = (car.velocity * car.velocity * tanf(car.steerAngle)) / car.wheelbase;

        // Longitudinal acceleration is car.acceleration

        // Calculate load transfers
        // Static normal loads
        float Fz_front_static = (car.lr / car.wheelbase) * car.mass * GRAVITY;
        float Fz_rear_static = (car.lf / car.wheelbase) * car.mass * GRAVITY;

        // Longitudinal load transfer
        float deltaFz_long = (car.h_cg / car.wheelbase) * car.mass * car.acceleration;

        // Lateral load transfer (assuming it equally affects front and rear axles)
        float deltaFz_lat = (car.h_cg / car.trackWidth) * car.mass * a_lat;

        // Total normal loads
        float Fz_front = Fz_front_static - deltaFz_long - deltaFz_lat / 2.0f;
        float Fz_rear = Fz_rear_static + deltaFz_long - deltaFz_lat / 2.0f;

        // Update position and heading
        float velocityX = car.velocity * cosf(car.heading + beta);
        float velocityZ = car.velocity * sinf(car.heading + beta);

        car.x += velocityX * deltaTime;
        car.z += velocityZ * deltaTime;

        car.heading += (car.velocity / car.wheelbase) * tanf(car.steerAngle) * deltaTime;

        // Update velocity
        car.velocity += car.acceleration * deltaTime;

        // Limit speed
        if (car.velocity > 55.0f)
            car.velocity = 55.0f;
        if (car.velocity < -20.0f)
            car.velocity = -20.0f;

        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the camera (following the car from behind)
        glm::vec3 eyePos = glm::vec3(car.x - 8.0f * cosf(car.heading), 5.0f, car.z - 8.0f * sinf(car.heading));
        glm::vec3 centerPos = glm::vec3(car.x, car.y, car.z);
        glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(eyePos, centerPos, upVec);

        // Set up the projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.1f, 1000.0f);

        // Load the projection and view matrices
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLoadMatrixf(&projection[0][0]);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(&view[0][0]);

        // Draw the ground plane (a large grid)
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_LINES);
        for (int i = -100; i <= 100; i++) {
            glVertex3f((float)i, 0.0f, -100.0f);
            glVertex3f((float)i, 0.0f,  100.0f);
            glVertex3f(-100.0f, 0.0f, (float)i);
            glVertex3f( 100.0f, 0.0f, (float)i);
        }
        glEnd();

        // Apply car transformations
        glPushMatrix();
        glTranslatef(car.x, car.y, car.z);
        glRotatef(car.heading * RAD2DEG, 0.0f, 1.0f, 0.0f);

        // Draw the car
        glScalef(car.length, 1.0f, car.width); // Scale the cube to represent a car
        drawCube();

        glPopMatrix();

        // Draw normal load arrows at front and rear axles
        glPushMatrix();
        glTranslatef(car.x, car.y, car.z);
        glRotatef(car.heading * RAD2DEG, 0.0f, 1.0f, 0.0f);
        drawNormalLoadArrows(Fz_front, Fz_rear);
        glPopMatrix();

        // Render text (switch to orthographic projection)
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Disable lighting for text rendering
        glDisable(GL_LIGHTING);

        // Prepare text content
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Speed: " << car.velocity << " m/s\n";
        ss << "Acceleration: " << car.acceleration << " m/s^2\n";
        ss << "Steering Angle: " << car.steerAngle * RAD2DEG << " degrees\n";
        ss << "Heading: " << car.heading * RAD2DEG << " degrees\n";
        ss << "Front Normal Load: " << Fz_front << " N\n";
        ss << "Rear Normal Load: " << Fz_rear << " N\n";

        // Render text
        renderText(10.0f, 20.0f, ss.str().c_str());

        // Re-enable lighting
        glEnable(GL_LIGHTING);

        // Restore previous projection and modelview matrices
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();

        // Update window size (in case of window resize)
        glfwGetWindowSize(window, &width, &height);
    }

    glfwTerminate();
    return 0;
}
