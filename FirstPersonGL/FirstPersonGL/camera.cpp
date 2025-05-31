#include "camera.h"
#include <GL/freeglut.h>
#include <cmath>
#include "Collision.h"

#define PI 3.14159265f

// Funci�n para coger la definicion del isColliding del main y usarla 
extern bool isColliding(float x, float z);

// Inicializa la posici�n y orientaci�n de la c�mara.
// Tambi�n guarda el centro de la pantalla para calcular la rotaci�n con el rat�n.
void Camera::init(int screenWidth, int screenHeight) {
    posX = 0.0f; posY = CAMERA_HEIGHT; posZ = 5.0f;
    yaw = -90.0f; pitch = 0.0f;
    centerX = screenWidth / 2;
    centerY = screenHeight / 2;
}

// Calcula la direcci�n de visi�n de la c�mara a partir de yaw y pitch.
// Establece la vista utilizando gluLookAt para simular una c�mara en primera persona.
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

// Activa o desactiva la linterna (luz spotlight) en funci�n del par�metro 'enabled'.
// La luz sigue la posici�n y orientaci�n actual de la c�mara.
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

// Mueve la c�mara hacia adelante o hacia atr�s seg�n el valor de 'delta' y comprueba si collisiona con algun objeto.
void Camera::moveForward(float delta) {
    float radYaw = yaw * PI / 180.0f;
    float radPitch = pitch * PI / 180.0f;

    float nextX = posX + cos(radYaw) * cos(radPitch) * delta;
    float nextZ = posZ + sin(radYaw) * cos(radPitch) * delta;

    if (!isColliding(nextX, nextZ)) {
        posX = nextX;
        posZ = nextZ;
    }

    posY = CAMERA_HEIGHT;
}

// Desplaza la c�mara lateralmente (izquierda o derecha) seg�n 'delta' y comprueba si collisiona con algun objeto.
void Camera::strafe(float delta) {
    float radYaw = (yaw + 90.0f) * PI / 180.0f;
    float nextX = posX + cos(radYaw) * delta;
    float nextZ = posZ + sin(radYaw) * delta;

    if (!isColliding(nextX, nextZ)) {
        posX = nextX;
        posZ = nextZ;
    }

    posY = CAMERA_HEIGHT;
}

// Actualiza los �ngulos de orientaci�n (yaw y pitch) en funci�n del movimiento del rat�n.
// Aplica sensibilidad y limita el �ngulo vertical para evitar rotaciones completas.
void Camera::mouseLook(int x, int y) {
    float sensitivity = 0.2f;
    yaw += (x - centerX) * sensitivity;
    pitch -= (y - centerY) * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}