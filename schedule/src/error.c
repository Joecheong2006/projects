#include "error.h"
#include <stdio.h>

void log_error(error_type et, char* msg) {
	printf("%s ", msg);
	if (et != ErrorNone) {
		printf("error = %d\n", (int)et);
	}
	printf("success\n");
}
