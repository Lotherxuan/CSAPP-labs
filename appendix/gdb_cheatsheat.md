### gdb的基本使用方法

在完成CSAPP的实验中我们会经常经常用到gdb调试器，特别是在前几个lab中使用得非常频繁。在此总结一下gdb的基本使用方法。

##### 1.编译

在编译阶段，如果是通过gcc/g++命令直接编译，就一定要传递`-g`选项来生成调试信息。例如：`$ gcc -Wall -O2 -g 源文件`。如果使用Makefile构建，一般要给CFLAGS中指定 -g 选项。如： `CFLAGS = -Wall -O2 -g`

##### 2.启动gdb

一般来说我们可以`gdb 可执行文件`的方式来启动gdb,其中gdb读取符号表的过程就发生在此时。

如果我们直接通过`gdb`命令启动，就可以使用命令`file 可执行文件`来使gdb读取符号表。

在gdb读取符号表后即可设置断点，运行程序。

##### 3.设置断点

设置断点有如下一些格式

b <行号>

b <函数名称>

b \*<函数名称>

b \*<代码地址>

b 文件名:行号

b 文件名:函数名

d [编号]

b是break(也可使用break)的简写，设置断点。d是Delete breakpoint的简写，删除指定编号的某个断点，或删除所有断点。断点编号从1开始递增。

##### 4.运行

通过`run`命令使被调试程序开始运行，`run`命令后面可以跟命令行参数，向被调试程序传递命令行参数就是发生在此处。举例如下：

```shell
 (gdb) run -a
```

##### 5.显示帧栈

可以通过`bt`(`backtrace`的简写)，列出调用栈。

完全等价的用法有`info stack`(简写为 `info s`)。

##### 6.程序运行控制

**c**

countinue的缩写，继续执行被调试程序，直至下一个断点或程序结束。

在高级语言(相对于汇编代码)的层面上：

**s**
 执行一行源程序代码，如果此行代码中有函数调用，则进入该函数。相当于其它调试器中的“Step Into (单步跟踪进入)”。
 这个命令必须在有源代码调试信息的情况下才可以使用（GCC编译时使用“-g”参数）。

```undefined
(gdb) s
```

**n**
 执行一行源程序代码，此行代码中的函数调用也一并执行。相当于其它调试器中的“Step Over (单步跟踪)”。
 这个命令必须在有源代码调试信息的情况下才可以使用（GCC编译时使用“-g”参数）。

```undefined
(gdb) n
```



在汇编代码的层面上

**si**
si命令类似于s命令，但针对汇编指令。

```undefined
(gdb) si
```

**ni**
ni命令类似于n命令，但针对汇编指令。

```undefined
(gdb) ni
```

##### 7.改变gdb显示的数据及格式

1. display，设置程序中断后欲显示的数据及其格式。
    例如，如果希望每次程序中断后可以看到即将被执行的下一条汇编指令，可以使用命令`display /i $pc`，其中\$pc 代表当前汇编指令，/i 表示以十六进行显示。当需要关心汇编代码时，此命令相当有用。
    undispaly，取消先前的display设置，编号从1开始递增。

```bash
(gdb) display /i $pc
(gdb) undisplay 1
```

##### 8.显示内存地址

```
(gdb) help x
Examine memory: x/FMT ADDRESS.
ADDRESS is an expression for the memory address to examine.
FMT is a repeat count followed by a format letter and a size letter.
Format letters are o(octal), x(hex), d(decimal), u(unsigned decimal),
  t(binary), f(float), a(address), i(instruction), c(char) and s(string).
  
Size letters are b(byte), h(halfword), w(word), g(giant, 8 bytes).

The specified number of objects of the specified size are printed
according to the format.

Defaults for format and size letters are those previously used.

Default count is 1.  Default address is following last thing printed
with this command or "print".

(gdb) x/6cb 0x804835c //打印地址0x804835c起始的内存内容，连续6个字节，以字符格式输出。
```

举例如下

```assembly
(gdb) x $pc
0x8048ebd <main+173>:   0x0f6ef883
(gdb) x/i $pc     // x/i意为显示汇编指令
0x8048ebd <main+173>:   cmp  $0x6e,%eax
```

##### 9.显示寄存器

`info registers` 可以显示寄存器内容，简写为 `info reg`，或者更为简略的`r g`

i(info的简写)可以显示各种信息，不只是寄存器，详见`help i`

##### 10.显示变量

`print` 命令可以显示变量的值。简写为`p`。

该命令能直接将寄存器作为变量来显示值，用法如`p $rip`。

显示时可以使用以下格式：`p/格式 变量`，其中格式的可选参数参考**8.显示内存地址**中提供的格式字符。

##### 11.监视点

要找到变量在何处被改变，可以使用watch命令

watch <表达式>         //<表达式>发生变化时暂停运行

awatch <表达式>         //<表达式>被访问、改变时暂停运行

rwatch <表达式>          //<表达式>被访问时暂停运行