#include "fifo.h"

int enQ(struct Q *q, char c)
{
	if(q->end == q->len - 1 && q->start == 0) return 1;
	if(q->end + 1 == q->start) return 1;
	
	q->buf[q->end++] = c;
	if(q->end == q->len) q->end = 0;
	
	return 0;
}
int deQ(struct Q *q, char *c)
{
	if(q->start == q->end) return 1;
	
	*c = q->buf[q->start++];
	if(q->start == q->len) q->start = 0;
	
	return 0;
}