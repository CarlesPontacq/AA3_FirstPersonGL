#pragma once
#define CAMERA_H

class Camera {
public:
    void init(int screenWidth, int screenHeight);
    void apply();
    void moveForward(float delta);
    void strafe(float delta);
    void mouseLook(int x, int y);

private:
    float posX, posY, posZ;
    float yaw, pitch;
    int centerX, centerY;
};


