## lab8实验记录

简要介绍实验内容：实验要求我们完成一个动态内存分配器，即模拟malloc,free和realloc功能。完成实验需要我们理解内存布局，管理内存的数据结构，并在时间效率和空间效率上进行权衡取舍。

详细实验内容请见[官网](http://csapp.cs.cmu.edu/3e/labs.html)。

### 实验前期准备

#### 材料阅读

1. CSAPP第九章。首先毫无疑问是需要通读CSAPP的第9章虚拟内存，内存布局模型，分配器的要求和目标这些实验原理相关内容都包含在CSAPP第9章中。
2. 系统接口。需要了解sbrk这样直接通过系统调用获得内存的系统接口。
3. 官方提供的实验指导。实验指导[链接](http://csapp.cs.cmu.edu/3e/malloclab.pdf)在此。

#### 测试评估相关

回到实验本身上来，如果是从实验官网上直接下载的`Self-Study Handout`，那么代码框架中所提供的测试文件只有两个测试情形非常简单（测试数据量较小）的测试文件，不足以在统计学意义上评估我们设计的动态内存分配器的性能。建议下载实验[完整的测试文件](https://github.com/Ethan-Yan27/CSAPP-Labs/tree/master/yzf-malloclab-handout/traces)。

将所有测试文件都放在`traces`文件夹中，然后将`traces`文件夹放在`config.h`的同级目录中，在`config.h`中间中添加宏定义`#define TRACEDIR "./traces/"`，之后我们就可以使用`./mdriver -V -v -g`的命令来测试和评估我们设计的内存分配器。

### 基础情况分析



