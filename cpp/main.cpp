#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>

// Constants
const float PI = 3.14159265358979323846f;
const float GRAVITY = 9.81f;

// Define the initial position and orientation of the car
struct Car {
    float x, y, z;            // Position
    float rotation;           // Yaw rotation in degrees
    float velocity;           // Current speed of the car (m/s)
    float acceleration;       // Current acceleration (m/s^2)

    // Vehicle parameters
    float mass;               // Mass of the car (kg)
    float wheelbase;          // Distance between front and rear axles (m)
    float trackWidth;         // Distance between left and right wheels (m)
    float COMHeight;          // Height of the center of mass (m)
    float rollStiffness;      // Roll stiffness (Nm/rad)
} car = {
    0.0f, 0.5f, 0.0f,          // x, y, z (Adjusted y to 0.5f)
    0.0f,                      // rotation
    0.0f,                      // velocity
    0.0f,                      // acceleration
    1500.0f,                   // mass
    2.5f,                      // wheelbase
    1.6f,                      // trackWidth
    0.5f,                      // COMHeight
    150000.0f                  // rollStiffness
};

// Movement parameters
const float MAX_SPEED = 30.0f;          // Maximum speed units per second
const float ACCELERATION_RATE = 5.0f;   // Acceleration units per second squared
const float BRAKE_RATE = 10.0f;         // Braking units per second squared
const float ROTATE_SPEED = 60.0f;       // Degrees per second

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Variables for lateral dynamics
float steeringAngle = 0.0f; // Steering angle in radians

// Function to process input
void processInput(GLFWwindow *window) {
    // Close window on ESC
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Handle acceleration and braking
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        car.acceleration = ACCELERATION_RATE;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        car.acceleration = -BRAKE_RATE;
    }
    else {
        // Apply friction when no acceleration/braking
        if (car.velocity > 0.0f) {
            car.acceleration = -5.0f; // Friction deceleration
            if (car.velocity + car.acceleration * deltaTime < 0.0f)
                car.acceleration = -car.velocity / deltaTime;
        }
        else if (car.velocity < 0.0f) {
            car.acceleration = 5.0f; // Friction deceleration
            if (car.velocity + car.acceleration * deltaTime > 0.0f)
                car.acceleration = -car.velocity / deltaTime;
        }
        else {
            car.acceleration = 0.0f;
        }
    }

    // Update velocity with acceleration
    car.velocity += car.acceleration * deltaTime;
    // Clamp velocity to max speed
    if (car.velocity > MAX_SPEED)
        car.velocity = MAX_SPEED;
    if (car.velocity < -MAX_SPEED / 2) // Reverse speed is typically lower
        car.velocity = -MAX_SPEED / 2;

    // Handle rotation only when the car is moving
    if (fabs(car.velocity) > 0.1f) { // Threshold to prevent jitter
        // Calculate steering angle
        steeringAngle = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            steeringAngle = 15.0f * (PI / 180.0f); // 15 degrees to radians
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            steeringAngle = -15.0f * (PI / 180.0f); // -15 degrees to radians
        }

        // Update car rotation based on steering and velocity
        float angularVelocity = (car.velocity / car.wheelbase) * tan(steeringAngle);
        car.rotation += angularVelocity * deltaTime * (180.0f / PI); // Convert to degrees
        if (car.rotation >= 360.0f) car.rotation -= 360.0f;
        if (car.rotation < 0.0f) car.rotation += 360.0f;
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
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);

    // Back face (z = -1.0f)
    glColor3f(0.8f, 0.0f, 0.0f);     // Dark Red
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);

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
    glVertex3f( 0.5f, 0.5f,  0.5f);
    glVertex3f( 0.5f, 0.5f, -0.5f);

    // Bottom face (y = -1.0f)
    glColor3f(0.6f, 0.0f, 0.0f);     // Darker Red
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);

    glEnd();
}

