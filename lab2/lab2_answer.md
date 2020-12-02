## Lab2实验记录

简要介绍实验要求如下，实验要求通过反汇编一个二进制文件来拆除一个二进制炸弹。



首先可以直接把二进制文件反汇编成汇编文件，这样之后在翻阅汇编代码的时候比较方便。

```shell
$ objdump -d bomb > bomb_disas.asm
```



### phase1

```assembly
$ gdb
GNU gdb (GDB) 10.1
Copyright (C) 2020 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-pc-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
(gdb) file ./bomb
Reading symbols from ./bomb...
(gdb) b phase_1
Breakpoint 1 at 0x400ee0
(gdb) r
Starting program: /home/lotherxuan/workspace/CSAPP-labs/lab2/bomb/bomb 
Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
this is a test string.

Breakpoint 1, 0x0000000000400ee0 in phase_1 ()
(gdb) x/10i $rip
=> 0x400ee0 <phase_1>:  		sub    $0x8,%rsp
   0x400ee4 <phase_1+4>:        mov    $0x402400,%esi
   0x400ee9 <phase_1+9>:        call   0x401338 <strings_not_equal>
   0x400eee <phase_1+14>:       test   %eax,%eax
   0x400ef0 <phase_1+16>:       je     0x400ef7 <phase_1+23>
   0x400ef2 <phase_1+18>:       call   0x40143a <explode_bomb>
   0x400ef7 <phase_1+23>:       add    $0x8,%rsp
   0x400efb <phase_1+27>:       ret    
   0x400efc <phase_2>:  push   %rbp
   0x400efd <phase_2+1>:        push   %rbx

```

简要解释上述代码。第1行命令启动调试器。第17行在调试器中执行*bomb*二进制文件。第19行在函数*phase_1()*处设置断点。至于为何要在此处设置断点，我们需要从实验给出的*bomb.c*文件中寻找答案。*bomb.c*源码如下：

```c
/***************************************************************************
 * Dr. Evil's Insidious Bomb, Version 1.1
 * Copyright 2011, Dr. Evil Incorporated. All rights reserved.
 *
 * LICENSE:
 *
 * Dr. Evil Incorporated (the PERPETRATOR) hereby grants you (the
 * VICTIM) explicit permission to use this bomb (the BOMB).  This is a
 * time limited license, which expires on the death of the VICTIM.
 * The PERPETRATOR takes no responsibility for damage, frustration,
 * insanity, bug-eyes, carpal-tunnel syndrome, loss of sleep, or other
 * harm to the VICTIM.  Unless the PERPETRATOR wants to take credit,
 * that is.  The VICTIM may not distribute this bomb source code to
 * any enemies of the PERPETRATOR.  No VICTIM may debug,
 * reverse-engineer, run "strings" on, decompile, decrypt, or use any
 * other technique to gain knowledge of and defuse the BOMB.  BOMB
 * proof clothing may not be worn when handling this program.  The
 * PERPETRATOR will not apologize for the PERPETRATOR's poor sense of
 * humor.  This license is null and void where the BOMB is prohibited
 * by law.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#include "phases.h"

/* 
 * Note to self: Remember to erase this file so my victims will have no
 * idea what is going on, and so they will all blow up in a
 * spectaculary fiendish explosion. -- Dr. Evil 
 */

FILE *infile;

int main(int argc, char *argv[])
{
    char *input;

    /* Note to self: remember to port this bomb to Windows and put a 
     * fantastic GUI on it. */

    /* When run with no arguments, the bomb reads its input lines 
     * from standard input. */
    if (argc == 1) {  
	infile = stdin;
    } 

    /* When run with one argument <file>, the bomb reads from <file> 
     * until EOF, and then switches to standard input. Thus, as you 
     * defuse each phase, you can add its defusing string to <file> and
     * avoid having to retype it. */
    else if (argc == 2) {
	if (!(infile = fopen(argv[1], "r"))) {
	    printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
	    exit(8);
	}
    }

    /* You can't call the bomb with more than 1 command line argument. */
    else {
	printf("Usage: %s [<input_file>]\n", argv[0]);
	exit(8);
    }

    /* Do all sorts of secret stuff that makes the bomb harder to defuse. */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    /* Hmm...  Six phases must be more secure than one phase! */
    input = read_line();             /* Get input                   */
    phase_1(input);                  /* Run the phase               */
    phase_defused();                 /* Drat!  They figured it out!
				      * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");

    /* The second phase is harder.  No one will ever figure out
     * how to defuse this... */
    input = read_line();
    phase_2(input);
    phase_defused();
    printf("That's number 2.  Keep going!\n");

    /* I guess this is too easy so far.  Some more complex code will
     * confuse people. */
    input = read_line();
    phase_3(input);
    phase_defused();
    printf("Halfway there!\n");

    /* Oh yeah?  Well, how good is your math?  Try on this saucy problem! */
    input = read_line();
    phase_4(input);
    phase_defused();
    printf("So you got that one.  Try this one.\n");
    
    /* Round and 'round in memory we go, where we stop, the bomb blows! */
    input = read_line();
    phase_5(input);
    phase_defused();
    printf("Good work!  On to the next...\n");

    /* This phase will never be used, since no one will get past the
     * earlier ones.  But just in case, make this one extra hard. */
    input = read_line();
    phase_6(input);
    phase_defused();

    /* Wow, they got it!  But isn't something... missing?  Perhaps
     * something they overlooked?  Mua ha ha ha ha! */
    
    return 0;
}

```

