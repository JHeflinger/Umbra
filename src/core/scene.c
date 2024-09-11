#include "scene.h"

void InitializeScene(Scene* scene) {
    scene->camera3D.position = (Vector3){ 10, 10, 10 };
    scene->camera3D.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    scene->camera3D.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    scene->camera3D.fovy = 90.0f;
    scene->camera3D.projection = CAMERA_PERSPECTIVE;
}