// Function to draw an arrow indicating movement direction
void drawDirectionArrow() {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for the arrow

    // Arrow base (from the center front)
    glVertex3f(0.0f, 0.5f, 2.0f); // Starting point at front center of the car
    glVertex3f(0.0f, 0.5f, 3.5f); // End point forward

    // Arrowhead
    glVertex3f(0.0f, 0.5f, 3.5f);
    glVertex3f(-0.2f, 0.5f, 3.3f);

    glVertex3f(0.0f, 0.5f, 3.5f);
    glVertex3f(0.2f, 0.5f, 3.3f);

    glEnd();
}

// Function to draw normal load arrows at each tire
void drawArrow(float x, float y, float z, float load) {
    float scale = 0.0005f;
    float arrowHeight = load * scale;
    glLineWidth(5.0f);
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

void drawNormalLoadArrows(float frontLeftLoad, float frontRightLoad, float rearLeftLoad, float rearRightLoad,
                          float wheelbase, float trackWidth) {
    // Positions of the tires relative to the car's center
    float frontZ = wheelbase / 2.0f;
    float rearZ = -wheelbase / 2.0f;
    float leftX = trackWidth / 2.0f;
    float rightX = -trackWidth / 2.0f;

    // Front-Left Tire
    drawArrow(leftX, 0.5f, frontZ, frontLeftLoad);

    // Front-Right Tire
    drawArrow(rightX, 0.5f, frontZ, frontRightLoad);

    // Rear-Left Tire
    drawArrow(leftX, 0.5f, rearZ, rearLeftLoad);

    // Rear-Right Tire
    drawArrow(rightX, 0.5f, rearZ, rearRightLoad);
}

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

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Update car position based on velocity and direction
        float rad = car.rotation * PI / 180.0f;
        car.x += car.velocity * deltaTime * sinf(rad);
        car.z += car.velocity * deltaTime * cosf(rad); // Forward along positive Z

        // Calculate lateral acceleration
        float a_lat = car.velocity * car.velocity * tan(steeringAngle) / car.wheelbase;

        // Calculate load transfer due to lateral acceleration
        float deltaFz = (car.mass * a_lat * car.COMHeight) / car.trackWidth;

        // Roll angle calculation
        float rollAngle = (car.mass * a_lat * car.COMHeight) / car.rollStiffness; // Roll angle in radians

        // Static normal load per tire (assuming equal weight distribution)
        float normalLoadPerTire = (car.mass * GRAVITY) / 4.0f;

        // Adjust normal loads on each tire due to lateral load transfer
        float frontLeftLoad = normalLoadPerTire - deltaFz / 2.0f;
        float frontRightLoad = normalLoadPerTire + deltaFz / 2.0f;
        float rearLeftLoad = normalLoadPerTire - deltaFz / 2.0f;
        float rearRightLoad = normalLoadPerTire + deltaFz / 2.0f;

        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the camera (following the car from behind)
        glm::vec3 eyePos = glm::vec3(car.x - 8.0f * sinf(rad), 5.0f, car.z - 8.0f * cosf(rad));
        glm::vec3 centerPos = glm::vec3(car.x, car.y, car.z);
        glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(eyePos, centerPos, upVec);

        // Set up the projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

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
        glRotatef(car.rotation, 0.0f, 1.0f, 0.0f);

        // Apply roll rotation around the car's longitudinal axis
        glRotatef(rollAngle * (180.0f / PI), 0.0f, 0.0f, 1.0f);

        // Draw the car
        glScalef(2.0f, 1.0f, 4.0f); // Scale the cube to represent a car
        drawCube();

        glPopMatrix();

        // Draw the direction arrow
        glPushMatrix();
        glTranslatef(car.x, car.y, car.z);
        glRotatef(car.rotation, 0.0f, 1.0f, 0.0f);
        glScalef(0.5f, 0.5f, 0.5f); // Adjust scale for the arrow
        drawDirectionArrow();
        glPopMatrix();

        // Draw normal load arrows at each tire
        glPushMatrix();
        glTranslatef(car.x, car.y, car.z);
        glRotatef(car.rotation, 0.0f, 1.0f, 0.0f);
        drawNormalLoadArrows(frontLeftLoad, frontRightLoad, rearLeftLoad, rearRightLoad,
                             car.wheelbase, car.trackWidth);
        glPopMatrix();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
