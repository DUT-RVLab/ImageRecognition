#!/bin/zsh

# 读取图片名称并且创建imagelist.txt
rm imagelist.txt
touch imagelist.txt
for image in `ls images`
	echo images/$image >> imagelist.txt

# 编译demo
g++ $(pkg-config --cflags --libs opencv) demo.cpp -o demo

# 运行demo
time ./demo

# clean up
rm imagelist.txt
rm demo
