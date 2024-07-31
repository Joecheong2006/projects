#ifndef _QUEUE_H
#define _QUEUE_H
#include "util.h"

typedef struct {
	i64 size, capacity;
	i32 front, back;
} queue_data;

#define queue(T) T*

#define queue_status(que)\
    (((queue_data*)(que))[-1])

#define queue_is_empty(que)\
	(queue_status(que).size == 0)

#define queue_is_full(que)\
	(queue_status(que).size == queue_status(que).capacity)

#define queue_push(que, ...)\
    _queue_push(que, &(__typeof__(*(que))){__VA_ARGS__}, sizeof(__typeof__(*(que))))

#define queue_pop(que)\
	_queue_pop(que)

#define queue_front(que)\
	_queue_front(que, sizeof(*(que)))

#define queue_back(que)\
	_queue_back(que, sizeof(*(que)))

void* make_queue(i64 cap, i64 per_size);
void free_queue(void* que);

i32 _queue_push(void* que, void* data, i64 per_size);
void _queue_pop(void* que);
void* _queue_front(void* que, i64 per_size);
void* _queue_back(void* que, i64 per_size);

#endif
