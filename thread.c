#include "thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include<string.h>

//全局变量，在main.c声明定义
extern struct thread_struct *current;
extern struct thread_struct *task[NR_TASKS];

//定义在sched.c
void schedule();

//初始化init_task，当前运行线程current，和线程数组task
void init(struct thread_struct *init_task, struct thread_struct **current, struct thread_struct **task){
	printf("init thread main\n");
	init_task->id = 0;
	init_task->th_fn = NULL;
	init_task->esp = 0;
	init_task->wakeuptime;
	//线程main一开始必须是RUNNING
	init_task->status = THREAD_RUNNING;
	init_task->counter = 15;
	init_task->priority = 15;
	for(int i = 0; i < STACK_SIZE; i++) init_task->stack[i] = 0;

	*current = init_task;
	task[0] = init_task;
}

//线程开始运行会执行这个函数
void start(struct thread_struct *tsk){
	//执行该线程的过程函数，也就是一个循环打印
	tsk->th_fn();
	//当循环打印完毕，也就是线程完成任务结束了
	//线程状态改为死亡EXIT
	tsk->status = THREAD_EXIT;
	printf("thread %d exited\n", tsk->id);
	//进行调度
	schedule();
	//不在这里进行free，是因为要保证线程main在所有线程结束后才退出，必须要用thread_join
}

//创建进程函数
int thread_create(int *tid, void(*start_routine)()){
	int id = -1;
	//开辟内存
	struct thread_struct *tsk = (struct thread_struct*)malloc(sizeof(struct thread_struct));

	//找一个未使用的线程id
	while(++id < NR_TASKS && task[id]);
	//找不到未使用的线程id
	if(id == NR_TASKS) return -1;
	//把开辟好的内存指针，也就是线程结构体指针放进县城数组
	task[id] = tsk;
	//tid传回去
	if(tid) *tid = id;
	//初始化结构体中的变量
	tsk->id = id;
	tsk->th_fn = start_routine;
	int *stack = tsk->stack;
	tsk->esp = (int)(stack + STACK_SIZE - 11);
	tsk->wakeuptime = 0;
	//这里创建后的线程是就绪状态
	tsk->status = THREAD_READY;
	tsk->counter = 15;
	tsk->priority = 15;

	//7~0是各个寄存器的内容，没有实际意义
	stack[STACK_SIZE - 11] = 7;
	stack[STACK_SIZE - 10] = 6;
	stack[STACK_SIZE - 9] = 5;
	stack[STACK_SIZE - 8] = 4;
	stack[STACK_SIZE - 7] = 3;
	stack[STACK_SIZE - 6] = 2;
	stack[STACK_SIZE - 5] = 1;
	stack[STACK_SIZE - 4] = 0;
	stack[STACK_SIZE - 3] = (int)start;
	stack[STACK_SIZE - 2] = 100;
	stack[STACK_SIZE - 1] = (int)tsk;

	return 0;
}

//保证main函数在其余四个线程结束后再退出
//同时用于释放创建线程是申请的内存
void thread_join(int tid){
	while(task[tid]->status != THREAD_EXIT) schedule();
	free(task[tid]);
	task[tid] = NULL;
}

