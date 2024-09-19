#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include "utils/structs.h"

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
	Color color;
	size_t stage;
} SceneObject;

DECLARE_ARRLIST(SceneObject);
DECLARE_ARRLIST(ARRLIST_size_t);

typedef struct {
    SceneType type;
    Camera camera3D;
    Camera2D camera2D;
	ARRLIST_SceneObject objects;
} Scene;

Scene* GetScene();

void InitializeScene();

void DrawScene();

void DrawSceneStage(size_t stage);

void DrawLeftovers();

void ResetSceneCamera();

void SaveScene(const char* path);

void LoadScene(const char* path);

#endif
