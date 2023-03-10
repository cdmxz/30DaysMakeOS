/* マルチタスク関係 */

#include "bootpack.h"

struct TASKCTL* taskctl;
struct TIMER* task_timer;

struct TASK* task_init(struct MEMMAN* memman)
{
	int i;
	struct TASK* task;
	taskctl = (struct TASKCTL*)memman_alloc_4k(memman, sizeof(struct TASKCTL));
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}
	task = task_alloc();
	task->flags = 2; /*ｻ?ｶｯﾖﾐｱ?ﾖｾ*/
	task->priority = 2; /* 0.02ﾃ?*/
	task->level = 0; /*ﾗ?ｸﾟLEVEL */
	task_add(task);
	task_switchsub(); /* LEVEL ﾉ靹ﾃ*/
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);
	return task;

}

struct TASK* task_alloc(void)
{
	int i;
	struct TASK* task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* 使用中マーク */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			task->tss.eax = 0; /* とりあえず0にしておくことにする */
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0; /* もう全部使用中 */
}

void task_run(struct TASK* task, int level, int priority)
{
	if (level < 0) {
		level = task->level; /*ｲｻｸﾄｱ膈EVEL */
	}
	if (priority > 0) {
		task->priority = priority;
	}
	if (task->flags == 2 && task->level != level) { /*ｸﾄｱ莉?ｶｯﾖﾐｵﾄLEVEL */
		task_remove(task); /*ﾕ簑?ﾖｴﾐﾐﾖｮｺ?flagｵﾄﾖｵｻ盂萸ｪ1｣ｬﾓﾚﾊﾇﾏﾂﾃ豬ﾄifﾓ?ｾ菫鰓ｲｻ盂ｻﾖｴﾐﾐ*/
	}
	if (task->flags != 2) {
		/*ｴﾓﾐﾝﾃﾟﾗｴﾌｬｻｽﾐﾑｵﾄﾇ鰔ﾎ*/
		task->level = level;
		task_add(task);
	}
	taskctl->lv_change = 1; /*ﾏﾂｴﾎﾈﾎﾎ?ﾇﾐｻｻﾊｱｼ?ｲ餃EVEL */
	return;
}

void task_switch(void)
{
	struct TASKLEVEL* tl = &taskctl->level[taskctl->now_lv];
	struct TASK* new_task, * now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change != 0) {
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
	return;
}

void task_sleep(struct TASK* task)
{
	struct TASK* now_task;
	if (task->flags == 2) {
		/*ﾈ郢?ｴｦﾓﾚｻ?ｶｯﾗｴﾌｬ*/
		now_task = task_now();
		task_remove(task); /*ﾖｴﾐﾐｴﾋﾓ?ｾ莊ﾄｻｰflagsｽｫｱ萸ｪ1 */
		if (task == now_task) {
			/*ﾈ郢?ﾊﾇﾈﾃﾗﾔｼｺﾐﾝﾃﾟ｣ｬﾔ?ﾐ靨ｪｽ?ﾐﾐﾈﾎﾎ?ﾇﾐｻｻ*/
			task_switchsub();
			now_task = task_now(); /*ﾔﾚﾉ雜ｨｺ?ｻ?ﾈ｡ｵｱﾇｰﾈﾎﾎ?ｵﾄﾖｵ*/
			farjmp(0, now_task->sel);
		}
	}
	return;
}


struct TASK* task_now(void)
{
	struct TASKLEVEL* tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

void task_add(struct TASK* task)
{
	struct TASKLEVEL* tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2; /*ｻ?ｶｯﾖﾐ*/
	return;
}
void task_remove(struct TASK* task)
{
	int i;
	struct TASKLEVEL* tl = &taskctl->level[task->level];
	/*ﾑｰﾕﾒtaskﾋ?ﾔﾚｵﾄﾎｻﾖﾃ*/
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/*ﾔﾚﾕ簑? */
			break;
		}
	}
	tl->running--;
	if (i < tl->now) {
		tl->now--; /*ﾐ靨ｪﾒﾆｶｯｳﾉﾔｱ｣ｬﾒｪﾏ獗ｦｵﾘｴｦﾀ? */
	}
	if (tl->now >= tl->running) {
		/*ﾈ郢?nowｵﾄﾖｵｳ?ﾏﾖﾒ?ｳ｣｣ｬﾔ?ｽ?ﾐﾐﾐﾞﾕ?*/
		tl->now = 0;
	}
	task->flags = 1; /* ﾐﾝﾃﾟﾖﾐ */

	/* ﾒﾆｶｯ */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}
	return;
}

void task_switchsub(void)
{
	int i;
	/*ﾑｰﾕﾒﾗ?ﾉﾏｲ羞ﾄLEVEL */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			break; /*ﾕﾒｵｽﾁﾋ*/
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}
