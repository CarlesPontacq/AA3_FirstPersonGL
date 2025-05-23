#pragma once

class Camera {
public:
    void init(int screenWidth, int screenHeight);
    void apply();
    void moveForward(float delta);
    void strafe(float delta);
    void mouseLook(int x, int y);
    void updateFlashlight(bool enabled);
    float getX() const { return posX; }
    float getZ() const { return posZ; }


private:
    float posX, posY, posZ;
    float yaw, pitch;
    int centerX, centerY;

    const float CAMERA_HEIGHT = 1.75f;
};







