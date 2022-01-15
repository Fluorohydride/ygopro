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

### MacOS下编译
* 需要以下组件或工具
 * clang
 * premake5
 * libevent
 * lua5.3
 * sqlite3
* 参考步骤
 * 安装libevent sqlite3
 * 编译premake5
 * wget https://www.lua.org/ftp/lua-5.4.3.tar.gz && tar xf lua-5.4.3.tar.gz && mv lua-5.4.3 lua && cp premake/lua/* lua/
 * ./premake5 gmake --cc=clang
 * cd build && make config=release

### 运行
* 使用[ygopro-server](https://github.com/mycard/ygopro-server)运行
* 手动运行的参数是
 * `./ygopro 0 0 0 1 F F F 8000 5 1 180 0`
 * 端口（0为随机）
 * 禁卡表编号
 * 卡片允许
 * 决斗模式
 * 决斗规则编号
 * 不检查卡组
 * 不洗切卡组
 * 初始LP
 * 初始手牌数
 * 每回合抽卡
 * 每回合时间
 * 特殊选项，按位选择
  * 0x1: 保存录像到 `./replay` 文件夹
  * 0x2: 不向观战者发送录像
