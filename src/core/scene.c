#include "scene.h"
#include "panels/chain.h"
#include "utils/files.h"

#define MAX_LINE_SIZE 256
#define MAX_ARGS 4

IMPL_ARRLIST(SceneObject);
IMPL_ARRLIST(ARRLIST_size_t);

Scene g_scene;
ARRLIST_ARRLIST_size_t g_stage_chain;
ARRLIST_size_t g_leftovers;

typedef struct {
	char moniker[MAX_LINE_SIZE];
	char valstr[MAX_ARGS][MAX_LINE_SIZE];
	int numvals;
	int startbracket;
	int endbracket;
	int property;
} PrunedLine;

PrunedLine prune_line(const char* line) {
	PrunedLine pl = { 0 };

	int ind = 0;
	int parsing_moniker = 0;
	int moniker_ind = 0;
	int parsing_args = 0;
	int arg_ind = 0;
	#define CHECK_NOT_TRAIL(x) (x != ' ' && x != '\t')
	#define CHECK_YES_TRAIL(x) (x == ' ' || x == '\t')
	while (line[ind] != '\0') {
		if (parsing_moniker == 0 && CHECK_NOT_TRAIL(line[ind])) parsing_moniker = 1;
		if (parsing_moniker == 1) {
			if (CHECK_YES_TRAIL(line[ind])) {
				parsing_moniker = 2;
			} else {
				pl.moniker[moniker_ind] = line[ind];
				moniker_ind++;
			}
		}
		if (parsing_moniker == 2) {
			if (CHECK_NOT_TRAIL(line[ind])) {
				if (parsing_args == 0) {
					parsing_args = 1;
					arg_ind = 0;
					pl.numvals++;
				}
			} else {
				parsing_args = 0;
			}
			if (parsing_args == 1) {
				pl.valstr[pl.numvals - 1][arg_ind] = line[ind];
				arg_ind++;
			}
		}
		ind++;
	}
	#undef CHECK_NOT_TRAIL
	#undef CHECK_YES_TRAIL

	if (strcmp(pl.moniker, "}") == 0) {
		strcpy(pl.moniker, "N/A");
		pl.endbracket = 1;
	}

	if (strcmp(pl.valstr[0], "{") == 0) {
		pl.numvals--;
		pl.startbracket = 1;
		for (int i = 0; i < MAX_ARGS - 1; i++)
			strcpy(pl.valstr[i], pl.valstr[i+1]);
	}

	if (pl.moniker[strlen(pl.moniker) - 1] == ':') {
		pl.property = 1;
		pl.moniker[strlen(pl.moniker) - 1] = '\0';
	}

	if (strcmp(pl.moniker, "SCENE2D") == 0 || strcmp(pl.moniker, "SCENE3D") == 0) {
		strcpy(pl.valstr[0], pl.moniker + 5);
		pl.moniker[5] = '\0';
	}

	return pl;
}

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

	LoadSceneError err = LoadScene("default.slumbra");
	printf("error: %d\n", (int)err.type);
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

