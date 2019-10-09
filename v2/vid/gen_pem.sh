#!/usr/bin/env bash

echo '\t每个人在使用前都应该使用本脚本生成属于自己的https证书\n' \
     '\tvid-certfile.pem 是certfile， ' \
     'vid-keyfile.pem 是keyfile，分别对应了脚本启动参数\n' \
     '\t证书有效期默认1000天\n' \
     '\t如果要支持https，Common Name应该与域名保持一致。'

openssl req -new -x509 -newkey rsa:2048 -days 1000 -nodes -out vid-certfile.pem -keyout vid-keyfile.pem