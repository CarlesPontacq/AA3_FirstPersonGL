#include "camera.h"
#include <GL/freeglut.h>
#include <cmath>

#define PI 3.14159265f

void Camera::init(int screenWidth, int screenHeight) {
    posX = 0.0f; posY = CAMERA_HEIGHT; posZ = 5.0f;
    yaw = -90.0f; pitch = 0.0f;
    centerX = screenWidth / 2;
    centerY = screenHeight / 2;
}

void Camera::apply() {
    float radYaw = yaw * PI / 180.0f;
    float radPitch = pitch * PI / 180.0f;

    float dirX = cos(radYaw) * cos(radPitch);
    float dirY = sin(radPitch);
    float dirZ = sin(radYaw) * cos(radPitch);

    gluLookAt(posX, CAMERA_HEIGHT, posZ,
        posX + dirX, CAMERA_HEIGHT + dirY, posZ + dirZ,
        0.0f, 1.0f, 0.0f);
}

void Camera::updateFlashlight(bool enabled) {
    if (enabled) {
        glEnable(GL_LIGHT1);

        float radYaw = yaw * PI / 180.0f;
        float radPitch = pitch * PI / 180.0f;

        float dirX = cos(radYaw) * cos(radPitch);
        float dirY = sin(radPitch);
        float dirZ = sin(radYaw) * cos(radPitch);

        GLfloat lightPos[] = { posX, CAMERA_HEIGHT, posZ, 1.0f };
        GLfloat lightDir[] = { dirX, dirY, dirZ };

        glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDir);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0f);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 15.0f);
        GLfloat diffuse[] = { 1.0f, 1.0f, 0.8f, 1.0f };
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    }
    else {
        glDisable(GL_LIGHT1);
    }
}

void Camera::moveForward(float delta) {
    float radYaw = yaw * PI / 180.0f;
    float radPitch = pitch * PI / 180.0f;

    posX += cos(radYaw) * cos(radPitch) * delta;
    posZ += sin(radYaw) * cos(radPitch) * delta;
    posY = CAMERA_HEIGHT;
}

void Camera::strafe(float delta) {
    float radYaw = (yaw + 90.0f) * PI / 180.0f;
    posX += cos(radYaw) * delta;
    posZ += sin(radYaw) * delta;
    posY = CAMERA_HEIGHT;
}

void Camera::mouseLook(int x, int y) {
    float sensitivity = 0.2f;
    yaw += (x - centerX) * sensitivity;
    pitch -= (y - centerY) * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}
