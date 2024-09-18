#include "scene.h"
#include "panels/chain.h"

IMPL_ARRLIST(SceneObject);
IMPL_ARRLIST(ARRLIST_size_t);

Scene g_scene;
ARRLIST_ARRLIST_size_t g_stage_chain;
ARRLIST_size_t g_leftovers;

void draw_scene_object(SceneObject obj) {
	switch (obj.type) {
		case SCENE_GRID:
			DrawGrid(obj.slices, obj.step);
			break;
		case SCENE_CUBE:
			if (g_scene.type == SCENE3D) DrawCube((Vector3){ obj.x, obj.y, obj.z}, obj.w, obj.l, obj.h, obj.color);
			break;
		case SCENE_SPHERE:
			if (g_scene.type == SCENE3D) DrawSphere((Vector3){ obj.x, obj.y, obj.z}, obj.radius, obj.color);
			break;
		case SCENE_RECTANGLE:
			if (g_scene.type == SCENE2D) DrawRectangle(obj.x, obj.y, obj.w, obj.h, obj.color);
			break;
		case SCENE_CIRCLE:
			if (g_scene.type == SCENE2D) DrawCircle(obj.x, obj.y, obj.radius, obj.color);
			break;
		default: break;
	}
}

Scene* GetScene() {
	return &g_scene;
}

void InitializeScene() {
    g_scene.type = SCENE3D;
    ResetSceneCamera();

	SceneObject grid = { 0 };
	grid.type = SCENE_GRID;
	grid.slices = 100;
	grid.step = 10;
	ARRLIST_SceneObject_add(&g_scene.objects, grid);

	SceneObject cube = { 0 };
	cube.type = SCENE_CUBE;
	cube.w = 5.0f;
	cube.l = 10.0f;
	cube.h = 5.0f;
	cube.color = (Color){ 255, 155, 155, 255 };
	ARRLIST_SceneObject_add(&g_scene.objects, cube);

	SceneObject sphere = { 0 };
	sphere.type = SCENE_SPHERE;
	sphere.x = 10.0f;
	sphere.stage = 2;
	sphere.radius = 4.0f;
	sphere.color = (Color){ 155, 255, 155, 255 };
	ARRLIST_SceneObject_add(&g_scene.objects, sphere);
}

void DrawScene() {
	for (int i = 0; i < g_stage_chain.size; i++)
		ARRLIST_size_t_clear(&g_stage_chain.data[i]);
	ARRLIST_size_t_clear(&g_leftovers);
	for (int i = g_stage_chain.size; i < ShaderChainSize(); i++) {
		ARRLIST_size_t newarr = { 0 };
		ARRLIST_ARRLIST_size_t_add(&g_stage_chain, newarr);
	}

	for (int i = 0; i < g_scene.objects.size; i++) {
		SceneObject obj = ARRLIST_SceneObject_get(&g_scene.objects, i);
		if (obj.stage != 0) {
			if (obj.stage > ShaderChainSize())
				ARRLIST_size_t_add(&g_leftovers, (size_t)i);
			else
				ARRLIST_size_t_add(&g_stage_chain.data[obj.stage - 1], (size_t)i);
			continue;
		}
		draw_scene_object(obj);	
	}
}

void DrawSceneStage(size_t stage) {
	for (int i = 0; i < g_stage_chain.data[stage].size; i++) {
		SceneObject obj = g_scene.objects.data[g_stage_chain.data[stage].data[i]];
		draw_scene_object(obj);
	}
}

void DrawLeftovers() {
	for (int i = 0; i < g_leftovers.size; i++) {
		SceneObject obj = g_scene.objects.data[g_leftovers.data[i]];
		draw_scene_object(obj);
	}
}

void ResetSceneCamera() {
    g_scene.camera3D.position = (Vector3){ 10, 10, 10 };
    g_scene.camera3D.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    g_scene.camera3D.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    g_scene.camera3D.fovy = 90.0f;
    g_scene.camera3D.projection = CAMERA_PERSPECTIVE;
}
