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
   0x400efc <phase_2>:  		push   %rbp
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

```assembly
Breakpoint 1, 0x0000000000400f43 in phase_3 ()
(gdb) x/35i $rip
=> 0x400f43 <phase_3>:  		sub    $0x18,%rsp
   0x400f47 <phase_3+4>:        lea    0xc(%rsp),%rcx
   0x400f4c <phase_3+9>:        lea    0x8(%rsp),%rdx
   0x400f51 <phase_3+14>:       mov    $0x4025cf,%esi
   0x400f56 <phase_3+19>:       mov    $0x0,%eax
   0x400f5b <phase_3+24>:       call   0x400bf0 <__isoc99_sscanf@plt>
   0x400f60 <phase_3+29>:       cmp    $0x1,%eax
   0x400f63 <phase_3+32>:       jg     0x400f6a <phase_3+39>
   0x400f65 <phase_3+34>:       call   0x40143a <explode_bomb>
   0x400f6a <phase_3+39>:       cmpl   $0x7,0x8(%rsp)
   0x400f6f <phase_3+44>:       ja     0x400fad <phase_3+106>
   0x400f71 <phase_3+46>:       mov    0x8(%rsp),%eax
   0x400f75 <phase_3+50>:       jmp    *0x402470(,%rax,8)
   0x400f7c <phase_3+57>:       mov    $0xcf,%eax
   0x400f81 <phase_3+62>:       jmp    0x400fbe <phase_3+123>
   0x400f83 <phase_3+64>:       mov    $0x2c3,%eax
   0x400f88 <phase_3+69>:       jmp    0x400fbe <phase_3+123>
   0x400f8a <phase_3+71>:       mov    $0x100,%eax
   0x400f8f <phase_3+76>:       jmp    0x400fbe <phase_3+123>
   0x400f91 <phase_3+78>:       mov    $0x185,%eax
   0x400f96 <phase_3+83>:       jmp    0x400fbe <phase_3+123>
   0x400f98 <phase_3+85>:       mov    $0xce,%eax
   0x400f9d <phase_3+90>:       jmp    0x400fbe <phase_3+123>
   0x400f9f <phase_3+92>:       mov    $0x2aa,%eax
   0x400fa4 <phase_3+97>:       jmp    0x400fbe <phase_3+123>
   0x400fa6 <phase_3+99>:       mov    $0x147,%eax
   0x400fab <phase_3+104>:      jmp    0x400fbe <phase_3+123>
   0x400fad <phase_3+106>:      call   0x40143a <explode_bomb>
   0x400fb2 <phase_3+111>:      mov    $0x0,%eax
   0x400fb7 <phase_3+116>:      jmp    0x400fbe <phase_3+123>
   0x400fb9 <phase_3+118>:      mov    $0x137,%eax
   0x400fbe <phase_3+123>:      cmp    0xc(%rsp),%eax
   0x400fc2 <phase_3+127>:      je     0x400fc9 <phase_3+134>
   0x400fc4 <phase_3+129>:      call   0x40143a <explode_bomb>
   0x400fc9 <phase_3+134>:      add    $0x18,%rsp
   0x400fcd <phase_3+138>:      ret    
```

同样和前面的phase一样，我们让程序运行到*phase_3*的位置，然后打印出*phase_3*的汇编代码。我们主要关注第15行和第15行下面接着的连续16行代码。可以看到该phase主要考察对*switch*语句的运用和理解。*(%rsp+8)*的值实际上充当了地址的偏移量，指向了*switch*语句中某一个*case*的地址。接着观察12、13行，我们可以看到*(%rsp+8)*的值被限定到不大于7,且是一个无符号数，故取值范围为0~7共8个值。然后我们观察第15行下面接着的连续16行代码，可以看到没两行代码为一组，结构都是给%eax赋值，然后跳转到某一个地址。这也印证了我们先前的猜想。下面我们打印出地址0x402470处的值如下。从打印出来的16进制表示我们可以看到如下特点，首先是每64位为一组，表示一个地址值。每一行存储了两个地址。其次我们从字节顺序也能发现这是小端序的机器。接下来就可以解出phase3了，每一个*(%rsp+8)*的值对应着跳转到下面的某个*case*,也就意味着对应着某个*(%rsp+12)*的值，故最终有8个解。

