#include <GL/freeglut.h>
#include "Camera.h"
#include "Collision.h"
#include <cmath>
#include <cstdlib>

AABB houseAABB = { -6.5f, -3.5f, -6.5f, -3.5f }; // Collider de la casa
AABB doorAABB = { -5.25f, -4.75f, -3.6f, -3.4f }; // Collider de la puerta

Camera camera;
bool keys[256];
bool isNight = true;
float sunAngle = 0.0f;
float dayDuration = 60.0f; // segons per a un dia complet
float lastTime = 0.0f;
float doorAngle = 0.0f;
bool doorOpen = false;
const float PI = 3.14159f;

float carX = -50.0f;   
float carZ = 0.0f;
float carSpeed = 5.0f; 
float carAngle = 0.0f;
int currentPathIndex = 0;
const int numPathSegments = 4;
float pathAngles[] = { 0.0f, 90.0f, 180.0f, 270.0f };

// Esta funcion sirve para modificar el collider de la puerta segun que tan abierta o cerrada esta
AABB computeDoorAABB(float doorAngleDegrees) {
    // Centro de la puerta
    float centerX = -5.0f;  // Posici�n de la casa en X
    float centerZ = -3.5f;  // Posici�n frontal donde se encuentra la puerta

    // Tama�o de la puerta
    float width = 0.5f;
    float depth = 0.1f;

    // Cuando est� cerrada, es vertical en Z
    if (doorAngleDegrees == 0.0f) {
        return { centerX - width / 2, centerX + width / 2,
                 centerZ - depth / 2, centerZ + depth / 2 };
    }
    // Si est� completamente abierta (90�), es horizontal
    else if (doorAngleDegrees == 90.0f) {
        return { centerX - depth / 2, centerX + depth / 2,
                 centerZ - width / 2, centerZ + width / 2 };
    }
    // Intermedia: aproximamos por interpolaci�n lineal (simple pero efectiva)
    else {
        float t = doorAngleDegrees / 90.0f;
        float minX = centerX - (1.0f - t) * width / 2 - t * depth / 2;
        float maxX = centerX + (1.0f - t) * width / 2 + t * depth / 2;
        float minZ = centerZ - (1.0f - t) * depth / 2 - t * width / 2;
        float maxZ = centerZ + (1.0f - t) * depth / 2 + t * width / 2;
        return { minX, maxX, minZ, maxZ };
    }
}

// Esta funci�n comprueba si la camera esta collisionando con la puerta o la casa
bool isColliding(float x, float z) {
    AABB doorCollider = computeDoorAABB(doorAngle);

    return houseAABB.contains(x, z) || doorCollider.contains(x, z);
}


