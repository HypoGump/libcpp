# libcpp
也是一个造轮子的地方。

本项目是基于 Muduo 的扩展。基本的 Reactor 组件及 TCP C/S 是Muduo 改写的 C++11 版本。 

## 目录结构

```
.
├── docs 说明文档
├── src
│   ├── datetime 日期时间 / 定时器 
│   ├── http     
│   ├── io       io缓冲区等
│   ├── logging
│   ├── net
│   ├── reactor reactor组件
│   ├── thread  
│   └── utils
└── testsuite
```

# 第三方依赖

1. [http-parser]( https://github.com/nodejs/http-parser ) 
2. [ikcp]() 

# 功能

1. 双缓冲异步日志库
2. 多种协议服务器/客户端：TCP / UDP / KCP / HTTP
3. 协程
4. 线程池
5. 信号量 / 读写锁 / 自旋锁

# Feature

1. one loop per thread 模型
2. reactor 模式

# 性能测试



# TODO

1. log 的 backtrace 功能异常
2. KCP C/S 没有缓冲区
3. 优化高并发下异步日志的锁争用，比如concurrent hash；解决异常终止时的日志残留问题
4. 添加异步回调接口
5. 基于UDP的server/client
6. http
7. ipc / rpc
8. KCP 服务端/客户端：适合用协程实现，因为基于 UDP 只有一个 socket。更加暴力的方法是做一个 socket 池，新连接来之后告知对方新的通信端口。目前实现是将处理流程分配到从线程，数据 I/O 还是在主线程。



# 感谢

1. [Muduo]( https://github.com/chenshuo/muduo )
3. [libtnet]( https://github.com/siddontang/libtnet ) 