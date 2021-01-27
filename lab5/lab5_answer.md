## Lab5实验记录

简要介绍实验如下，实验分为两个部分,

### Part A

在PartA中我们需要在给定cache的s,E,b的参数的情况下，实现一个LRU策略的cache。

在一开始我们面对实现一个cache可能会觉得存在诸多困难，

记录一个C语言中遇到的bug，和C语言中类型占的位有关。

首先考虑这样一个情况：

```c
long l=1<<32;
```

编译器会报错`警告：左移次数大于或等于类型宽度 [-Wshift-count-overflow]`。

但如果我们确认`sizeof(long)`的值为8,并且`CHAR_BIT`的值也为8时，我们可以确保该环境下long的长度为64,但仍然出现了编译器警告。

原因我们进行移位的1仍然是`int`类型，而绝大多数情况下`int`类型都占32位，所以会发生移位溢出。关于该问题的详细解释可见该问题[warning: left shift count >= width of type](https://stackoverflow.com/questions/4201301/warning-left-shift-count-width-of-type)

故代码应该改为

```c
long l=1L<<32;
```

关于C语言中类型所占的位数额外补充一些：

在C\C++中，**一字节未必是8bits**。

根据C++标准，除了**char必然是1byte**之外，其它都是实现定义的。甚至包括**1 byte是多少bits都是实现定义**的。

对于某一个类型的具体宽度应该这样计算：**`sizeof(long long)*CHAR_BIT`**

普通情况下，在**大众常见的主流pc跟手机架构中，int长度都是稳定的32位。并不是变长**。

反倒对于**long**这个类型**，在现存主流的某些架构中是32位，某些架构是64位**，这会特别麻烦。个人建议是永远**不要**使用**long**这个类型。只使用 int32，int64这样的。

换句话说，byte，short，int 基本上可以认为是8，16，32位，而64位建议使用 int64_t 这样的类型，不要使用long或者long long。

以上来自知乎回答：https://www.zhihu.com/question/376447390/answer/1066440989

稍微总结一下的话就是

涉及到长度敏感的，使用

```c
#include <stdint.h>
```

然后用 uint32_t 这类显式别名。