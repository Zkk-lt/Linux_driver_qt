# Linux_driver_qt

# Qt 环境配置说明（基于 tslib + Qt 5.15.2 交叉编译）

本文档整理了在 Ubuntu 上编译 `tslib` 与 `Qt`，并部署到 ARM 开发板运行的完整流程。

---

## 1. 编译 tslib

### 1.1 下载与解压

- 下载地址：<https://github.com/libts/tslib/tags>
- 示例版本：`tslib-1.21.tar.bz2`

```bash
tar xvf tslib-1.21.tar.bz2
```

### 1.2 安装依赖

```bash
sudo apt-get install autoconf
sudo apt-get install automake
sudo apt-get install libtool
```

### 1.3 配置、编译、安装

进入解压目录后，执行：

```bash
./configure --host=arm-linux-gnueabihf ac_cv_func_malloc_0_nonnull=yes --cache-file=arm-linux.cache --prefix=/path/to/arm-tslib
make
make install
```

> `--prefix` 为自定义的 tslib 安装输出目录（与源码目录名无关），请根据实际路径修改。

---

## 2. 编译 Qt 源码

### 2.1 下载 Qt 5.15.2 源码

- 下载地址：<https://download.qt.io/archive/qt/5.15/5.15.2/single/>
- 文件：`qt-everywhere-src-5.15.2.tar.xz`

```bash
tar -xvf qt-everywhere-src-5.15.2.tar.xz
```

### 2.2 修改 `qmake.conf`

修改文件：`qtbase/mkspecs/linux-arm-gnueabi-g++/qmake.conf`

参考内容如下：

```ini
#
# qmake configuration for building with arm-linux-gnueabi-g++
#

MAKEFILE_GENERATOR      = UNIX
CONFIG                 += incremental
QMAKE_INCREMENTAL_STYLE = sublib

QT_QPA_DEFAULT_PLATFORM = linuxfb
QMAKE_CFLAGS += -O2 -march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard
QMAKE_CXXFLAGS += -O2 -march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard

include(../common/linux.conf)
include(../common/gcc-base-unix.conf)
include(../common/g++-unix.conf)

# modifications to g++.conf
QMAKE_CC                = arm-linux-gnueabihf-gcc
QMAKE_CXX               = arm-linux-gnueabihf-g++
QMAKE_LINK              = arm-linux-gnueabihf-g++
QMAKE_LINK_SHLIB        = arm-linux-gnueabihf-g++

# modifications to linux.conf
QMAKE_AR                = arm-linux-gnueabihf-ar cqs
QMAKE_OBJCOPY           = arm-linux-gnueabihf-objcopy
QMAKE_NM                = arm-linux-gnueabihf-nm -P
QMAKE_STRIP             = arm-linux-gnueabihf-strip
load(qt_config)
```

### 2.3 配置编译选项

在 Qt 源码根目录创建 `autoconfigure.sh`：

```bash
./configure -prefix /path/to/qt-everywhere-src-5.15.2/arm-qt \
-opensource \
-confirm-license \
-release \
-strip \
-shared \
-xplatform linux-arm-gnueabi-g++ \
-optimized-qmake \
-c++std c++11 \
--rpath=no \
-pch \
-skip qt3d \
-skip qtactiveqt \
-skip qtandroidextras \
-skip qtcanvas3d \
-skip qtconnectivity \
-skip qtdatavis3d \
-skip qtdoc \
-skip qtgamepad \
-skip qtlocation \
-skip qtmacextras \
-skip qtnetworkauth \
-skip qtpurchasing \
-skip qtremoteobjects \
-skip qtscript \
-skip qtscxml \
-skip qtsensors \
-skip qtspeech \
-skip qtsvg \
-skip qttools \
-skip qttranslations \
-skip qtwayland \
-skip qtwebengine \
-skip qtwebview \
-skip qtwinextras \
-skip qtx11extras \
-skip qtxmlpatterns \
-make libs \
-make examples \
-nomake tools -nomake tests \
-gui \
-widgets \
-dbus-runtime \
--glib=no \
--iconv=no \
--pcre=qt \
--zlib=qt \
-no-openssl \
--freetype=qt \
--harfbuzz=qt \
-no-opengl \
-linuxfb \
--xcb=no \
-tslib \
--libpng=qt \
--libjpeg=qt \
--sqlite=qt \
-plugin-sql-sqlite \
-I/path/to/arm-tslib/tslib/include \
-L/path/to/arm-tslib/tslib/lib \
-recheck-all
```

需要按实际环境修改以下路径：

1. `-prefix`：Qt 编译输出路径
2. `-I`：tslib 头文件路径
3. `-L`：tslib 库文件路径

常见参数说明：

- `-opensource`：构建开源版本
- `-release`：构建 release 版本
- `-xplatform linux-arm-gnueabi-g++`：指定交叉编译平台
- `-skip qt3d`：跳过指定模块
- `-make libs`：编译库
- `-make examples`：编译示例
- `-nomake tools`：不编译工具
- `-no-openssl`：不使用 OpenSSL

### 2.4 执行编译

先安装 `g++`：

```bash
sudo apt-get install g++
```

执行脚本并编译安装：

```bash
chmod +x autoconfigure.sh
./autoconfigure.sh
make
make install
```

将 `arm-qt` 打包，便于后续部署：

```bash
tar -jcf arm-qt.tar.bz2 arm-qt
```

---

## 3. 开发板配置 Qt 运行环境

### 3.1 拷贝并解压 tslib/Qt

将 `arm-tslib.tar.bz2` 与 `arm-qt.tar.bz2` 拷贝到开发板文件系统（示例为 NFS 根文件系统）：

```bash
cp arm-qt.tar.bz2 ~/myTest/nfs/rootfs/usr/lib
cd ~/myTest/nfs/rootfs/usr/lib/
tar xf arm-qt.tar.bz2
rm arm-qt.tar.bz2
```

`arm-tslib` 的复制解压方式相同。

### 3.2 配置环境变量

编辑 `/etc/profile`，追加：

```bash
export TSLIB_ROOT=/usr/lib/arm-tslib
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_TSDEVICE=/dev/input/event1
export TSLIB_CONFFILE=$TSLIB_ROOT/etc/ts.conf
export TSLIB_PLUGINDIR=$TSLIB_ROOT/lib/ts
export TSLIB_CALIBFILE=/etc/pointercal
export LD_PRELOAD=$TSLIB_ROOT/lib/libts.so

export QT_ROOT=/usr/lib/arm-qt
export QT_QPA_GENERIC_PLUGINS=tslib:/dev/input/event1
export QT_QPA_FONTDIR=/usr/share/fonts
export QT_QPA_PLATFORM_PLUGIN_PATH=$QT_ROOT/plugins
export QT_QPA_PLATFORM=linuxfb:tty=/dev/fb0
export QT_PLUGIN_PATH=$QT_ROOT/plugins
export LD_LIBRARY_PATH=$QT_ROOT/lib:$QT_ROOT/plugins/platforms
export QML2_IMPORT_PATH=$QT_ROOT/qml
export QT_QPA_FB_TSLIB=1
```

使配置立即生效：

```bash
source /etc/profile
```

---

## 4. Qt 运行测试

可直接运行 Qt 自带示例验证环境：

```bash
/usr/lib/arm-qt/examples/widgets/animation/animatedtiles/animatedtiles
```

若程序能正常显示并响应触摸，说明 Qt + tslib 环境配置成功。
