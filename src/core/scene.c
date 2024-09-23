#include "scene.h"
#include "panels/chain.h"
#include "utils/files.h"
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>


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
		strcpy(pl.valstr[pl.numvals], pl.moniker + 5);
		pl.moniker[5] = '\0';
		pl.numvals++;
	}

	return pl;
}

void draw_scene_object(SceneObject obj) {
	switch (obj.type) {
		case SCENE_GRID:
			if (g_scene.type == SCENE3D) DrawGrid(obj.slices, obj.step);
			else {
				float max = obj.step * obj.slices;
				for (int i = 0; i <= obj.slices / 2; i++) {
					DrawLine((float)i * -1.0f * obj.step, max/2.0f, (float)i * -1.0f * obj.step, max/-2.0f, RAYWHITE);
					DrawLine(max/-2.0f, (float)i * -1.0f * obj.step, max/2.0f, (float)i * -1.0f * obj.step, RAYWHITE);
					DrawLine((float)i * obj.step, max/2.0f, (float)i * obj.step, max/-2.0f, RAYWHITE);
					DrawLine(max/-2.0f, (float)i * obj.step, max/2.0f, (float)i * obj.step, RAYWHITE);
				}
				DrawLine(0, max/2.0f, 0, max/-2.0f, (Color){ 100, 100, 100, 255 });
				DrawLine(max/-2.0f, 0, max/2.0f, 0, (Color){ 100, 100, 100, 255 });
			}
			break;
		case SCENE_CUBE:
			if (g_scene.type == SCENE3D) DrawCube((Vector3){ obj.x, obj.y, obj.z}, obj.w, obj.l, obj.h, obj.color);
			break;
		case SCENE_SPHERE:
			if (g_scene.type == SCENE3D) DrawSphere((Vector3){ obj.x, obj.y, obj.z}, obj.radius, obj.color);
			break;
		case SCENE_RECTANGLE:
			if (g_scene.type == SCENE2D) DrawRectanglePro((Rectangle){ obj.x, obj.y, obj.w, obj.h}, (Vector2){ obj.w / 2.0f, obj.h / 2.0f}, obj.rotation, obj.color);
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
}

void ResetScene() {
	ARRLIST_SceneObject_clear(&g_scene.objects);
	g_scene = (Scene){ 0 };
	ResetSceneCamera();
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

    g_scene.camera2D.offset = (Vector2){ 0.0f, 0.0f };
    g_scene.camera2D.target = (Vector2){ 0.0f, 0.0f };
    g_scene.camera2D.zoom = 10.0f;
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
				data_len += sprintf(savedata + data_len, "\t\tslices: %d\n", (int)obj.slices);
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
	SceneObject tempobj = { 0 };
	int scene_init = 0;
	int adding_obj = 0;
	int done = 0;
    if (file == NULL) {
		error.type = UNABLE_TO_OPEN_FILE;
        return error;
    }
	#define RETERR(err) { error.type = err; fclose(file); return error; }
    char line[MAX_LINE_SIZE];
    while (fgets(line, MAX_LINE_SIZE, file)) {
		error.line += 1;
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        } else if (len >= MAX_LINE_SIZE) {
			RETERR(OBNOXIOUS_LONG_LINE);
		}

		int checkempt_ind = 0;
		int empty_line = 1;
		while(line[checkempt_ind] != '\0' && checkempt_ind < MAX_LINE_SIZE) {
			if (line[checkempt_ind] != ' ' && line[checkempt_ind] != '\t') {
				empty_line = 0;
				break;
			}
			checkempt_ind++;
		}
		if (empty_line == 1) continue;
		if (done == 1) RETERR(OUTSIDE_SCENE_DEF);

		PrunedLine pl = prune_line(line);

		if (scene_init == 0) {
			if (strcmp("SCENE", pl.moniker) == 0) {
				if (pl.numvals != 1)
					RETERR(INVALID_NUM_SCENE_ARGS);
				if (strcmp("3D", pl.valstr[0]) == 0) {
					tempscene.type = SCENE3D;
				} else if (strcmp("2D", pl.valstr[0]) == 0) {
					tempscene.type = SCENE2D;
				} else {
					RETERR(INVALID_SCENE_ARG);
				}
			} else {
				RETERR(NO_START_SCENE_MONIKER);
			}
			scene_init = 1;
		} else if (adding_obj == 0 && strcmp("N/A", pl.moniker) == 0 && pl.endbracket == 1) {
			done = 1;
		} else {
			if (adding_obj == 0) {
				tempobj = (SceneObject){ 0 };
				adding_obj = 1;
				if (strcmp("Cube", pl.moniker) == 0) {
					tempobj.type = SCENE_CUBE;	
				} else if (strcmp("Sphere", pl.moniker) == 0) {
					tempobj.type = SCENE_SPHERE;
				} else if (strcmp("Grid", pl.moniker) == 0) {
					tempobj.type = SCENE_GRID;
				} else if (strcmp("Rectangle", pl.moniker) == 0) {
					tempobj.type = SCENE_RECTANGLE;
				} else if (strcmp("Circle", pl.moniker) == 0) {
					tempobj.type = SCENE_CIRCLE;
				} else {
					RETERR(INVALID_OBJECT_MONIKER);
				}
				if (pl.startbracket != 1) RETERR(NO_START_BRACKET);
				if (pl.numvals != 0) RETERR(INVALID_NUM_OBJECT_ARGS);
				if (pl.endbracket == 1) RETERR(UNALLOWED_END_BRACKET);
				if (pl.property == 1) RETERR(INVALID_PROPERTY);
			} else if (adding_obj == 1) {
				if (pl.endbracket) {
					if (pl.startbracket == 1) RETERR(UNALLOWED_START_BRACKET);
					if (pl.numvals != 0) RETERR(INVALID_NUM_OBJECT_ARGS);
					if (pl.property == 1) RETERR(INVALID_PROPERTY);
					ARRLIST_SceneObject_add(&tempscene.objects, tempobj);
					adding_obj = 0;
				} else if (pl.property) {
					if (pl.startbracket == 1) RETERR(UNALLOWED_START_BRACKET);
					if (pl.endbracket == 1) RETERR(UNALLOWED_END_BRACKET);
					char* endptr;
					#define GETFLOAT(setto, getstr) {errno = 0; setto = strtof(getstr, &endptr); if (errno != 0 || endptr == getstr || *endptr != '\0') RETERR(NOT_A_FLOAT);}
					#define GETINT(setto, getstr) {errno = 0; setto = (size_t)strtol(getstr, &endptr, 10); if (errno != 0 || endptr == getstr || *endptr != '\0') RETERR(NOT_AN_INT);}
					switch (tempobj.type) {
						case SCENE_CUBE:
							if (strcmp("x", pl.moniker)	== 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.x, pl.valstr[0]);
							} else if (strcmp("y", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.y, pl.valstr[0]);
							} else if (strcmp("z", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.z, pl.valstr[0]);
							} else if (strcmp("w", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.w, pl.valstr[0]);
							} else if (strcmp("h", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.h, pl.valstr[0]);
							} else if (strcmp("l", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.l, pl.valstr[0]);
							} else if (strcmp("color", pl.moniker) == 0) {
								if (pl.numvals != 4) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.color.r, pl.valstr[0]);
								GETINT(tempobj.color.g, pl.valstr[1]);
								GETINT(tempobj.color.b, pl.valstr[2]);
								GETINT(tempobj.color.a, pl.valstr[3]);
							} else if (strcmp("stage", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.stage, pl.valstr[0]);
							} else {
								RETERR(INVALID_PROPERTY);
							}
							break;
						case SCENE_SPHERE:
							if (strcmp("x", pl.moniker)	== 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.x, pl.valstr[0]);
							} else if (strcmp("y", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.y, pl.valstr[0]);
							} else if (strcmp("z", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.z, pl.valstr[0]);
							} else if (strcmp("radius", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.radius, pl.valstr[0]);
							} else if (strcmp("color", pl.moniker) == 0) {
								if (pl.numvals != 4) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.color.r, pl.valstr[0]);
								GETINT(tempobj.color.g, pl.valstr[1]);
								GETINT(tempobj.color.b, pl.valstr[2]);
								GETINT(tempobj.color.a, pl.valstr[3]);
							} else if (strcmp("stage", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.stage, pl.valstr[0]);
							} else {
								RETERR(INVALID_PROPERTY);
							}
							break;
						case SCENE_GRID:
							if (strcmp("slices", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.slices, pl.valstr[0]);
							} else if (strcmp("step", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.step, pl.valstr[0]);
							} else if (strcmp("stage", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.stage, pl.valstr[0]);
							} else {
								RETERR(INVALID_PROPERTY);
							}
							break;
						case SCENE_RECTANGLE:
							if (strcmp("x", pl.moniker)	== 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.x, pl.valstr[0]);
							} else if (strcmp("y", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.y, pl.valstr[0]);
							} else if (strcmp("w", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.w, pl.valstr[0]);
							} else if (strcmp("h", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.h, pl.valstr[0]);
							} else if (strcmp("rotation", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.rotation, pl.valstr[0]);
							} else if (strcmp("color", pl.moniker) == 0) {
								if (pl.numvals != 4) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.color.r, pl.valstr[0]);
								GETINT(tempobj.color.g, pl.valstr[1]);
								GETINT(tempobj.color.b, pl.valstr[2]);
								GETINT(tempobj.color.a, pl.valstr[3]);
							} else if (strcmp("stage", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.stage, pl.valstr[0]);
							} else {
								RETERR(INVALID_PROPERTY);
							}
							break;
						case SCENE_CIRCLE:
							if (strcmp("x", pl.moniker)	== 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.x, pl.valstr[0]);
							} else if (strcmp("y", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.y, pl.valstr[0]);
							} else if (strcmp("radius", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETFLOAT(tempobj.radius, pl.valstr[0]);
							} else if (strcmp("color", pl.moniker) == 0) {
								if (pl.numvals != 4) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.color.r, pl.valstr[0]);
								GETINT(tempobj.color.g, pl.valstr[1]);
								GETINT(tempobj.color.b, pl.valstr[2]);
								GETINT(tempobj.color.a, pl.valstr[3]);
							} else if (strcmp("stage", pl.moniker) == 0) {
								if (pl.numvals != 1) RETERR(INVALID_NUM_ARGS);
								GETINT(tempobj.stage, pl.valstr[0]);
							} else {
								RETERR(INVALID_PROPERTY);
							}
							break;
						default:
							RETERR(SHOULD_NEVER_HAPPEN);
					}
					#undef GETFLOAT
				} else {
					RETERR(INVALID_PROPERTY);
				}
			}
		}
    }

	fclose(file);
	ARRLIST_SceneObject_clear(&g_scene.objects);
	g_scene = tempscene;
	ResetSceneCamera();
	return error;
	#undef RETERR
}
