# The-graduation-design
ubuntu 14.04
C,socket programming
已完成如下功能：
1、server端与client端建立稳定的链接并能交换信息
2、server端接收来自client端的“GET”请求，并根据请求数的增加或减少来自动调整服务进程数
3、server端能设置初始服务进程数和最大服务进程数
4、自动检测端口号是否被占用
5、已经可以满足大量并发请求，用shell写了一个测试方法，结果1000个并发进入的时候，没有引起服务器崩溃。
6、用curses做了简单的界面

