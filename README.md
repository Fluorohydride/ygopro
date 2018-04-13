## YGOPro(Server)
[![Build status](https://ci.appveyor.com/api/projects/status/qgkqi6o0wq7qn922/branch/server?svg=true)](https://ci.appveyor.com/project/zh99998/ygopro/branch/server)
[![Build Status](https://travis-ci.org/moecube/ygopro.svg?branch=server)](https://travis-ci.org/moecube/ygopro)

一个YGOPro的服务端版本，运行后自动建立主机，并开启端口供YGOPro客户端连接。

现用于[萌卡](https://mycard.moe/)和[YGOPro 233服](http://mercury233.me/ygosrv233/)。

### Linux下编译
* 需要以下组件或工具
 * gcc
 * premake5
 * libevent
 * lua5.3
 * sqlite3
* 可参考本项目 [.travis.yml](https://github.com/mycard/ygopro/blob/server/.travis.yml) 中的脚本

### Windows下编译
* 需要以下组件或工具
 * Visual Studio
 * premake5
 * libevent
 * lua5.3
 * sqlite3
* 可参考本项目 [appveyor.yml](https://github.com/mycard/ygopro/blob/server/appveyor.yml) 中的脚本

### 运行
* 使用[ygopro-server](https://github.com/mycard/ygopro-server)运行
* 手动运行的参数是
 * ./ygopro 0 0 0 1 F F F 8000 5 1 180 0
 * 端口（0为随机）
 * 禁卡表编号
 * 卡片允许
 * 决斗模式
 * 旧规则
 * 不检查卡组
 * 不洗切卡组
 * 初始LP
 * 初始手牌数
 * 每回合抽卡
 * 每回合时间
 * 录像保存模式