可以看到，这个源文件引用了*support.h*和*phases.h*，而这两个文件是缺少的，所以*bomb.c*也就无法定义。但*bomb.c*向我们展示了这个文件的主要结构，还有最重要的一点，就是代表各个函数的符号名。这也是我们设置断点的位置所在。



继续回到phase1的实验中。第21行运行到断点处。第28行打印出当前指令计数器中后十条的指令。可以看到该函数的内容大致是将一个立即数作为第一个参数，调用*strings_not_equal*这个函数，然后根据该函数的返回值决定是否引爆炸弹。大胆猜测这个立即数指向了内存中的某个地址，而该地址保存的值正是phase1的答案。

```assembly
(gdb) x/s 0x402400
0x402400:       "Border relations with Canada have never been better."

```

输出结果后可以看到正是一行字符串，紧接着我们单独运行一个新的bomb进程，然后输入这行字符串。

```assembly
$ ./bomb      
Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
Border relations with Canada have never been better.
Phase 1 defused. How about the next one?

```

可以看到我们顺利通过了phase1。

简单回顾phase1,我必须承认我有赌的成分...我并没有完全弄清楚函数*strings_not_equal*的内容（实际上是可以通过汇编代码推测出来的），就直接猜测了他的判定机制，所幸最后成功了。



## phase2

