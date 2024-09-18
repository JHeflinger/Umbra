#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include "easyobjects.h"

typedef enum {
    SCENE2D = 0,
    SCENE3D = 1
} SceneType;

typedef enum {
	SCENE_GRID,
	SCENE_CUBE,
	SCENE_SPHERE,
	SCENE_RECTANGLE,
	SCENE_CIRCLE
} ObjectType;

typedef struct {
	ObjectType type;
	float x;
	float y;
	float z;
	float w;
	float l;
	float h;
	float radius;
	float slices;
	float step;
	Vector3 rotation;
} SceneObject;

DECLARE_ARRLIST(SceneObject);

typedef struct {
    SceneType type;
    Camera camera3D;
    Camera2D camera2D;
	ARRLIST_SceneObject objects;
} Scene;

void InitializeScene(Scene* scene);

void DrawScene(Scene* scene);

void ResetSceneCamera(Scene* scene);

#endif
