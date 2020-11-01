/*void switch_to(struct thread_struct* next)*/

.section .text
.global switch_to  /*让其他文件能调用这个方法*/
switch_to:
	call closealarm
	push %ebp		/*push操作是保存现场的过程*/
	mov %esp, %ebp	/*保存ebp寄存器的内容后，就把当前栈顶指针esp也就是函数入口地址放进ebp寄存器*/

	push %edi
	push %esi
	push %ebx
	push %edx
	push %ecx
	push %eax
	pushfl

	mov current, %eax	/*current为当前运行线程结构体的指针*/
	mov %esp, 8(%eax)	/*eax+8，结构体声明时，第三个变量是esp*/
	mov 8(%ebp), %eax	/*ebp在上面更改后已经是函数入口地址，ebp+8是函数参数*/
	mov %eax, current	/*函数参数就是next，选中的下一个线程结构体指针*/
	mov 8(%eax), %esp	/*同样的，结构体指针+8就是变量esp*/

	popfl				/*将esp指向的新堆栈中的内容全部取出，完成线程切换*/
	popl %eax
	popl %edx
	popl %ecx
	popl %ebx
	popl %esi
	popl %edi

	popl %ebp
	call openalarm
	ret
