#ifndef FIFO_H
#define FIFO_H

struct Q
{
	char *buf;
	int len;
	int start;
	int end;
};

int enQ(struct Q *q, char c);
int deQ(struct Q *q, char *c);

#endif