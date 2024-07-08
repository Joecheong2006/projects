#include "todo_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

error_type int_to_str(i8 i, char* str) {
	if (str == NULL) {
		return ErrorInvalidParam;
	}
	int c = 0;
	do {
		int round = i % 10;
		str[c] = round + '0';
		i /= 10;
		c++;
	} while (i != 0);

	str[c] = 0;
	for (int i = 0; i < c / 2; ++i) {
		char temp = str[i];
		str[i] = str[c - i - 1];
		str[c - i - 1] = temp;
	}
	return ErrorNone;
}

static void cd_todo_list(todo_list* tl) {
	char number[3];
	int_to_str(tl->order, number);
	char name[3 + TODO_LIST_NAME_MAX_LEN];
	strcpy(name, tl->name);
	strcat(name, "-");
	strcat(name, number);
	chdir(name);
}

static error_type remove_task(task* t) {
	if (t == NULL) {
		return ErrorInvalidParam;
	}
	char number[3];
	char cmd[TASK_NAME_MAX_LEN + 7];
	strcpy(cmd, "rm ");
	strcat(cmd, t->name);
	strcat(cmd, "-");
	int_to_str(t->order, number);
	strcat(cmd, number);
	system(cmd);
	return ErrorNone;
}

static error_type load_task(task* t, char* file_name) {
	if (file_name == NULL) {
		return ErrorInvalidParam;
	}
	int len = strchr(file_name, '\n') - file_name;
	file_name[len] = 0;
	while (file_name[--len - 1] != '-');
	task result;
	result.order = atoi(file_name + len),
	strncpy(result.name, file_name, len - 1);
	result.name[len - 1] = 0;

	FILE* file = fopen(file_name, "r");
	if (!file) {
	  	return ErrorOpenFile;
	}
	char buf[2];
	fgets(buf, sizeof(buf), file);
	buf[1] = 0;
	fclose(file);

	result.finished = buf[0] - '0';

	*t = result;
	return ErrorNone;
}

static error_type update_task_order(task* t, i8 new_order) {
	if (t == NULL) {
		return ErrorInvalidParam;
	}
	char number[3];

	char cmd[TASK_NAME_MAX_LEN * 2 + 14];
	strcpy(cmd, "mv ");
	strcat(cmd, t->name);
	strcat(cmd, "-");
	int_to_str(t->order, number);
	strcat(cmd, number);
	strcat(cmd, " ");
	strcat(cmd, t->name);
	strcat(cmd, "-");
	int_to_str(new_order, number);
	strcat(cmd, number);
	system(cmd);
	return ErrorNone;
}

error_type init_todo_list(todo_list* result, char* name) {
	if (result == NULL || name == NULL) {
		return ErrorInvalidParam;
	}
	if (strlen(name) > TODO_LIST_NAME_MAX_LEN) {
		return ErrorInvalidStringSize;
	}
	strcpy(result->name, name);
	result->tasks = make_vector();
	result->tasks_finished = 0;
	result->tasks_total = -1;
	return ErrorNone;
}

error_type free_todo_list(todo_list* tl) {
	if (tl == NULL) {
		return ErrorInvalidParam;
	}
	free_vector(&tl->tasks);
	return ErrorNone;
}

error_type todo_list_init_task(todo_list* tl) {
	if (tl == NULL) {
		return ErrorInvalidParam;
	}
	{
		char number[3];
		int_to_str(tl->order, number);
		char cmd[9 + TODO_LIST_NAME_MAX_LEN];
		strcpy(cmd, "mkdir -p ");
		strcat(cmd, tl->name);
		strcat(cmd, "-");
		strcat(cmd, number);
		system(cmd);
	}
	{
		char buf[256];
		cd_todo_list(tl);
		strcpy(buf, "rg --files > .status");
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
		vector_resize(tl->tasks, lines);

		while (fgets(buf, sizeof(buf), file)) {
			task t;
			load_task(&t, buf);
			tl->tasks[t.order] = t;
			tl->tasks_finished += t.finished;
		}
		fclose(file);

		tl->tasks_total = vector_size(tl->tasks);

		chdir("..");
	}
	return ErrorNone;
}

error_type todo_list_add_task(todo_list* tl, task* t) {
	if (tl == NULL || t == NULL) {
		return ErrorInvalidParam;
	}

	t->order = vector_size(tl->tasks);

	char number[3];

	char cmd[15 + TODO_LIST_NAME_MAX_LEN + TASK_NAME_MAX_LEN];
	cd_todo_list(tl);
	strcpy(cmd, "echo 0 > ");
	strcat(cmd, t->name);
	strcat(cmd, "-");
	int_to_str(t->order, number);
	strcat(cmd, number);
	system(cmd);
	chdir("..");

	vector_pushe(tl->tasks, *t);

	return ErrorNone;
}

error_type todo_list_remove_task(todo_list* tl, i8 order) {
	if (tl == NULL || order >= tl->tasks_total || order < 0) {
		return ErrorInvalidParam;
	}

	cd_todo_list(tl);
	remove_task(&tl->tasks[order]);
	for_vector(tl->tasks, i, order + 1) {
		update_task_order(&tl->tasks[i], tl->tasks[i].order - 1);
	}
	chdir("..");
	vector_pop(tl->tasks);
	--tl->tasks_total;
	return ErrorNone;
}

error_type todo_list_swap_task(todo_list* tl, i8 from_order, i8 to_order) {
	if (tl == NULL || from_order >= tl->tasks_total || to_order >= tl->tasks_total || from_order < 0 || to_order < 0) {
		return ErrorInvalidParam;
	}

	if (from_order == to_order) {
		return ErrorNone;
	}
	cd_todo_list(tl);
	error_type et = update_task_order(&tl->tasks[from_order], tl->tasks_total);
	if (et != ErrorNone) {
		chdir("..");
		return et;
	}
	et = update_task_order(&tl->tasks[to_order], from_order);
	if (et != ErrorNone) {
		chdir("..");
		return et;
	}
	int temp_order = tl->tasks[from_order].order;
	tl->tasks[from_order].order = tl->tasks_total;
	et = update_task_order(&tl->tasks[from_order], to_order);
	if (et != ErrorNone) {
		tl->tasks[from_order].order = temp_order;
		chdir("..");
		return et;
	}
	chdir("..");

	tl->tasks[from_order].order = tl->tasks[from_order].order;
	tl->tasks[to_order].order = temp_order;

	return ErrorNone;
}

error_type todo_list_check_task(todo_list* tl, i8 order) {
	if (tl == NULL || order >= tl->tasks_total || order < 0) {
		return ErrorInvalidParam;
	}

	char number[3];
	char cmd[8 + TASK_NAME_MAX_LEN * 2];
	cd_todo_list(tl);
	strcpy(cmd, "echo ");
	int_to_str(!tl->tasks[order].finished, number);
	strcat(cmd, number);
	strcat(cmd, " > ");
	strcat(cmd, tl->tasks[order].name);
	strcat(cmd, "-");
	int_to_str(order, number);
	strcat(cmd, number);
	system(cmd);
	chdir("..");
	return ErrorNone;
}

void log_todo_list(todo_list* tl) {
	printf("[[%s:%d, %d, %d]]\n", tl->name, tl->order, tl->tasks_total, tl->tasks_finished);
	for (int order = 0; order < tl->tasks_total; ++order) {
		log_task(&tl->tasks[order]);
	}
}
