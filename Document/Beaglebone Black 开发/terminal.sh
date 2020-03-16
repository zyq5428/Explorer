#!/bin/bash
#start shadowsocks,and open terminnal ss
sudo sslocal -c /etc/shadowsocks.json -d start
echo "export http_proxy=http://localhost:8123"
echo "export https_proxy=http://localhost:8123"
