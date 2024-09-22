#include "viewport.h"
#include "core/scene.h"
#include "panels/chain.h"
#include "raylib.h"

RenderTexture2D g_ViewportTexture = { 0 };

void InitializeViewport() {
	g_ViewportTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    InitializeScene();
}

void CleanViewport() {
	UnloadRenderTexture(g_ViewportTexture);
}

void UpdateViewport() {
    int is3d = GetScene()->type == SCENE3D;
	BeginTextureMode(g_ViewportTexture);
    ClearBackground(BLACK);
    if (is3d) BeginMode3D(GetScene()->camera3D);
    else BeginMode2D(GetScene()->camera2D);
    DrawScene();
    if (is3d) EndMode3D();
    else EndMode2D();
	EndTextureMode();
    for (int i = 0; i < ShaderChainSize(); i++) {
        BeginTextureMode(g_ViewportTexture);
        BeginShaderMode(GetShaderInChain(i));
        if (is3d) BeginMode3D(GetScene()->camera3D);
        else BeginMode2D(GetScene()->camera2D);
		DrawSceneStage(i);
        if (is3d) EndMode3D();
        else EndMode2D();
        DrawTextureRec(
            g_ViewportTexture.texture, 
            (Rectangle){ 0, 0, (float)g_ViewportTexture.texture.width, 
            (float)-g_ViewportTexture.texture.height }, 
            (Vector2){ 0.0f , 0.0f }, WHITE);
        EndShaderMode();
        EndTextureMode();
    }
	BeginTextureMode(g_ViewportTexture);
    if (is3d) BeginMode3D(GetScene()->camera3D);
    else BeginMode2D(GetScene()->camera2D);
	DrawLeftovers();
    if (is3d) EndMode3D();
    else EndMode2D();
	EndTextureMode();
}

void DrawViewport(float x, float y, float w, float h) {
    DrawTextureRec(
		g_ViewportTexture.texture,
		CLITERAL(Rectangle){ g_ViewportTexture.texture.width/2 - w/2, 0, w, -h },
		CLITERAL(Vector2){ x, y }, WHITE);
    GetScene()->camera2D.offset = (Vector2){ w/2.0f, h/2.0f };
}

void ViewportInput() {
    if (IsKeyPressed(KEY_GRAVE)) ResetSceneCamera();
    if (GetScene()->type == SCENE3D){
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
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            rotation.x = rotation_sensitivity * GetMouseDelta().x;
            rotation.y = rotation_sensitivity * GetMouseDelta().y;
        }
        UpdateCameraPro(&GetScene()->camera3D, velocity, rotation, 0.0f);
    } else {
        float movement_speed = 80.0f * GetFrameTime();
        Vector3 velocity = { 0 };
        if (IsKeyDown(KEY_W)) velocity.y = movement_speed;
        if (IsKeyDown(KEY_S)) velocity.y = -movement_speed;
        if (IsKeyDown(KEY_A)) velocity.x = movement_speed;
        if (IsKeyDown(KEY_D)) velocity.x = -movement_speed;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            velocity.x = 1.0f * GetMouseDelta().x / GetScene()->camera2D.zoom;
            velocity.y = 1.0f * GetMouseDelta().y / GetScene()->camera2D.zoom;
        }
        GetScene()->camera2D.zoom += GetMouseWheelMoveV().y;
        GetScene()->camera2D.zoom = GetScene()->camera2D.zoom < 0.001f ? 0.001f : GetScene()->camera2D.zoom;
        GetScene()->camera2D.target.x -= velocity.x;
        GetScene()->camera2D.target.y -= velocity.y;
    }
}
