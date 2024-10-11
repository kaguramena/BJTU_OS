**使用方法**

1. 执行编译连接
    
    make
2. 运行shell
    
    ./shell

**预览效果**

![终端效果预览](img/shell_preview.png)

**内置命令**

* cd

![cd预览](img/cd_preview.png)

如果只输入'cd',那么将返回$HOME路径。

* ls

![ls预览](img/ls_preview.png)

* echo

![echo预览](img/echo_preview.png)

* mkdir

![mkdir预览](img/mkdir_preview.png)

* pstree

![pstree预览](img/pstree_preview.png)
pstree是一个简易版的实现，距离真实shell端的pstree还有一定区别，但是可以实现具体功能

**外部命令**

所有外部命令都可以直接调用，包括带参数
![外部命令预览](img/exec_preview.png)

**环境变量支持**

支持所有环境变量的解析
![环境变量预览](img/env_preview.png)