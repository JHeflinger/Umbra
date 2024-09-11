#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"

typedef enum {
    Orthographic = 0,
    Perspective = 1
} CameraType;

typedef struct {
    CameraType type;
    Camera camera3D;
    Camera2D camera2D;
} Scene;

void InitializeScene(Scene* scene);

#endif