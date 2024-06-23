#include "todo_list.h"
#include "memallocate.h"
#include "vector.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>

static void int_to_str(int i, char* str) {
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
		return ErrorInvaildParam;
	}
	free_vector(&sc->tasks);
	FREE(sc);
	return ErrorNone;
}

static task load_task(char* name) {
	int len = strchr(name, '\n') - name;
	while (name[--len - 1] != '-');
	task result;
	result.order = atoi(name + len),
	strncpy(result.name, name, len - 1);
	result.name[len - 1] = 0;
	return result;
}

error_type todo_list_init_task(todo_list* sc) {
	if (sc == NULL) {
		return ErrorInvaildParam;
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
			task t = load_task(buf);
			printf("%s %d\n", t.name, t.order);
			vector_pushe(sc->tasks, t);
		}
		fclose(file);

		chdir("..");
	}
	return ErrorNone;
}

error_type todo_list_add_task(todo_list* tl, task* t) {
	if (tl == NULL || t == NULL) {
		return ErrorInvaildParam;
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

error_type todo_list_get_task(todo_list* tl, task** result, char* name) {
	if (tl == NULL || name == NULL || result == NULL) {
		return ErrorInvaildParam;
	}
	for_vector(tl->tasks, i, 0) {
		if (strcmp(name, tl->tasks[i].name) == 0) {
			*result = &tl->tasks[i];
			return ErrorNone;
		}
	}
	*result = NULL;
	return ErrorNone;
}
