## CR0寄存器

参考网址：[CR0-4寄存器介绍](https://blog.csdn.net/epluguo/article/details/9260429)，[软件调试——IA-32 保护模式下寄存器一览](https://blog.csdn.net/weixin_30896763/article/details/95933122)

![img](https://img-blog.csdn.net/20130706214745890?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvZXBsdWd1bw==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

其中第30位`CD`表示是否禁用Cache，用于控制缓存的开启与关闭；29位`NW`表示是否禁止同时写入内存和缓存，如果关闭则仅写入缓存

文中`CR0_CACHE_DISABLE `宏定义的值为`0x60000000`共计32位，高4位是`0110`，即在29为和30位都设置为1。

> 【Q】如果`NW`用于表示在写入缓存时是否同时写入内存，那么如果不将其设为1会怎样，是否无法计算内存大小？
>
> 这种情况下的`CR0_CACHE_DISABLE `值应该为`0x40000000`，高4位是`0100`，仅仅将30位置1



## EFLAGS寄存器

参考网址：[x86—EFLAGS寄存器详解](https://blog.csdn.net/jn1158359135/article/details/7761011)，[FLAGS register - Wikipedia](https://en.wikipedia.org/wiki/FLAGS_register)

| Intel x86 FLAGS register[[1\]](https://en.wikipedia.org/wiki/FLAGS_register#cite_note-1) |                         |              |                                                              |          |                      |                        |
| :----------------------------------------------------------: | :---------------------: | :----------: | :----------------------------------------------------------: | :------: | :------------------: | :--------------------: |
|                            Bit #                             |          Mask           | Abbreviation |                         Description                          | Category |          =1          |           =0           |
|                            FLAGS                             |                         |              |                                                              |          |                      |                        |
|                              0                               |         0x0001          |      CF      |    [Carry flag](https://en.wikipedia.org/wiki/Carry_flag)    |  Status  |      CY(Carry)       |      NC(No Carry)      |
|                              1                               |         0x0002          |              | Reserved, always 1 in **EFLAGS** [[2\]](https://en.wikipedia.org/wiki/FLAGS_register#cite_note-2)[[3\]](https://en.wikipedia.org/wiki/FLAGS_register#cite_note-r8085-3) |          |                      |                        |
|                              2                               |         0x0004          |      PF      |   [Parity flag](https://en.wikipedia.org/wiki/Parity_flag)   |  Status  |   PE(Parity Even)    |     PO(Parity Odd)     |
|                              3                               |         0x0008          |              | Reserved[[3\]](https://en.wikipedia.org/wiki/FLAGS_register#cite_note-r8085-3) |          |                      |                        |
|                              4                               |         0x0010          |      AF      |   [Adjust flag](https://en.wikipedia.org/wiki/Adjust_flag)   |  Status  | AC(Auxiliary Carry)  | NA(No Auxiliary Carry) |
|                              5                               |         0x0020          |              | Reserved[[3\]](https://en.wikipedia.org/wiki/FLAGS_register#cite_note-r8085-3) |          |                      |                        |
|                              6                               |         0x0040          |      ZF      |     [Zero flag](https://en.wikipedia.org/wiki/Zero_flag)     |  Status  |       ZR(Zero)       |      NZ(Not Zero)      |
|                              7                               |         0x0080          |      SF      |     [Sign flag](https://en.wikipedia.org/wiki/Sign_flag)     |  Status  |     NG(Negative)     |      PL(Positive)      |
|                              8                               |         0x0100          |      TF      | [Trap flag](https://en.wikipedia.org/wiki/Trap_flag) (single step) | Control  |                      |                        |
|                              9                               |         0x0200          |      IF      | [Interrupt enable flag](https://en.wikipedia.org/wiki/IF_(x86_flag)) | Control  | EI(Enable Interrupt) | DI(Disable Interrupt)  |
|                              10                              |         0x0400          |      DF      | [Direction flag](https://en.wikipedia.org/wiki/Direction_flag) | Control  |       DN(Down)       |         UP(Up)         |
|                              11                              |         0x0800          |      OF      | [Overflow flag](https://en.wikipedia.org/wiki/Overflow_flag) |  Status  |     OV(Overflow)     |    NV(Not Overflow)    |
|                            12-13                             |         0x3000          |     IOPL     | [I/O privilege level](https://en.wikipedia.org/wiki/IOPL) (286+ only), always 1[*[clarification needed](https://en.wikipedia.org/wiki/Wikipedia:Please_clarify)*] on 8086 and 186 |  System  |                      |                        |
|                              14                              |         0x4000          |      NT      |    Nested task flag (286+ only), always 1 on 8086 and 186    |  System  |                      |                        |
|                              15                              |         0x8000          |              | Reserved, always 1 on 8086 and 186, always 0 on later models |          |                      |                        |
|                            EFLAGS                            |                         |              |                                                              |          |                      |                        |
|                              16                              |       0x0001 0000       |      RF      | [Resume flag](https://en.wikipedia.org/w/index.php?title=Resume_flag&action=edit&redlink=1) (386+ only) |  System  |                      |                        |
|                              17                              |       0x0002 0000       |      VM      | [Virtual 8086 mode](https://en.wikipedia.org/wiki/Virtual_8086_mode) flag (386+ only) |  System  |                      |                        |
|                              18                              |       0x0004 0000       |      AC      |                Alignment check (486SX+ only)                 |  System  |                      |                        |
|                              19                              |       0x0008 0000       |     VIF      |              Virtual interrupt flag (Pentium+)               |  System  |                      |                        |
|                              20                              |       0x0010 0000       |     VIP      |             Virtual interrupt pending (Pentium+)             |  System  |                      |                        |
|                              21                              |       0x0020 0000       |      ID      |           Able to use CPUID instruction (Pentium+)           |  System  |                      |                        |
|                            22‑31                             |       0xFFC0 0000       |              |                           Reserved                           |  System  |                      |                        |
|                            RFLAGS                            |                         |              |                                                              |          |                      |                        |
|                            32‑63                             | 0xFFFF FFFF… …0000 0000 |              |                           Reserved                           |          |                      |                        |

注：

S: Status flag
C: Control flag
X: System flag

特殊目的寄存器

EIP是指令指针，它指向下一条将要被执行的指令。EIP会被指令RET，RETI，JMP,CALL和INT改变。这个寄存器可能是CPU中最重要的寄存器了，因为他指导着CPU下一条该执行的指令。

段寄存器

在32位系统中，段寄存器通常不会被修改或者显式的使用。Win32使用平面内存寻址模式。这样的原因请看文章《[Windows背景知识之一](http://www.cnblogs.com/awpatp/archive/2009/11/03/1595380.html)》中，《Windows的内存安排》一段的描述。

额外的寄存器

Intel家族中还有一些其他的寄存器，但是它们并不适用于用户态的debugging。简单列出他们如下：

Protected Mode Registers

Control Registers

Debug and Test Registers

Floating Point Unit Register Set

MMX Registers

SIMD Floating Point Registers



其中第18位的AC标志位全称为Alignment Check，只在i486的CPU中值为1



# CPU栈相关操作

参考网址：[pushad和pushfd](http://blog.sina.com.cn/s/blog_af95b18b0101hmx0.html)，[32位微处理器指令系统之堆栈操作指令](https://kknews.cc/zh-cn/code/abnl9jn.amp)

PUSHFD全称为push eflags double word



**堆栈**是在内存RAM中开辟的一段空间，利用“**先进后出**”或“**后进先出**”原则存取数据。

如果把数据压入堆栈，则堆栈指针的值是减少的，即所谓的向下生成堆栈。

由SS:SP（16位）或SS:ESP（32位）指向栈底（栈空）或栈顶（栈不空）地址。

利用数据**入栈指令**PUSH和数据**出栈指令**POP操作堆栈。