#include "camera.h"
#include <GL/freeglut.h>
#include <cmath>

#define PI 3.14159265f

void Camera::init(int screenWidth, int screenHeight) {
    posX = 0.0f; posY = 1.75f; posZ = 5.0f;
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

    gluLookAt(posX, posY, posZ,
        posX + dirX, posY + dirY, posZ + dirZ,
        0.0f, 1.0f, 0.0f);
}

void Camera::moveForward(float delta) {
    float radYaw = yaw * PI / 180.0f;
    float radPitch = pitch * PI / 180.0f;

    posX += cos(radYaw) * cos(radPitch) * delta;
    posZ += sin(radYaw) * cos(radPitch) * delta;
    posY += sin(radPitch) * delta;
}

void Camera::strafe(float delta) {
    float radYaw = (yaw + 90.0f) * PI / 180.0f;
    posX += cos(radYaw) * delta;
    posZ += sin(radYaw) * delta;
}

void Camera::mouseLook(int x, int y) {
    float sensitivity = 0.2f;
    yaw += (x - centerX) * sensitivity;
    pitch -= (y - centerY) * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}
