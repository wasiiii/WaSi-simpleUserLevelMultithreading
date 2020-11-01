#ifndef this
#define this

//最大线程个数
#define NR_TASKS 16
//最大堆栈容量
#define STACK_SIZE 1024

//线程的四个状态
//就绪READY，运行RUNNING，阻塞SLEEP，死亡EXIT
#define THREAD_READY 0
#define THREAD_RUNNING 1
#define THREAD_SLEEP 2
#define THREAD_EXIT 3

//线程结构体
struct thread_struct{
	//线程id
	int id;
	//线程过程函数
	void(*th_fn)();
	//线程的栈顶指针
	int esp;
	//被唤醒时间
	unsigned int wakeuptime;
	//线程状态
	int status;
	//线程的时间片
	int counter;
	//线程的优先级，用于pick函数中，重新调整线程时间片时
	//假如没有priority，时间片为0的线程，调整后还是为0
	int priority;
	//线程可使用的对战
	int stack[STACK_SIZE];
};

//四个函数声明
int thread_create(int *tid, void(*start_routine)());
void thread_join(int tid);
void mysleep(int seconds);
void init(struct thread_struct *init_stack, struct thread_struct **current, struct thread_struct **task);

#endif

