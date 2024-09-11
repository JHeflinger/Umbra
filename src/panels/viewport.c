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