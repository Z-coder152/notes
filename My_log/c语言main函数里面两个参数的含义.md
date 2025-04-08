在 C 语言里，int main(int argc, char **argv) 是程序的入口函数，其中的两个参数具备特定含义：

### 1. int argc

- argc 属于整数类型的参数，代表命令行参数的数量。在程序启动时，操作系统会把命令行里的参数数量传递给 argc。

- 这里的参数数量包含程序名自身，所以 argc 的最小值是 1，因为程序名必然会作为一个参数。

### 2. char **argv

- argv 是一个指向字符指针的指针，它本质上是一个字符串数组，数组中的每个元素都是一个指向以空字符 '\0' 结尾的字符串的指针。

- argv[0] 指向的是程序名，后续的元素 argv[1]、argv[2] 等依次指向命令行中输入的其他参数。

下面给出一个简单的示例代码，用来展示这两个参数的使用：

```
#include <stdio.h>

int main(int argc, char **argv) {
    // 输出命令行参数的数量
    printf("命令行参数的数量: %d\n", argc);

    // 输出每个命令行参数
    for (int i = 0; i < argc; i++) {
        printf("参数 %d: %s\n", i, argv[i]);
    }

    return 0;
}
```

在这个示例中：

- 借助 argc 输出命令行参数的数量。

- 利用 for 循环和 argv 输出每个命令行参数。

假如你将上述代码编译成名为 test 的可执行文件，然后在命令行输入 ./test hello world，程序会输出：

```
命令行参数的数量: 3
参数 0: ./test
参数 1: hello
参数 2: world
```

这表明程序名 ./test 是第一个参数，而 hello 和 world 是后续的参数。