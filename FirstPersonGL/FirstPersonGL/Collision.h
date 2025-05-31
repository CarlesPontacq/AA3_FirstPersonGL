#pragma once

// Header del struct AABB con la definici�n de las variables y las funciones de la c�mara
struct AABB {
    float minX, maxX;
    float minZ, maxZ;
    bool active = true;

    bool contains(float x, float z) const {
        if (active) {
            return (x >= minX && x <= maxX && z >= minZ && z <= maxZ);
        }
        else
        {
            return false;
        }
    }
};