void SaveScene(const char* path) {
	char* savedata = EZALLOC(20 + g_scene.objects.size*200, sizeof(char));
	size_t data_len = 0;
	data_len += sprintf(savedata + data_len, "SCENE%s {\n", g_scene.type == SCENE3D ? "3D" : "2D");
	for (int i = 0; i < g_scene.objects.size; i++) {
		SceneObject obj = ARRLIST_SceneObject_get(&g_scene.objects, i);
		switch (obj.type) {
			case SCENE_GRID:
				data_len += sprintf(savedata + data_len, "\tGrid {\n");
				data_len += sprintf(savedata + data_len, "\t\tslices: %f\n", obj.slices);
				data_len += sprintf(savedata + data_len, "\t\tstep: %f\n", obj.step);
				data_len += sprintf(savedata + data_len, "\t\tstage: %d\n", (int)obj.stage);
				data_len += sprintf(savedata + data_len, "\t}\n");
				break;
			case SCENE_CUBE:
				data_len += sprintf(savedata + data_len, "\tCube {\n");
				data_len += sprintf(savedata + data_len, "\t\tx: %f\n", obj.x);
				data_len += sprintf(savedata + data_len, "\t\ty: %f\n", obj.y);
				data_len += sprintf(savedata + data_len, "\t\tz: %f\n", obj.z);
				data_len += sprintf(savedata + data_len, "\t\tw: %f\n", obj.w);
				data_len += sprintf(savedata + data_len, "\t\tl: %f\n", obj.l);
				data_len += sprintf(savedata + data_len, "\t\th: %f\n", obj.h);
				data_len += sprintf(savedata + data_len, "\t\tcolor: %d %d %d %d\n", obj.color.r, obj.color.g, obj.color.b, obj.color.a);
				data_len += sprintf(savedata + data_len, "\t\tstage: %d\n", (int)obj.stage);
				data_len += sprintf(savedata + data_len, "\t}\n");
				break;
			case SCENE_SPHERE:
				data_len += sprintf(savedata + data_len, "\tSphere {\n");
				data_len += sprintf(savedata + data_len, "\t\tx: %f\n", obj.x);
				data_len += sprintf(savedata + data_len, "\t\ty: %f\n", obj.y);
				data_len += sprintf(savedata + data_len, "\t\tz: %f\n", obj.z);
				data_len += sprintf(savedata + data_len, "\t\tradius: %f\n", obj.radius);
				data_len += sprintf(savedata + data_len, "\t\tcolor: %d %d %d %d\n", obj.color.r, obj.color.g, obj.color.b, obj.color.a);
				data_len += sprintf(savedata + data_len, "\t\tstage: %d\n", (int)obj.stage);
				data_len += sprintf(savedata + data_len, "\t}\n");
				break;
			case SCENE_RECTANGLE:
				data_len += sprintf(savedata + data_len, "\tRectangle {\n");
				data_len += sprintf(savedata + data_len, "\t\tx: %f\n", obj.x);
				data_len += sprintf(savedata + data_len, "\t\ty: %f\n", obj.y);
				data_len += sprintf(savedata + data_len, "\t\tw: %f\n", obj.w);
				data_len += sprintf(savedata + data_len, "\t\th: %f\n", obj.h);
				data_len += sprintf(savedata + data_len, "\t\trotation: %f\n", obj.rotation);
				data_len += sprintf(savedata + data_len, "\t\tcolor: %d %d %d %d\n", obj.color.r, obj.color.g, obj.color.b, obj.color.a);
				data_len += sprintf(savedata + data_len, "\t\tstage: %d\n", (int)obj.stage);
				data_len += sprintf(savedata + data_len, "\t}\n");
				break;
			case SCENE_CIRCLE:
				data_len += sprintf(savedata + data_len, "\tCircle {\n");
				data_len += sprintf(savedata + data_len, "\t\tx: %f\n", obj.x);
				data_len += sprintf(savedata + data_len, "\t\ty: %f\n", obj.y);
				data_len += sprintf(savedata + data_len, "\t\tradius: %f\n", obj.radius);
				data_len += sprintf(savedata + data_len, "\t\tcolor: %d %d %d %d\n", obj.color.r, obj.color.g, obj.color.b, obj.color.a);
				data_len += sprintf(savedata + data_len, "\t\tstage: %d\n", (int)obj.stage);
				data_len += sprintf(savedata + data_len, "\t}\n");
				break;
			default: break;
		}
	}
	data_len += sprintf(savedata + data_len, "}");
	SaveFile(savedata, strlen(savedata), path);
	EZFREE(savedata);
}

LoadSceneError LoadScene(const char* path) {
	LoadSceneError error = { 0 };
	FILE *file = fopen(path, "r");
	Scene tempscene = { 0 };
    if (file == NULL) {
		error.type = UNABLE_TO_OPEN_FILE;
        return error;
    }

    char line[MAX_LINE_SIZE];
    while (fgets(line, MAX_LINE_SIZE, file)) {
		error.line += 1;
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        } else if (len >= MAX_LINE_SIZE) {
			error.type = OBNOXIOUS_LONG_LINE;
			return error;
		}

		PrunedLine pl = prune_line(line);
		printf("PRUNED LINE:\n");
		printf("\t moniker: \"%s\"\n", pl.moniker);
		printf("\t endbracket: \"%s\"\n", pl.endbracket == 1 ? "YES" : "NO");
		printf("\t startbracket: \"%s\"\n", pl.startbracket == 1 ? "YES" : "NO");
		printf("\t property: \"%s\"\n", pl.property == 1 ? "YES" : "NO");
		for (int i = 0; i < 4; i++) {
			if (pl.valstr[i][0] != '\0') printf("\t valstr %d: \"%s\"\n", i+1, pl.valstr[i]);
		}
		printf("\n");
    }

	fclose(file);
	Scene t = g_scene;
	g_scene = tempscene;
	SaveScene("debug.slumbra");
	g_scene = t;
	return error;
}
