# libcpp
也是一个造轮子的地方

## 文档结构

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





1. 优化高并发下异步日志的锁争用，比如concurrent hash；解决异常终止时的日志残留问题
2. 添加异步回调接口
3. 基于UDP的server/client
4. http
5. ipc / rpc
6. im即时通讯



# 感谢

1. [Muduo]( https://github.com/chenshuo/muduo )
2. [http-parser]( https://github.com/nodejs/http-parser )
3. [libtnet]( https://github.com/siddontang/libtnet ) 