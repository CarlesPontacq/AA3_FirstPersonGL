#include <GL/freeglut.h>
#include "Camera.h"

Camera camera;
bool keys[256];
bool isNight = true;

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

void drawTree(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Tronc
    glPushMatrix();
    glColor3f(0.5f, 0.25f, 0.1f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f); // Fer que el cilindre sigui vertical
    GLUquadric* trunk = gluNewQuadric();
    gluCylinder(trunk, 0.1f, 0.1f, 1.0f, 12, 3);
    gluDeleteQuadric(trunk);
    glPopMatrix();

    // Copa (esfera damunt del tronc)
    glPushMatrix();
    glTranslatef(0.0f, 1.0f + 0.5f, 0.0f);
    glColor3f(0.0f, 0.6f, 0.0f);
    glutSolidSphere(0.5f, 12, 12);
    glPopMatrix();

    glPopMatrix();
}

void drawForest() {
    for (float x = -50; x <= 50; x += 3) {
        for (float z = -50; z <= 50; z += 3) {
            if ((int(x + z)) % 2 == 0)
                drawTree(x, z);
        }
    }
}

void drawGround() {
    glColor3f(0.1f, 0.35f, 0.1f); // Verd fosc, tipus gespa
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();
}

void display() {
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

    glColor3f(0.0f, 0.5f, 1.0f);
    glutSolidCube(1);

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

void timer(int v) {
    float speed = 0.1f;
    if (keys['w']) camera.moveForward(speed);
    if (keys['s']) camera.moveForward(-speed);
    if (keys['a']) camera.strafe(-speed);
    if (keys['d']) camera.strafe(speed);

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyDown(unsigned char key, int x, int y) {
    keys[key] = true;
    if (key == 'n') isNight = !isNight;
}

void keyUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

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