```assembly
(gdb) x/20x 0x402470
0x402470:       0x00400f7c      0x00000000      0x00400fb9      0x00000000
0x402480:       0x00400f83      0x00000000      0x00400f8a      0x00000000
0x402490:       0x00400f91      0x00000000      0x00400f98      0x00000000
0x4024a0:       0x00400f9f      0x00000000      0x00400fa6      0x00000000

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
0 207
Halfway there!
```



### phase4

```assembly
(gdb) x/22i $rip
=> 0x40100c <phase_4>:  		sub    $0x18,%rsp
   0x401010 <phase_4+4>:        lea    0xc(%rsp),%rcx
   0x401015 <phase_4+9>:        lea    0x8(%rsp),%rdx
   0x40101a <phase_4+14>:       mov    $0x4025cf,%esi
   0x40101f <phase_4+19>:       mov    $0x0,%eax
   0x401024 <phase_4+24>:       call   0x400bf0 <__isoc99_sscanf@plt>
   0x401029 <phase_4+29>:       cmp    $0x2,%eax
   0x40102c <phase_4+32>:       jne    0x401035 <phase_4+41>
   0x40102e <phase_4+34>:       cmpl   $0xe,0x8(%rsp)
   0x401033 <phase_4+39>:       jbe    0x40103a <phase_4+46>
   0x401035 <phase_4+41>:       call   0x40143a <explode_bomb>
   0x40103a <phase_4+46>:       mov    $0xe,%edx
   0x40103f <phase_4+51>:       mov    $0x0,%esi
   0x401044 <phase_4+56>:       mov    0x8(%rsp),%edi
   0x401048 <phase_4+60>:       call   0x400fce <func4>
   0x40104d <phase_4+65>:       test   %eax,%eax
   0x40104f <phase_4+67>:       jne    0x401058 <phase_4+76>
   0x401051 <phase_4+69>:       cmpl   $0x0,0xc(%rsp)
   0x401056 <phase_4+74>:       je     0x40105d <phase_4+81>
   0x401058 <phase_4+76>:       call   0x40143a <explode_bomb>
   0x40105d <phase_4+81>:       add    $0x18,%rsp
   0x401061 <phase_4+85>:       ret
```
同样和前面的phase一样，我们让程序运行到*phase_4*的位置，然后打印出*phase_4*的汇编代码。首先由第3、4行分配的地址以及第5行向*scanf*传递的参数可以看到该phase我们预计将输入2个数字。其中*(%rsp+12)*的值比较好确定。我们直接关注第19、20行，可以直接解出*(%rsp+12)*的值就是0。至于*(%rsp+8)*的值我们可以首先看向第10行，该行限定了*(%rsp+8)*的范围必须小于14。这是还算挺重要的一个信息，我们会在后面用到。接下来的第13、14、15、16行大致是设置传递给函数*func4*的参数，然后调用函数。第17、18行表示要求函数返回值必须为0。然后我们分析函数*func4*的内容。这其实是一个非常奇怪的递归函数。传递给函数的参数有如下特点，第二、三个参数是程序设定好的常数值，第一个参数虽然来自用户输入，但范围也有所限定。只要能进入这个递归函数体的内部，函数最后的返回值一定是0。所以我们输入的第一个数字可以是小于14的任何数字，第二个数字为0。

顺便说一句，一开始我也对在这里放这样一个返回值为常数的函数感觉非常奇怪。但仔细想想从代码健壮性的角度似乎也是可以理解的，考虑到如果把函数递归次数作为程序暴露给外部的接口，则会有可能出现耗尽栈空间等等结果。

