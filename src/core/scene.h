#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"

typedef enum {
    SCENE2D = 0,
    SCENE3D = 1
} SceneType;

typedef struct {
    SceneType type;
    Camera camera3D;
    Camera2D camera2D;
} Scene;

void InitializeScene(Scene* scene);

void ResetSceneCamera(Scene* scene);

#endif