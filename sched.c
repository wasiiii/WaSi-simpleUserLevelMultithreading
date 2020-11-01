#include "thread.h"
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>

//全局变量，在main.c声明定义
extern struct thread_struct *current;
extern struct thread_struct *task[NR_TASKS];

//在switch.s中定义
void switch_to(struct thread_struct *next);

//获取当前时间
static unsigned int getmstime(){
	struct timeval tv;
	if(gettimeofday(&tv, NULL) < 0){
		perror("gettimeofday");
		exit(-1);
	}
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//选择下一个线程
static struct thread_struct *pick(){
	int i, next, c;
	
	//把阻塞中并且能杯唤醒的线程先唤醒，状态改为就绪READY
	for(i = 0; i < NR_TASKS; i++){
		if(task[i] && task[i]->status == THREAD_SLEEP && getmstime() > task[i]->wakeuptime){
			task[i]->status = THREAD_READY;
		}
	}

	while(1){
		c = -1;
		next = 0;
		//选出时间片最多的就绪线程
		for(i = 0; i < NR_TASKS; i++){
			if(!task[i]) continue;
			if(task[i]->status == THREAD_READY && task[i]->counter > c){
				c = task[i]->counter;
				next = i;
			}
		}
		if(c) break;

		printf("重新调整时间片:  ");
		//如果所有就绪进程的时间片都是0，则重新调整时间片的值
		if(c == 0){
			for(i = 0; i < NR_TASKS; i++){
				if(task[i] && task[i]->status != THREAD_EXIT){
					task[i]->counter = task[i]->priority + (task[i]->counter >> 1);
					if(!i) printf("thread main,%d   ", task[i]->counter);
					else printf("thread %d,%d   ", task[i]->id, task[i]->counter);
				}
			}
			printf("\n");
		}
	}

	//选中的线程状态改为运行RUNNING
	task[next]->status = THREAD_RUNNING;
	return task[next];
}

//调度函数
void schedule(){
	//选择下一个进程
	struct thread_struct *next = pick();
	if(next){
		//进行上下文切换
		switch_to(next);
	}
}



void closealarm(){
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	if(sigprocmask(SIG_BLOCK, &mask, NULL) < 0){
		perror("sigprocamask BLOCK");
	}
}

void openalarm(){
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	if(sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0){
		perror("sigprocamask BLOCK");
	}
}

//线程进入阻塞（规定时间）
void mysleep(int seconds){
	//更新被唤醒时间，为规定时间
	current->wakeuptime = getmstime() + 1000*seconds;
	//状态改为阻塞
	current->status = THREAD_SLEEP;
	//进行调度
	schedule();
}	

//定时器每次到点调用的函数，时间片轮转
static void do_timer(){
	//定时器每次都将当前运行线程的时间片-1
	if(--current->counter > 0) return;
	//当前线程时间片用完，改为阻塞状态
	//如果线程没有运行完，在pick中会统一重新调整时间片
	current->counter = 0;
	current->status = THREAD_SLEEP;
	//进行调度
	schedule();
}

//初始化定时器
void init_timer(){
	struct itimerval value;
	value.it_value.tv_sec = 0;
	//1ms后开始计时
	value.it_value.tv_usec = 1000;
	value.it_interval.tv_sec = 0;
	//每10ms执行一次do_timer
	value.it_interval.tv_usec = 10000;
	if(setitimer(ITIMER_REAL, &value, NULL) < 0){
		perror("setitimer");
	}
	signal(SIGALRM, do_timer);
}