```assembly
(gdb) file ./bomb
Reading symbols from ./bomb...
(gdb) b phase_2
Breakpoint 1 at 0x400efc
(gdb) r
Starting program: /home/lotherxuan/workspace/CSAPP-labs/lab2/bomb/bomb 
Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
Border relations with Canada have never been better.
Phase 1 defused. How about the next one?
debug phase_2
(gdb) x/25i $rip
=> 0x400efc <phase_2>:  		push   %rbp
   0x400efd <phase_2+1>:        push   %rbx
   0x400efe <phase_2+2>:        sub    $0x28,%rsp
   0x400f02 <phase_2+6>:        mov    %rsp,%rsi
   0x400f05 <phase_2+9>:        call   0x40145c <read_six_numbers>
   0x400f0a <phase_2+14>:       cmpl   $0x1,(%rsp)
   0x400f0e <phase_2+18>:       je     0x400f30 <phase_2+52>
   0x400f10 <phase_2+20>:       call   0x40143a <explode_bomb>
   0x400f15 <phase_2+25>:       jmp    0x400f30 <phase_2+52>
   0x400f17 <phase_2+27>:       mov    -0x4(%rbx),%eax
   0x400f1a <phase_2+30>:       add    %eax,%eax
   0x400f1c <phase_2+32>:       cmp    %eax,(%rbx)
   0x400f1e <phase_2+34>:       je     0x400f25 <phase_2+41>
   0x400f20 <phase_2+36>:       call   0x40143a <explode_bomb>
   0x400f25 <phase_2+41>:       add    $0x4,%rbx
   0x400f29 <phase_2+45>:       cmp    %rbp,%rbx
   0x400f2c <phase_2+48>:       jne    0x400f17 <phase_2+27>
   0x400f2e <phase_2+50>:       jmp    0x400f3c <phase_2+64>
   0x400f30 <phase_2+52>:       lea    0x4(%rsp),%rbx
   0x400f35 <phase_2+57>:       lea    0x18(%rsp),%rbp
   0x400f3a <phase_2+62>:       jmp    0x400f17 <phase_2+27>
   0x400f3c <phase_2+64>:       add    $0x28,%rsp
   0x400f40 <phase_2+68>:       pop    %rbx
   0x400f41 <phase_2+69>:       pop    %rbp
   0x400f42 <phase_2+70>:       ret    

```
不同于lab1跌跌撞撞弄出来的，lab2的解体过程我敢说还是弄的比较明白的！大致过程和phase_1一样，我们先展示出phase_2的汇编代码。首先关注16、17行，我们可以看到调用了函数*read_six_number*,并将当前栈指针$rsp的值作为第二个参数传入到函数*read_six_number*中。函数汇编代码如下所示。接下来分析函数*read_six_number*。可以看到该函数中有很多赋值语句，很多寄存器都进行过赋值，我们主要关注第13行调用*sscanf()*前的寄存器状态，设%rsi中的值为x,则函数*sscanf()*的第3~8个参数的值依次为x,x+4,x+8,x+12,x+16,x+20，显然，这六个值正是存储我们输出字符串的地址。也就是说我们将会输入六个数字。返回到函数*phase_2()*中，紧接着是一个循环结构，共循环6次，分别检查%rsp,%rsp+4,%rsp+8,%rsp+12,%rsp+16,%rsp+20上的值是否等于1,2,4,8,16,32。不等于则引爆炸弹，如最终phase2的答案即为1,2,4,8,16,32。

```assembly
(gdb) x/17i 0x40145c
   0x40145c <read_six_numbers>: 		sub    $0x18,%rsp
   0x401460 <read_six_numbers+4>:       mov    %rsi,%rdx
   0x401463 <read_six_numbers+7>:       lea    0x4(%rsi),%rcx
   0x401467 <read_six_numbers+11>:      lea    0x14(%rsi),%rax
   0x40146b <read_six_numbers+15>:      mov    %rax,0x8(%rsp)
   0x401470 <read_six_numbers+20>:      lea    0x10(%rsi),%rax
   0x401474 <read_six_numbers+24>:      mov    %rax,(%rsp)
   0x401478 <read_six_numbers+28>:      lea    0xc(%rsi),%r9
   0x40147c <read_six_numbers+32>:      lea    0x8(%rsi),%r8
   0x401480 <read_six_numbers+36>:      mov    $0x4025c3,%esi
   0x401485 <read_six_numbers+41>:      mov    $0x0,%eax
   0x40148a <read_six_numbers+46>:      call   0x400bf0 <__isoc99_sscanf@plt>
   0x40148f <read_six_numbers+51>:      cmp    $0x5,%eax
   0x401492 <read_six_numbers+54>:      jg     0x401499 <read_six_numbers+61>
   0x401494 <read_six_numbers+56>:      call   0x40143a <explode_bomb>
   0x401499 <read_six_numbers+61>:      add    $0x18,%rsp
   0x40149d <read_six_numbers+65>:      ret 
```

运行结果如下：

```shell
$ ./bomb
Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
Border relations with Canada have never been better.
Phase 1 defused. How about the next one?
1 2 4 8 16 32
That's number 2.  Keep going!
```



### phase3