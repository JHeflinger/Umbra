#include "scene.h"

IMPL_ARRLIST(SceneObject);

void InitializeScene(Scene* scene) {
    scene->type = SCENE3D;
    ResetSceneCamera(scene);

	SceneObject grid = { 0 };
	grid.type = SCENE_GRID;
	grid.slices = 100;
	grid.step = 10;
	ARRLIST_SceneObject_add(&scene->objects, grid);
}

void DrawScene(Scene* scene) {
	if (scene->type == SCENE3D) {
		for (int i = 0; i < scene->objects.size; i++) {
			SceneObject obj = ARRLIST_SceneObject_get(&scene->objects, i);
			switch (obj.type) {
				case SCENE_GRID:
					DrawGrid(obj.slices, obj.step);
					break;
				default: break;
			}
		}
		DrawCube((Vector3){ 0, 0, 0 }, 5.0f, 5.0f, 5.0f, WHITE);
	}
}

void ResetSceneCamera(Scene* scene) {
    scene->camera3D.position = (Vector3){ 10, 10, 10 };
    scene->camera3D.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    scene->camera3D.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    scene->camera3D.fovy = 90.0f;
    scene->camera3D.projection = CAMERA_PERSPECTIVE;
}
