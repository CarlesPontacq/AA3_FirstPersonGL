#include <GL/glut.h>
#include <cmath>

float sunAngle = 0.0f;
float dayDuration = 30.0f; // Seconds for a full day
float lastTime = 0.0f;
float intensity = 1.0f;

float morning[3] = { 1.0f, 0.5f, 0.2f };
float day[3] = { 0.5f, 0.8f, 1.0f };
float evening[3] = { 1.0f, 0.3f, 0.0f };
float night[3] = { 0.05f, 0.05f, 0.2f };

const float PI = 3.14159f;
float lampIntensity = 0.0f;

float cameraX = 15.0;
float cameraZ = 20.0;

static float cameraAngle = 3.75f;

void PlayerCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float eyeX = cameraX + sin(cameraAngle);
    float eyeZ = cameraZ + cos(cameraAngle);

    gluLookAt(cameraX, 5.0f, cameraZ,
        eyeX, 5.0f, eyeZ,
        0.0f, 1.0f, 0.0f);
}


void updateLighting() {
    float lightColor[4];

    float rawIntensity = sinf(sunAngle);
    intensity = 0.5f * rawIntensity + 0.5f;

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

    // To avoid making the scene objects to dark
    intensity = fmaxf(0.5f, intensity);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

    float ambientStrength = (intensity > 0.1f) ? 0.3f : 0.1f;
    float ambient[4] = { ambientStrength, ambientStrength, ambientStrength, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}

void updateLampLighting() {
    float lampLightColor[4];

    float rawIntensity = cosf(sunAngle);

    lampIntensity = (intensity <= 0.5f) ? 1.0f : 0.0f;

    lampLightColor[0] = 1.0f * lampIntensity;
    lampLightColor[1] = 1.0f * lampIntensity;
    lampLightColor[2] = 0.9f * lampIntensity;
    lampLightColor[3] = 1.0f;

    // To avoid making the scene objects to dark
    lampIntensity = fmaxf(0.2f, lampIntensity);

    glLightfv(GL_LIGHT1, GL_DIFFUSE, lampLightColor);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lampLightColor);
}

void drawScene() {
    float safeIntensity = fmaxf(0.4f, intensity);
    // Ground
    // Intensity multiplying the Color values to make the color change through the day
    glColor3f(0.3f * safeIntensity, 0.9f * safeIntensity, 0.3f * safeIntensity);
    glBegin(GL_QUADS);
    glVertex3f(-50.0f, 0.0f, -50.0f);
    glVertex3f(50.0f, 0.0f, -50.0f);
    glVertex3f(50.0f, 0.0f, 50.0f);
    glVertex3f(-50.0f, 0.0f, 50.0f);
    glEnd();

    // Building 
    glPushMatrix();
    glColor3f(0.5f * safeIntensity, 0.5f * safeIntensity, 0.5f * safeIntensity);
    glTranslatef(-10.0f, 5.0f, 0.0f);
    glScalef(5.0f, 10.0f, 5.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Tree body
    glPushMatrix();
    glColor3f(0.55f * safeIntensity, 0.27f * safeIntensity, 0.07f * safeIntensity);
    glTranslatef(10.0f, 1.0f, -10.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* treeTrunk = gluNewQuadric();
    gluCylinder(treeTrunk, 0.25f, 0.25f, 2.0f, 16, 16);
    gluDeleteQuadric(treeTrunk);
    glPopMatrix();

    // Tree leaves 
    glPushMatrix();
    glColor3f(0.0f * safeIntensity, 0.8f * safeIntensity, 0.0f * safeIntensity);
    glTranslatef(10.0f, 3.0f, -10.0f);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    // Lamp body
    glPushMatrix();
    glColor3f(0.5f * safeIntensity, 0.5f * safeIntensity, 0.5f * safeIntensity);
    glTranslatef(0.0f, 2.5f, 10.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* lampPole = gluNewQuadric();
    gluCylinder(lampPole, 0.2f, 0.2f, 3.0f, 16, 16);
    gluDeleteQuadric(lampPole);
    glPopMatrix();

    // Lamp light
    glPushMatrix();
    glColor3f(1.0f * lampIntensity, 1.0f * lampIntensity, 0.0f * lampIntensity);
    glTranslatef(0.0f, 5.5f, 10.0f);
    glutSolidSphere(0.4f, 16, 16);
    glPopMatrix();

    // Sun
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 0.0f);
    float sunX = 30.0f * cosf(sunAngle);
    float sunY = 30.0f * sinf(sunAngle);
    glTranslatef(sunX, sunY + 20.0f, 0.0f);
    glutSolidSphere(2.0, 20, 20);
    glPopMatrix();
}

void SkycColorChange() {
    // To determine what part of the day is and normalize it
    float t = fmodf(sunAngle, 2.0f * PI) / (2.0f * PI);
    float skyRGB[3];

    if (t >= 0.0f && t < 0.25f) {
        // To determine what part of this fase of the day is (beggining, middle or ending of the morning)
        float localT = t / 0.25f;
        for (int i = 0; i < 3; ++i) {
            // To combine the color of the beggining of the fase and the ending of the fase
            skyRGB[i] = morning[i] * (1.0f - localT) + day[i] * localT;
        }
    }
    // The case for the day
    else if (t >= 0.25f && t < 0.5f) {
        float localT = (t - 0.25f) / 0.25f;
        for (int i = 0; i < 3; ++i) {
            skyRGB[i] = day[i] * (1.0f - localT) + evening[i] * localT;
        }
    }
    // The case for the evening
    else if (t >= 0.5f && t < 0.75f) {
        float localT = (t - 0.5f) / 0.25f;
        for (int i = 0; i < 3; ++i) {
            skyRGB[i] = evening[i] * (1.0f - localT) + night[i] * localT;
        }
    }
    // The case for the night
    else {
        float localT = (t - 0.75f) / 0.25f;
        for (int i = 0; i < 3; ++i) {
            skyRGB[i] = night[i] * (1.0f - localT) + morning[i] * localT;
        }
    }

    glClearColor(skyRGB[0], skyRGB[1], skyRGB[2], 1.0f);
}

void display() {
    SkycColorChange();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    PlayerCamera();

    updateLighting();
    updateLampLighting();

    float sunX = 30.0f * cosf(sunAngle);
    float sunY = 30.0f * sinf(sunAngle);
    float sunPos[] = { sunX, sunY + 20.0f, 0.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, sunPos);

    float lampPos[] = { 0.0f, 5.5f, 10.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lampPos);


    drawScene();

    glutSwapBuffers();
}

void handleKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
    case 'W':
        cameraX += sin(cameraAngle);
        cameraZ += cos(cameraAngle);
        break;
    case 'a':
    case 'A':
        cameraAngle += 0.1f;
        break;
    case 's':
    case 'S':
        cameraX -= sin(cameraAngle);
        cameraZ -= cos(cameraAngle);
        break;
    case 'd':
    case 'D':
        cameraAngle -= 0.1f;
        break;
    case 'r':
    case 'R':
        cameraAngle = 3.75f;
        cameraX = 15.0;
        cameraZ = 20.0;
        break;
    case 27: exit(0);
    }

    glutPostRedisplay();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    sunAngle += (2.0f * 3.14159f / dayDuration) * deltaTime;
    if (sunAngle > 2.0f * 3.14159f)
        sunAngle -= 2.0f * 3.14159f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    float materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float materialShininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Joc d’Exploraci´o en Primera Persona");

    init();

    glutKeyboardFunc(handleKeyboard);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();

    return 0;
}