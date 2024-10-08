#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
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
    0.0f, 0.5f, 0.0f,          // x, y, z
    0.0f,                       // rotation
    0.0f,                       // velocity
    0.0f,                       // acceleration
    1500.0f,                    // mass
    2.5f,                       // wheelbase
    1.6f,                       // trackWidth
    0.5f,                       // COMHeight
    150000.0f                   // rollStiffness
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

// Function to render text on the screen
void renderText(float x, float y, void *font, const char* string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string);
        ++string;
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Set OpenGL version (optional, depends on your system)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

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

    // Initialize GLUT for text rendering
    int argc = 0;
    char *argv[] = { (char*)"" };
    glutInit(&argc, argv);

    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1280.0 / 720.0, 0.1, 1000.0);

    // Switch back to modelview matrix
    glMatrixMode(GL_MODELVIEW);

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
        float deltaFz = (car.mass * a_lat * car.COMHeight) / car.trackWidth; // Total lateral load transfer

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
        glLoadIdentity();

        // Set up the camera (following the car from behind)
        gluLookAt(
            car.x - 10.0f * sinf(rad), 5.0f, car.z - 10.0f * cosf(rad), // Eye position
            car.x, car.y, car.z,                                         // Look at car's position
            0.0, 1.0, 0.0                                                // Up vector
        );

        // Draw the ground plane (a large grid)
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_LINES);
        for (int i = -100; i <= 100; i++) {
            glVertex3f(i, 0, -100);
            glVertex3f(i, 0,  100);
            glVertex3f(-100, 0, i);
            glVertex3f( 100, 0, i);
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

        // Draw the direction arrow
        glScalef(0.5f, 0.5f, 0.5f); // Adjust scale for the arrow
        drawDirectionArrow();

        // Draw normal load arrows at each tire
        glScalef(2.0f, 2.0f, 2.0f); // Reset scale
        drawNormalLoadArrows(frontLeftLoad, frontRightLoad, rearLeftLoad, rearRightLoad,
                             car.wheelbase, car.trackWidth);

        glPopMatrix();

        // Set up orthographic projection for 2D text rendering
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 1280, 0, 720);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Disable depth test and lighting for text rendering
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        // Render text
        glColor3f(1.0f, 1.0f, 1.0f);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Speed: " << car.velocity << " m/s";
        renderText(10, 700, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        ss.str("");
        ss << "Acceleration: " << car.acceleration << " m/s^2";
        renderText(10, 680, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        ss.str("");
        ss << "Lateral Acceleration: " << a_lat << " m/s^2";
        renderText(10, 660, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        ss.str("");
        ss << "Steering Angle: " << steeringAngle * (180.0f / PI) << " degrees";
        renderText(10, 640, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        ss.str("");
        ss << "Roll Angle: " << rollAngle * (180.0f / PI) << " degrees";
        renderText(10, 620, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        ss.str("");
        ss << "Front Left Load: " << frontLeftLoad << " N";
        renderText(10, 600, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        ss.str("");
        ss << "Front Right Load: " << frontRightLoad << " N";
        renderText(10, 580, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        ss.str("");
        ss << "Rear Left Load: " << rearLeftLoad << " N";
        renderText(10, 560, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        ss.str("");
        ss << "Rear Right Load: " << rearRightLoad << " N";
        renderText(10, 540, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        // Restore projection and modelview matrices
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}