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
	float rotation;
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

typedef enum {
	NONE = 0,
	UNABLE_TO_OPEN_FILE = 1,
	OBNOXIOUS_LONG_LINE = 2,
	NO_START_SCENE_MONIKER = 3,
	INVALID_NUM_SCENE_ARGS = 4,
	INVALID_SCENE_ARG = 5,
	INVALID_OBJECT_MONIKER = 6,
	NO_START_BRACKET = 7,
	INVALID_NUM_OBJECT_ARGS = 8,
	UNALLOWED_END_BRACKET = 9,
	INVALID_PROPERTY = 10,
	OUTSIDE_SCENE_DEF = 11,
	UNALLOWED_START_BRACKET = 12,
	SHOULD_NEVER_HAPPEN = 999,
} LoadErrorCode;

typedef struct {
	LoadErrorCode type;
	size_t line;
	size_t col;
} LoadSceneError;

Scene* GetScene();

void InitializeScene();

void DrawScene();

void DrawSceneStage(size_t stage);

void DrawLeftovers();

void ResetSceneCamera();

void SaveScene(const char* path);

LoadSceneError LoadScene(const char* path);

#endif
