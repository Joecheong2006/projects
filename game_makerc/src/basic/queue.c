#include "queue.h"
#include "memallocate.h"
#include <string.h>
#include <assert.h>

void* make_queue(i64 cap, i64 per_size) {
    char* val = (char*)MALLOC(sizeof(queue_data) + cap * per_size);
    assert(val != NULL);
    if (val == NULL) {
    	return NULL;
    }
    val += (u64)sizeof(queue_data);
    queue_status(val) = (queue_data){ 0, cap, -1, -1 };
    return val;
}

void free_queue(void* que) {
	assert(que != NULL);
	FREE(&queue_status(que));
}

i32 _queue_push(void* que, void* data, i64 per_size) {
    (void)data, (void)per_size;
	queue_data* status = &queue_status(que);
	if (status->size == status->capacity) {
	    return 0;
	}
    status->front = (status->front + 1) % status->capacity;
    status->size++;
    memcpy((char*)que + status->front * per_size, data, per_size);
	if (status->back < 0) {
		status->back = 0;
	}
    return 1;
}

void _queue_pop(void* que) {
	queue_data* status = &queue_status(que);
	if (status->size > 0) {
		status->size--;
		status->back = (status->back + 1) % status->capacity;
	}
	if (status->back == status->front) {
		return;
	}
}

void* _queue_front(void* que, i64 per_size) {
	queue_data* status = &queue_status(que);
	if (status->size == 0) {
		return NULL;
	}
	return (char*)que + status->front * per_size;
}

void* _queue_back(void* que, i64 per_size) {
	queue_data* status = &queue_status(que);
	if (status->size == 0) {
		return NULL;
	}
	return (char*)que + status->back * per_size;
}
