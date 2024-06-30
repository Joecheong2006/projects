#include "error.h"
#include <stdio.h>

void log_error(error_type et, char* msg) {
	printf("%s", msg);
	if (et != ErrorNone) {
		printf("error = %d\n", et);
		return;
	}
	printf("success\n");
}
