#!/usr/bin/env bash
extension_text=extension=$1.so
ini_dir=$2/php.ini
if grep -Fxq $extension_text $ini_dir; then
    echo "extension.so already exists in php.ini"
else
    echo "extension.so not found in php.ini. Adding it..."
    echo $extension_text >> $ini_dir
    echo "extension.so added to php.ini"
fi