```assembly
(gdb) x/22i 0x400fce
   0x400fce <func4>:    sub    $0x8,%rsp
   0x400fd2 <func4+4>:  mov    %edx,%eax
   0x400fd4 <func4+6>:  sub    %esi,%eax
   0x400fd6 <func4+8>:  mov    %eax,%ecx
   0x400fd8 <func4+10>: shr    $0x1f,%ecx
   0x400fdb <func4+13>: add    %ecx,%eax
   0x400fdd <func4+15>: sar    %eax
   0x400fdf <func4+17>: lea    (%rax,%rsi,1),%ecx
   0x400fe2 <func4+20>: cmp    %edi,%ecx
   0x400fe4 <func4+22>: jle    0x400ff2 <func4+36>
   0x400fe6 <func4+24>: lea    -0x1(%rcx),%edx
   0x400fe9 <func4+27>: call   0x400fce <func4>
   0x400fee <func4+32>: add    %eax,%eax
   0x400ff0 <func4+34>: jmp    0x401007 <func4+57>
   0x400ff2 <func4+36>: mov    $0x0,%eax
   0x400ff7 <func4+41>: cmp    %edi,%ecx
   0x400ff9 <func4+43>: jge    0x401007 <func4+57>
   0x400ffb <func4+45>: lea    0x1(%rcx),%esi
   0x400ffe <func4+48>: call   0x400fce <func4>
   0x401003 <func4+53>: lea    0x1(%rax,%rax,1),%eax
   0x401007 <func4+57>: add    $0x8,%rsp
   0x40100b <func4+61>: ret
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
0 207
Halfway there!
3 0
So you got that one.  Try this one.

```



### phase5

```assembly
Breakpoint 1, 0x0000000000401062 in phase_5 ()
(gdb) x/40i $rip
=> 0x401062 <phase_5>:  		push   %rbx   
   0x401063 <phase_5+1>:        sub    $0x20,%rsp  //分配32字节的栈空间
   0x401067 <phase_5+5>:        mov    %rdi,%rbx   //%rdi和%rbx的值即为我们输入字符串的首地址
   0x40106a <phase_5+8>:        mov    %fs:0x28,%rax   
   0x401073 <phase_5+17>:       mov    %rax,0x18(%rsp)   //该行和上一行设置金丝雀值，防止缓冲区溢出攻击
   0x401078 <phase_5+22>:       xor    %eax,%eax
   0x40107a <phase_5+24>:       call   0x40131b <string_length>
   0x40107f <phase_5+29>:       cmp    $0x6,%eax    //字符串长度不为6时引爆炸弹
   0x401082 <phase_5+32>:       je     0x4010d2 <phase_5+112>
   0x401084 <phase_5+34>:       call   0x40143a <explode_bomb>
   0x401089 <phase_5+39>:       jmp    0x4010d2 <phase_5+112>
   0x40108b <phase_5+41>:       movzbl (%rbx,%rax,1),%ecx  //%ecx的值即为输入字符串以%rax作为偏移量的字符的值
   0x40108f <phase_5+45>:       mov    %cl,(%rsp)
   0x401092 <phase_5+48>:       mov    (%rsp),%rdx    
   0x401096 <phase_5+52>:       and    $0xf,%edx   //%ecx的低四位作为%edx的值
   0x401099 <phase_5+55>:       movzbl 0x4024b0(%rdx),%edx  //%edx作为偏移量，取得以0x4024b0作为字符串起始地址的某字符的值
   0x4010a0 <phase_5+62>:       mov    %dl,0x10(%rsp,%rax,1)  //为以%rsp+0x10为字符串起始地址，%rax为偏移量的字符赋值
   0x4010a4 <phase_5+66>:       add    $0x1,%rax
   0x4010a8 <phase_5+70>:       cmp    $0x6,%rax   //循环重复6次
   0x4010ac <phase_5+74>:       jne    0x40108b <phase_5+41>
   0x4010ae <phase_5+76>:       movb   $0x0,0x16(%rsp)
   0x4010b3 <phase_5+81>:       mov    $0x40245e,%esi
   0x4010b8 <phase_5+86>:       lea    0x10(%rsp),%rdi
   0x4010bd <phase_5+91>:       call   0x401338 <strings_not_equal>  //比较字符串是否相等
   0x4010c2 <phase_5+96>:       test   %eax,%eax
   0x4010c4 <phase_5+98>:       je     0x4010d9 <phase_5+119>
   0x4010c6 <phase_5+100>:      call   0x40143a <explode_bomb>
   0x4010cb <phase_5+105>:      nopl   0x0(%rax,%rax,1)
   0x4010d0 <phase_5+110>:      jmp    0x4010d9 <phase_5+119>
   0x4010d2 <phase_5+112>:      mov    $0x0,%eax
   0x4010d7 <phase_5+117>:      jmp    0x40108b <phase_5+41>
   0x4010d9 <phase_5+119>:      mov    0x18(%rsp),%rax
   0x4010de <phase_5+124>:      xor    %fs:0x28,%rax   //取出金丝雀值
   0x4010e7 <phase_5+133>:      je     0x4010ee <phase_5+140>
   0x4010e9 <phase_5+135>:      call   0x400b30 <__stack_chk_fail@plt>  //检查栈是否被破坏
   0x4010ee <phase_5+140>:      add    $0x20,%rsp  //释放栈空间
   0x4010f2 <phase_5+144>:      pop    %rbx
   0x4010f3 <phase_5+145>:      ret   
```

