/* FIFO儔僀僽儔儕 */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task)
/* FIFO僶僢僼傽偺弶婜壔 */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* 嬻偒 */
	fifo->flags = 0;
	fifo->p = 0; /* 彂偒崬傒埵抲 */
	fifo->q = 0; /* 撉傒崬傒埵抲 */
	fifo->task = task; /* 僨乕僞偑擖偭偨偲偒偵婲偙偡僞僗僋 */
	return;
}

int fifo32_put(struct FIFO32 *fifo, int data)
/* FIFO傊僨乕僞傪憲傝崬傫偱拁偊傞 */
{
	if (fifo->free == 0) {
		/* 嬻偒偑側偔偰偁傆傟偨 */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	if (fifo->task != 0) {
		if (fifo->task->flags != 2) { /* 僞僗僋偑怮偰偄偨傜 */
			task_run(fifo->task, -1, 0); /* 婲偙偟偰偁偘傞 */
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32 *fifo)
/* FIFO偐傜僨乕僞傪堦偮偲偭偰偔傞 */
{
	int data;
	if (fifo->free == fifo->size) {
		/* 僶僢僼傽偑嬻偭傐偺偲偒偼丄偲傝偁偊偢-1偑曉偝傟傞 */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo32_status(struct FIFO32 *fifo)
/* 偳偺偔傜偄僨乕僞偑棴傑偭偰偄傞偐傪曬崘偡傞 */
{
	return fifo->size - fifo->free;
}
