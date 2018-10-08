# watchDog-windows
> Windos 版本的看门狗,对指定程序进行守护,如果指定程序未在规定时间内进行喂狗,程序将会程序.

- 程序采用windows服务的方式在后台运行,并开机自动启动
- 可以通过配置文件的方式指定监控程序
- 通过内存共享的方式实现喂狗



## 程序逻辑

由看门狗这边调用win32API`CreateFileMapping`创建一个共享的内存,被看门程序使用win32API`OpenFileMapping`向这块共享的内存定时改变''狗粮'',表示程序正常运行中.下面为狗粮结构体

```c
typedef struct DogFood {
	//程序运行状态
	char status;
	//时间戳
	long timestamp;
}DogFood;
```

程序可以更具status不同,进行一个行为的扩展.

- 喂狗demo可以参考[watchdog-jni](https://github.com/onewe/watchdog-jni)
- 程序运行是必须要确保运行目录下的logs目录存在,以及conf目录的config.json存在



## 如何配置

程序的配置config.json文件在conf目录下面,目前此json很简单

```json
{
  "cmd":"calc"
}
```

修改cmd的值达到监控不同的程序