// Esta funci�n dibuja el modelo visual de la linterna en la vista del jugador, 
// con transformaciones aplicadas directamente al HUD (no al mundo 3D).
void drawFlashlightModel() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(60, 800.0 / 600.0, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(0.5f, -0.5f, -1.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);

    GLUquadric* quad = gluNewQuadric();
    glColor3f(0.8f, 0.8f, 0.2f);
    gluCylinder(quad, 0.04f, 0.035f, 0.2f, 16, 4);
    gluDeleteQuadric(quad);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Funci�n que configura y posiciona la luz spotlight que simula la linterna desde el HUD.
void updateFlashlightFromHUD() {
    if (!isNight) {
        glDisable(GL_LIGHT1);
        return;
    }

    glEnable(GL_LIGHT1);

    GLfloat pos[] = { 0.3f, 1.5f, 4.4f, 1.0f };
    GLfloat dir[] = { 0.0f, 0.0f, -1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 15.0f);
    GLfloat diffuse[] = { 1.0f, 1.0f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
}

// Funci�n que sirve para dibujar un solo �rbol
void drawTree(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Tronc
    glPushMatrix();
    glColor3f(0.5f, 0.25f, 0.1f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f); 
    GLUquadric* trunk = gluNewQuadric();
    gluCylinder(trunk, 0.1f, 0.1f, 1.0f, 12, 3);
    gluDeleteQuadric(trunk);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 1.0f + 0.5f, 0.0f);
    glColor3f(0.0f, 0.6f, 0.0f);
    glutSolidSphere(0.5f, 12, 12);
    glPopMatrix();

    glPopMatrix();
}

// Funci�n que llama a drawTree() segun un array de posiciones para dibujar todo el bosque
void drawForest() {
    for (float x = -50; x <= 50; x += 3) {
        for (float z = -50; z <= 50; z += 3) {
            if ((int(x + z)) % 2 == 0)
                drawTree(x, z);
        }
    }
}

// Funcion para dibujar el suelo de la escena
void drawGround() {
    glColor3f(0.1f, 0.35f, 0.1f); 
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();
}

// Ajusta la intensidad y color de la luz ambiental en funci�n del �ngulo solar para simular un ciclo d�a-noche.
void updateLighting() {
    float rawIntensity = sinf(sunAngle);
    float intensity = 0.5f * rawIntensity + 0.5f;

    float lightColor[4];
    if (intensity > 0.1f) {
        lightColor[0] = 1.0f * intensity;
        lightColor[1] = 1.0f * intensity;
        lightColor[2] = 0.9f * intensity;
    }
    else {
        lightColor[0] = 0.1f * intensity;
        lightColor[1] = 0.1f * intensity;
        lightColor[2] = 0.2f * intensity;
    }
    lightColor[3] = 1.0f;

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

    float ambientStrength = (intensity > 0.1f) ? 0.3f : 0.1f;
    float ambient[4] = { ambientStrength, ambientStrength, ambientStrength, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}

// Calcula e interpola el color del cielo a lo largo del ciclo diario (ma�ana, d�a, tarde, noche).
void SkyColorChange() {
    float t = fmodf(sunAngle, 2.0f * PI) / (2.0f * PI);

    float morning[3] = { 1.0f, 0.5f, 0.2f };
    float day[3] = { 0.5f, 0.8f, 1.0f };
    float evening[3] = { 1.0f, 0.3f, 0.0f };
    float night[3] = { 0.05f, 0.05f, 0.2f };

    float skyRGB[3];

    if (t < 0.25f) {
        float localT = t / 0.25f;
        for (int i = 0; i < 3; ++i)
            skyRGB[i] = morning[i] * (1.0f - localT) + day[i] * localT;
    }
    else if (t < 0.5f) {
        float localT = (t - 0.25f) / 0.25f;
        for (int i = 0; i < 3; ++i)
            skyRGB[i] = day[i] * (1.0f - localT) + evening[i] * localT;
    }
    else if (t < 0.75f) {
        float localT = (t - 0.5f) / 0.25f;
        for (int i = 0; i < 3; ++i)
            skyRGB[i] = evening[i] * (1.0f - localT) + night[i] * localT;
    }
    else {
        float localT = (t - 0.75f) / 0.25f;
        for (int i = 0; i < 3; ++i)
            skyRGB[i] = night[i] * (1.0f - localT) + morning[i] * localT;
    }

    glClearColor(skyRGB[0], skyRGB[1], skyRGB[2], 1.0f);
}

// Funci�n para dibujar la casa por el centro de la escena
void drawHouse(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Cos principal de la casa
    glPushMatrix();
    glColor3f(0.6f, 0.4f, 0.3f);
    glTranslatef(0.0f, 1.0f, 0.0f);
    glScalef(3.0f, 2.0f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Sostre
    glPushMatrix();
    glColor3f(0.4f, 0.1f, 0.1f);
    glTranslatef(0.0f, 2.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.5f, 0.0f, 1.5f);
    glVertex3f(1.5f, 0.0f, 1.5f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.5f, 0.0f, -1.5f);
    glVertex3f(-1.5f, 0.0f, -1.5f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.5f, 0.0f, -1.5f);
    glVertex3f(-1.5f, 0.0f, 1.5f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.5f, 0.0f, 1.5f);
    glVertex3f(1.5f, 0.0f, -1.5f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.5f); 
    glTranslatef(-0.25f, 1.0f, 0.01f); 
    glRotatef(-doorAngle, 0.0f, 1.0f, 0.0f); 
    glTranslatef(0.25f, -1.0f, 0.0f); 
    glColor3f(0.3f, 0.2f, 0.1f);
    glScalef(0.5f, 2.0f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

// Funci�n para dibujar el coche que se va moviendo por la escena
void drawCar(float x, float z, float angle) {
    glPushMatrix();
    glTranslatef(x, 0.5f, z);              
    glRotatef(angle, 0.0f, 1.0f, 0.0f);   

    // Cos 
    glPushMatrix();
    glColor3f(0.2f, 0.2f, 1.0f); 
    glScalef(2.0f, 0.5f, 1.0f); 
    glutSolidCube(1.0f);
    glPopMatrix();

    // Rodes (4)
    glColor3f(0.0f, 0.0f, 0.0f); 
    float offsetX = 0.9f;
    float offsetZ = 0.4f;
    float wheelY = -0.25f;

    
    glPushMatrix();
    glTranslatef(-offsetX, wheelY, offsetZ);
    glutSolidSphere(0.2f, 10, 10);
    glPopMatrix();

    
    glPushMatrix();
    glTranslatef(-offsetX, wheelY, -offsetZ);
    glutSolidSphere(0.2f, 10, 10);
    glPopMatrix();

    
    glPushMatrix();
    glTranslatef(offsetX, wheelY, offsetZ);
    glutSolidSphere(0.2f, 10, 10);
    glPopMatrix();

    
    glPushMatrix();
    glTranslatef(offsetX, wheelY, -offsetZ);
    glutSolidSphere(0.2f, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

// Funci�n para dibujar el contenido del minimapa en 2D
void drawMinimap() {
    // Fondo del minimapa
    glColor3f(0.1f, 0.3f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-60.0f, -60.0f);
    glVertex2f(60.0f, -60.0f);
    glVertex2f(60.0f, 60.0f);
    glVertex2f(-60.0f, 60.0f);
    glEnd();

    // Casa
    glColor3f(1.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-6.5f, -6.5f);
    glVertex2f(-3.5f, -6.5f);
    glVertex2f(-3.5f, -3.5f);
    glVertex2f(-6.5f, -3.5f);
    glEnd();

    // Arbres 
    glColor3f(0.0f, 0.8f, 0.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (float x = -50; x <= 50; x += 5) {
        for (float z = -50; z <= 50; z += 5) {
            if ((int(x + z)) % 2 == 0)
                glVertex2f(x, z);
        }
    }
    glEnd();

    // Cotxe 
    glColor3f(0.2f, 0.2f, 1.0f);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex2f(carX, carZ);
    glEnd();

    // Jugador
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex2f(camera.getX(), camera.getZ());
    glEnd();
}

// Prepara la configuraci�n de una viewport secundaria con proyecci�n ortogr�fica para mostrar el minimapa en 2D.
void renderMinimapViewport(int screenWidth, int screenHeight) {
    
    glViewport(screenWidth - 200, screenHeight - 200, 200, 200);

    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-60, 60, -60, 60, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    drawMinimap();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

}

// Funci�n para ir dibujando todos los elementos de la escena por orden
void display() {
    glViewport(0, 0, 800, 600); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 800.0 / 600.0, 0.1, 1000.0); 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.apply();

    SkyColorChange();
    updateLighting();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (isNight) glDisable(GL_LIGHT2);
    else glEnable(GL_LIGHT2);

    updateFlashlightFromHUD();
    glLoadIdentity();

    camera.apply();
    camera.updateFlashlight(isNight);
    drawFlashlightModel();

    drawGround();
    drawForest();
    drawHouse(-5.0f, -5.0f);
    drawCar(carX, carZ, carAngle);

    renderMinimapViewport(800, 600);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = 1.0f * w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, ratio, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

// Funci�n para ir actualizando las posiciones o valores de algunos objetos 
void timer(int v) {
    float speed = 0.1f;
    if (keys['w'] || keys['W']) camera.moveForward(speed);
    if (keys['s'] || keys['S']) camera.moveForward(-speed);
    if (keys['a'] || keys['A']) camera.strafe(-speed);
    if (keys['d'] || keys['D']) camera.strafe(speed);

    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    sunAngle += (2.0f * PI / dayDuration) * deltaTime;
    if (sunAngle > 2.0f * PI) sunAngle -= 2.0f * PI;

    float targetAngle = doorOpen ? 90.0f : 0.0f;
    if (doorAngle < targetAngle) doorAngle += 2.0f;
    else if (doorAngle > targetAngle) doorAngle -= 2.0f;

    float rad = carAngle * PI / 180.0f;
    carX += cos(rad) * carSpeed * deltaTime;
    carZ += sin(rad) * carSpeed * deltaTime;

    
    if (abs(carX) > 50 || abs(carZ) > 50) {
        currentPathIndex = rand() % numPathSegments; 
        carAngle = pathAngles[currentPathIndex];

        
        if (carX > 50) carX = 49;
        if (carX < -50) carX = -49;
        if (carZ > 50) carZ = 49;
        if (carZ < -50) carZ = -49;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// Funci�n para encender o apagar la linterna o abrir o cerrar la puerta de la casa
void keyDown(unsigned char key, int x, int y) {
    keys[key] = true;
    if (key == 'n' || key == 'N') isNight = !isNight;
    if (key == 'e' || key == 'E') doorOpen = !doorOpen;
}

void keyUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

//Funci�n para coger la posicion del raton en la pantalla
void mouseMotion(int x, int y) {
    camera.mouseLook(x, y);
    glutWarpPointer(400, 300);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Explorador 3D - Projecte AA3");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    GLfloat lightDir[] = { -1.0f, -1.0f, -0.5f, 0.0f };
    GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT2, GL_POSITION, lightDir);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT2, GL_SPECULAR, white);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glutSetCursor(GLUT_CURSOR_NONE);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutPassiveMotionFunc(mouseMotion);
    glutTimerFunc(0, timer, 0);

    glutWarpPointer(400, 300);
    camera.init(800, 600);

    glutMainLoop();
    return 0;
}
