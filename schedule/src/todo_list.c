#include "todo_list.h"
#include "memallocate.h"
#include "vector.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>

static error_type int_to_str(u8 i, char* str) {
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

static error_type sort_todo_list(todo_list* tl) {
	if (tl == NULL) {
		return ErrorInvalidParam;
	}
	for (int i = 0; i < tl->tasks_total; ++i) {
		for (int j = i + 1; j < tl->tasks_total; ++j) {
			if (i == tl->tasks[j].order) {
				task temp = tl->tasks[i];
				tl->tasks[i] = tl->tasks[j];
				tl->tasks[j] = temp;
			}
		}
	}
	return ErrorNone;
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
	while (file_name[--len - 1] != '-');
	task result;
	result.order = atoi(file_name + len),
	strncpy(result.name, file_name, len - 1);
	result.name[len - 1] = 0;
	result.finished = 0;
	*t = result;
	return ErrorNone;
}

static error_type update_task_order(task* t, u8 new_order) {
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

error_type make_todo_list(todo_list** result, char* name) {
	todo_list* out = MALLOC(sizeof(todo_list));
	if (!out) {
		return ErrorMallocFaild;
	}
	if (strlen(name) > TODO_LIST_NAME_MAX_LEN) {
		return ErrorInvalidStringSize;
	}
	strcpy(out->name, name);
	out->tasks = make_vector();
	*result = out;
	return ErrorNone;
}

error_type free_todo_list(todo_list* sc) {
	if (sc == NULL) {
		return ErrorInvalidParam;
	}
	free_vector(&sc->tasks);
	FREE(sc);
	return ErrorNone;
}

error_type todo_list_init_task(todo_list* sc) {
	if (sc == NULL) {
		return ErrorInvalidParam;
	}
	{
		char cmd[9 + TODO_LIST_NAME_MAX_LEN];
		strcpy(cmd, "mkdir -p ");
		strcat(cmd, sc->name);
		system(cmd);
	}
	{
		char buf[256];
		chdir(sc->name);
		// strcpy(buf, "rg --files --null | xargs -0 dirname | sort -u | grep '[^.]' > .status");
		strcpy(buf, "rg --files > .status");
		system(buf);
		
		FILE* file = fopen(".status", "r");
		if (!file) {
			return ErrorOpenFile;
		}
		while(fgets(buf, sizeof(buf), file)) {
			task t;
			load_task(&t, buf);
			vector_pushe(sc->tasks, t);
		}
		fclose(file);

		sc->tasks_total = vector_size(sc->tasks);
		sort_todo_list(sc);

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
	int_to_str(t->order, number);

	char cmd[15 + TODO_LIST_NAME_MAX_LEN + TASK_NAME_MAX_LEN];
	chdir(tl->name);
	strcpy(cmd, "touch ");
	strcat(cmd, t->name);
	strcat(cmd, "-");
	strcat(cmd, number);
	system(cmd);
	chdir("..");

	vector_pushe(tl->tasks, *t);

	return ErrorNone;
}

error_type todo_list_remove_task(todo_list* tl, u8 order) {
	if (tl == NULL || order >= tl->tasks_total) {
		return ErrorInvalidParam;
	}

	chdir(tl->name);
	remove_task(&tl->tasks[order]);
	for_vector(tl->tasks, i, order + 1) {
		update_task_order(&tl->tasks[i], tl->tasks[i].order - 1);
	}
	chdir("..");
	vector_pop(tl->tasks);
	--tl->tasks_total;
	return ErrorNone;
}

error_type todo_list_swap_task(todo_list* tl, u8 from_order, u8 to_order) {
	if (tl == NULL || from_order >= tl->tasks_total || to_order >= tl->tasks_total) {
		return ErrorInvalidParam;
	}

	char number[3];
	char to_name[TASK_NAME_MAX_LEN + 3];
	char from_name[TASK_NAME_MAX_LEN + 3];
	char cmd[(TASK_NAME_MAX_LEN + 3) * 2];

	strcpy(to_name, tl->tasks[to_order].name);
	int_to_str(tl->tasks[to_order].order, number);
	strcat(to_name, "-");
	strcat(to_name, number);

	chdir(tl->name);
	strcpy(cmd, "mv ");
	strcat(cmd, to_name);
	strcat(cmd, " ");
	strcat(cmd, ".temp_task");
	// printf("%s\n", cmd);
	system(cmd);

	strcpy(from_name, tl->tasks[from_order].name);
	int_to_str(tl->tasks[from_order].order, number);
	strcat(from_name, "-");
	strcat(from_name, number);

	strcpy(cmd, "mv ");
	strcat(cmd, from_name);
	strcat(cmd, " ");
	strcat(cmd, to_name);
	system(cmd);
	// printf("%s\n", cmd);

	strcpy(cmd, "mv ");
	strcat(cmd, ".temp_task");
	strcat(cmd, " ");
	strcat(cmd, from_name);
	system(cmd);
	// printf("%s\n", cmd);
	chdir("..");

	int temp_order = tl->tasks[to_order].order;
	tl->tasks[to_order].order = tl->tasks[from_order].order;
	tl->tasks[from_order].order = temp_order;

	return ErrorNone;
}

void log_todo_list(todo_list* tl) {
	printf("[[%s, %d, %d]]\n", tl->name, tl->tasks_finished, tl->tasks_total);
	for (int order = 0; order < tl->tasks_total; ++order) {
		log_task(&tl->tasks[order]);
	}
}
