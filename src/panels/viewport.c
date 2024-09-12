#include "viewport.h"
#include "core/scene.h"
#include "raylib.h"

RenderTexture2D g_ViewportTexture = { 0 };
Scene           g_Scene           = { 0 };

void InitializeViewport() {
	g_ViewportTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    InitializeScene(&g_Scene);
}

void CleanViewport() {
	UnloadRenderTexture(g_ViewportTexture);
}

void UpdateViewport() {
	BeginTextureMode(g_ViewportTexture);
    ClearBackground(BLACK);
    BeginMode3D(g_Scene.camera3D);
    DrawGrid(100, 1.0f);
    EndMode3D();
	EndTextureMode();
}

void RenderViewport(float x, float y, float w, float h) {
    DrawTextureRec(
		g_ViewportTexture.texture,
		CLITERAL(Rectangle){ g_ViewportTexture.texture.width/2 - w/2, 0, w, -h },
		CLITERAL(Vector2){ x, y }, WHITE);
}

void ViewportInput() {
    if (g_Scene.type = SCENE3D){
        float movement_speed = 20.0f * GetFrameTime();
        float rotation_sensitivity = 10.0f * GetFrameTime();
        Vector3 rotation = { 0 };
        Vector3 velocity = { 0 };
        if (IsKeyDown(KEY_W)) velocity.x = movement_speed;
        if (IsKeyDown(KEY_S)) velocity.x = -movement_speed;
        if (IsKeyDown(KEY_A)) velocity.y = -movement_speed;
        if (IsKeyDown(KEY_D)) velocity.y = movement_speed;
        if (IsKeyDown(KEY_SPACE)) velocity.z = movement_speed;
        if (IsKeyDown(KEY_LEFT_SHIFT)) velocity.z = -movement_speed;
        if (IsKeyPressed(KEY_GRAVE)) ResetSceneCamera(&g_Scene);
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            rotation.x = rotation_sensitivity * GetMouseDelta().x;
            rotation.y = rotation_sensitivity * GetMouseDelta().y;
        }
        UpdateCameraPro(&g_Scene.camera3D, velocity, rotation, 0.0f);
    }
    
}