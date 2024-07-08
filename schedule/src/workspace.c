#include "workspace.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static error_type update_todo_list_order(todo_list* tl, i32 new_order) {
	if (tl == NULL) {
		return ErrorInvalidParam;
	}
	char number[3];

	char cmd[TODO_LIST_NAME_MAX_LEN * 2 + 14];
	strcpy(cmd, "mv ");
	strcat(cmd, tl->name);
	strcat(cmd, "-");
	int_to_str(tl->order, number);
	strcat(cmd, number);
	strcat(cmd, " ");
	strcat(cmd, tl->name);
	strcat(cmd, "-");
	int_to_str(new_order, number);
	strcat(cmd, number);
	system(cmd);
	return ErrorNone;
}

static i32 have_duplicate_name(workspace* ws, char* name) {
	for_vector(ws->lists, i, 0) {
		if (strcmp(ws->lists[i].name, name) == 0) {
			return 1;
		}
	}
	return 0;
}

static error_type load_list(todo_list* tl, char* name) {
	if (name == NULL) {
		return ErrorInvalidParam;
	}
	int len = strchr(name, '\n') - name;
	while (name[--len - 1] != '-');
	tl->order = atoi(name + len),
	strncpy(tl->name, name, len - 1);
	tl->name[len - 1] = 0;
	init_todo_list(tl, tl->name);
	todo_list_init_task(tl);
	return ErrorNone;
}

static error_type remove_list(todo_list* tl) {
	if (tl == NULL) {
		return ErrorInvalidParam;
	}
	char number[3];
	char cmd[TASK_NAME_MAX_LEN + 9];
	strcpy(cmd, "rm -r ");
	strcat(cmd, tl->name);
	strcat(cmd, "-");
	int_to_str(tl->order, number);
	strcat(cmd, number);
	system(cmd);
	return ErrorNone;
}

static error_type update_list_order(todo_list* tl, i8 new_order) {
	if (tl == NULL) {
		return ErrorInvalidParam;
	}
	char number[3];

	char cmd[TASK_NAME_MAX_LEN * 2 + 14];
	strcpy(cmd, "mv ");
	strcat(cmd, tl->name);
	strcat(cmd, "-");
	int_to_str(tl->order, number);
	strcat(cmd, number);
	strcat(cmd, " ");
	strcat(cmd, tl->name);
	strcat(cmd, "-");
	int_to_str(new_order, number);
	strcat(cmd, number);
	system(cmd);
	return ErrorNone;
}

error_type init_workspace(workspace* ws, char* name) {
	if (ws == NULL || name == NULL) {
		return ErrorInvalidParam;
	}
	strcpy(ws->name, name);
	ws->lists = make_vector();
	return ErrorNone;
}

error_type free_workspace(workspace* ws) {
	if (ws == NULL) {
		return ErrorInvalidParam;
	}
	for_vector(ws->lists, i, 0) {
		free_todo_list(&ws->lists[i]);
	}
	free_vector(&ws->lists);
	return ErrorNone;
}

error_type workspace_init_list(workspace* ws) {
	if (ws == NULL) {
		return ErrorInvalidParam;
	}
	{
		char cmd[9 + TODO_LIST_NAME_MAX_LEN];
		strcpy(cmd, "mkdir -p ");
		strcat(cmd, ws->name);
		system(cmd);
	}
	{
		char buf[256];
		chdir(ws->name);
		strcpy(buf, "find -maxdepth 1 -type d | grep '[^.]' > .status");
		system(buf);
		
		FILE* file = fopen(".status", "r");
		if (!file) {
			return ErrorOpenFile;
		}

		int lines = 0;
		while (fgets(buf, sizeof(buf), file)) {
			++lines;
		}
		fseek(file, 0, SEEK_SET);
		vector_resize(ws->lists, lines);

		while (fgets(buf, sizeof(buf), file)) {
			todo_list sc;
			load_list(&sc, buf + 2);
			ws->lists[sc.order] = sc;
		}
		fclose(file);
		ws->lists_total = lines;

		chdir("..");
	}
	return ErrorNone;
}

error_type workspace_add_list(workspace* ws, todo_list* tl) {
	if (ws == NULL || tl == NULL || have_duplicate_name(ws, tl->name)) {
		return ErrorInvalidParam;
	}

	tl->order = vector_size(ws->lists);

	char number[3];
	int_to_str(tl->order, number);

	chdir(ws->name);
	todo_list_init_task(tl);
	chdir("..");

	vector_pushe(ws->lists, *tl);

	return ErrorNone;
}

error_type workspace_add_task(workspace* ws, i8 list_order, task* t) {
	if (list_order >= ws->lists_total || list_order < 0) {
		return ErrorInvalidParam;
	}
	chdir(ws->name);
	error_type et = todo_list_add_task(&ws->lists[list_order], t);
	if (et != ErrorNone) {
		return et;
	}
	chdir("..");
	return ErrorNone;
}

error_type workspace_remove_list(workspace* ws, i8 order) {
	if (ws == NULL || order >= ws->lists_total || order < 0) {
		return ErrorInvalidParam;
	}

	chdir(ws->name);
	remove_list(&ws->lists[order]);
	for_vector(ws->lists, i, order + 1) {
		update_list_order(&ws->lists[i], ws->lists[i].order - 1);
	}
	chdir("..");
	vector_pop(ws->lists);
	--ws->lists_total;
	return ErrorNone;
}

error_type workspace_remove_task(workspace* ws, i8 list_order, i8 task_order) {
	if (ws == NULL || list_order >= ws->lists_total || list_order < 0) {
		return ErrorInvalidParam;
	}
	chdir(ws->name);
	error_type et = todo_list_remove_task(&ws->lists[list_order], task_order);
	chdir("..");
	return et;
}

error_type workspace_swap_list(workspace* ws, i8 from_order, i8 to_order) {
	if (ws == NULL || from_order >= ws->lists_total || from_order < 0 || to_order >= ws->lists_total || to_order < 0) {
		return ErrorInvalidParam;
	}
	chdir(ws->name);
	error_type et = update_todo_list_order(&ws->lists[to_order], from_order);
	if (et != ErrorNone) {
		chdir("..");
		return et;
	}
	et = update_todo_list_order(&ws->lists[from_order], to_order);
	if (et != ErrorNone) {
		chdir("..");
		return et;
	}
	chdir("..");
	return ErrorNone;
}

error_type workspace_swap_task(workspace* ws, i8 list_order, i8 from_order, i8 to_order) {
	if (ws == NULL || list_order >= ws->lists_total || list_order < 0 || from_order >= ws->lists[list_order].tasks_total || from_order < 0 || to_order >= ws->lists[list_order].tasks_total || to_order < 0) {
		return ErrorInvalidParam;
	}
	chdir(ws->name);
	error_type et = todo_list_swap_task(&ws->lists[list_order], from_order, to_order);
	chdir("..");
	return et;
}

error_type workspace_check_task(workspace* ws, i8 list_order, i8 task_order) {
	if (ws == NULL || list_order >= ws->lists_total || list_order < 0 || task_order >= ws->lists[list_order].tasks_total || task_order < 0) {
		return ErrorInvalidParam;
	}
	chdir(ws->name);
	error_type et = todo_list_check_task(&ws->lists[list_order], task_order);
	chdir("..");
	return et;
}

void log_workspace(workspace* ws) {
	for_vector(ws->lists, i, 0) {
		log_todo_list(&ws->lists[i]);
	}
}

