#include "viewport.h"
#include "raylib.h"

RenderTexture2D g_ViewportTexture = { 0 };

void InitializeViewport() {
	g_ViewportTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}

void CleanViewport() {
	UnloadRenderTexture(g_ViewportTexture);
}

void UpdateViewport() {
	BeginTextureMode(g_ViewportTexture);
    ClearBackground(BLACK);
	EndTextureMode();
}

void RenderViewport(float x, float y, float w, float h) {
    g_ViewportTexture.texture.width = w;
    g_ViewportTexture.texture.height = h;
    DrawTextureRec(
		g_ViewportTexture.texture, 
		CLITERAL(Rectangle){ 0, 0, w, h }, 
		CLITERAL(Vector2){ x, y }, WHITE);
}