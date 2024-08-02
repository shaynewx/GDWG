#!/bin/bash

# 检查是否提供了文件名
if [ -z "$1" ]; then
    echo "请提供要检查的文件名。"
    exit 1
fi

# 使用 awk 查找包含中文字符的行，并打印行号
awk '/[\x{4E00}-\x{9FFF}]/ {print NR ":" $0}' "$1"
