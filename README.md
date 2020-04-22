## ＯCR 数据合成

利用字体和背景文件合成单字和文本行, 主要解决字符不在字体中的问题，

如果字体中没有对某个字符有明确的定义可能会用一个统一的字形来代替比如下面这样


![](images/2.png)

为了过滤掉这种情况，借鉴(copy) 了 tesseract 的实现

## 依赖安装
```

sudo apt-get install autoconf automake libtool
sudo apt-get install pkg-config
sudo apt-get install libpng-dev
sudo apt-get install libjpeg8-dev
sudo apt-get install libtiff5-dev
sudo apt-get install zlib1g-dev

sudo apt-get install libicu-dev
sudo apt-get install libpango1.0-dev
sudo apt-get install libcairo2-dev

sudo apt-get install libleptonica-dev
sudo apt-get install libboost-all-dev


```

## 使用方法

```$xslt
git clone 
mkdir build 
cd build
cmake ..
make
```

## 合成单字

1. 准备需要渲染的字符集合文本可是存储,一行一个字符
```$xslt
我
能
吞
下
玻
璃
```
2. 准备需要对的字体文件放在一个文件夹下

3. 准备背景图片，最好是没有文字的背景

４.
```
./build/synthword ${CHAR_DIRS} ${FONTS_DIR} ${OUT_DIR} ${BACK_IMAGE_DIR}
```

会对每个字符遍历所有字体，　背景文件只能是 png 格式


## 合成文本行
使用方法和合成单字一样，只是把单字的文本替换成文本行，一行一个文本。
```
./build/synthl ${TEXT_LINE} ${FONTS_DIR} ${OUT_DIR} ${BACK_IMAGE_DIR}
```
合成文本行的同时会生成对应的标注文件，包含每个字的位置信息

效果如下:

![](./images/FZFangSong-Z02.png)

![](./images/Noto%20Sans%20S%20Chinese%20Heavy.png)

![](./images/933ec3fa-c525-49bf-9bc4-3401697cf1d8.png)

![](./images/7c5f2070-fda5-4dcb-828c-ce6e402cddef.png)
