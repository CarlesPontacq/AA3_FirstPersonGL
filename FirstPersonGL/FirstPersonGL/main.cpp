// main.cpp
#include <GL/freeglut.h>
#include "Camera.h"

Camera camera;
bool keys[256];

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    camera.apply();

    // Escena de prova (plànol i cub)
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glEnd();

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