同样和前面的phase一样，我们让程序运行到*phase_5*的位置，然后打印出*phase_5*的汇编代码。由于该段汇编代码比较长，会通过注释的形式辅助讲解这段代码。

首先看到第26行，该行算是核心检测条件。我们把0x10(%rsp)和常数0x40245e这两个字符串的起始地址作为函数的两个参数送入字符串比较函数，根据两个字符串是否相等来决定是否引爆炸弹。第二个参数是已知参数，所以我们可以直接查看字符串内容，如下所示。
```shell
(gdb) x/s 0x40245e
0x40245e:       "flyers"
```
接着我们考虑0x10(%rsp)的值，显然这是栈上我们分配的空间，其字符串的具体内容一定是在函数体内部赋值的。在第14行到第22行的循环体内部，我们可以看到是在第18行进行字符串赋值的。顺便我们也可以观察循环结构，循环共执行6次，依次为以0x10(%rsp)为首地址的字符串的0~5的偏移量上的字符赋值。第18行我们可以值的来源是以内存地址0x4024b0为基址，%edx为偏移量来获取的。这样我们可以接着查看0x4024b0处字符串的内容，如下所示。
```shell
(gdb) x/s 0x4024b0
0x4024b0 <array.3449>:  "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"
```
这个字符串可以看到偏移量0~15处都是字母，后面则是一串自然语言。这显然不是偶然，我们前面提到偏移量是%edx的值，从第14到第17行我们可以看到%edx的值实际上是%ecx的低四位，所以取值范围为0~15。也就是说虽然这串字符很长，但我们只取得到偏移量为0~15的字符，大概就像密码表一样，后面那句自然语言基本就是彩蛋了。对照密码表我们可以写出偏移量，偏移量用16进制表示（待会儿会看到用途）。如下表所示

| f    | l    | y    | e    | r    | s    |
| ---- | ---- | ---- | ---- | ---- | ---- |
| 0x9  | 0xf  | 0xe  | 0x5  | 0x6  | 0x7  |

接下来我们看到第14行，可以看到偏移量的来源正是我们输入字符串的低四位的值。ASCII码一共有8位，我们只需在可显示字符中找到低四位和偏移量相等的即可，我选取的6个字符如下所示
| 9    | ?    | >    | 5    | 6    | 7    |
| ---- | ---- | ---- | ---- | ---- | ---- |
| 0x39 | 0x3f | 0x3e | 0x35 | 0x36 | 0x37 |

运行结果如下：

```shell
$ ./bomb
Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
Border relations with Canada have never been better.
Phase 1 defused. How about the next one?
1 2 4 8 16 32
That's number 2.  Keep going!
0 207
Halfway there!
3 0
So you got that one.  Try this one.
9?>567
Good work!  On to the next...
```

### phase6

```

```

