#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "thread.h"

//初始化线程，也是线程main
struct thread_struct init_task;
//以下两个变量是全局变量
//当前运行的线程
struct thread_struct *current;
//线程数组
struct thread_struct *task[NR_TASKS];
//初始化定时器函数，函数定义在sched.c文件
void init_timer();

//线程一二三都是利用mysleep函数进行主动切换
//线程四是利用时间片轮转调度被动切换
//线程一的过程函数
void fun1(){
	int i = 0;
	while(i <= 100){
		printf("thread 1 已完成%d%s 剩余时间片: %d\n", i, "%", current->counter);
		i += 25;
		mysleep(2);
	}
}
//线程二的过程函数
void fun2(){
	int i = 0;
	while(i <= 100){
		printf("thread 2 已完成%d%s 剩余时间片: %d\n", i, "%", current->counter);
		i += 25;
		mysleep(0.5);
	}
}
//线程三的过程函数
void fun3(){
	int i = 0;
	while(i <= 100){
		printf("thread 3 已完成%d%s 剩余时间片: %d\n", i, "%", current->counter);
		i += 25;
		mysleep(1);
	}
}
//线程四的过程函数
void fun4(){
	int i = 0;
	while(i <= 100){
		printf("thread 4 已完成%d%s 剩余时间片: %d\n", i, "%", current->counter);
		i += 25;
		for(int m = 0; m < 10000; m++)
			for(int n = 0; n < 10000; n++){}
	}
}

int main(){
	//初始化线程main，当前运行线程及线程数组
	init(&init_task, &current, task);
	//初始化定时器
	init_timer();

	//创建四个线程
	int tid1, tid2, tid3, tid4;
	thread_create(&tid1, fun1);
	printf("create thread %d\n", tid1);
	thread_create(&tid2, fun2);
	printf("create thread %d\n", tid2);
	thread_create(&tid3, fun3);
	printf("create thread %d\n", tid3);
	thread_create(&tid4, fun4);
	printf("create thread %d\n", tid4);

	//线程main的过程函数
	int i = 0;
	while(i <= 100){
		printf("thread main 已完成%d%s 剩余时间片: %d\n", i, "%", current->counter);
		i += 50;
		mysleep(1.5);
	}

	//线程main要等待这四个进程退出，才能执行return0退出
	thread_join(tid1);
	thread_join(tid2);
	thread_join(tid3);
	thread_join(tid4);	

	task[0]->status = THREAD_EXIT;
	printf("thread main exited\n");
	return 0;
}
