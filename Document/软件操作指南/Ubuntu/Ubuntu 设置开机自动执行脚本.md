# Ubuntu18.04 不能像16.04 那样可以直接使用 /etc/rc.local 文件，需要设置

## 建立 rc-local.service 文件

``` {.line-numbers highlight=[1]}
hero@hero:~$ sudo vim /etc/systemd/system/rc-local.service
[sudo] password for hero:
```

``` {.line-numbers highlight=[1, 5, 13]}
[Unit]
Description=/etc/rc.local Compatibility
ConditionPathExists=/etc/rc.local

[Service]
Type=forking
ExecStart=/etc/rc.local start
Timeoutsec=0
StandardOutput=tty
RemainAfterExit=yes
SysVStartPriority=99

[Install]
WantedBy=multi-user.target
```

## 创建文件 rc.local

``` {.line-numbers highlight=[1]}
hero@hero:~$ sudo vim /etc/rc.local
[sudo] password for hero:
```

``` {.line-numbers highlight=[1, 13]}
#!/bin/sh -e
#
# rc.local
#
# This script is executed at the end of each multiuser runlevel.
# Make sure that the script will "exit 0" on success or any other
# value on error.
#
# In order to enable or disable this script just change the execution
# bits.
#
# By default this script does nothing.
echo "测试脚本执行成功" > /usr/local/test.log

exit 0
```

> ***注意: 一定要将命令添加在 exit 0之前.***

## 给 rc.local 加上权限，启用服务

``` {.line-numbers highlight=[1-2]}
hero@hero:~$ sudo chmod 755 /etc/rc.local 
hero@hero:~$ sudo systemctl enable rc-local
Created symlink /etc/systemd/system/multi-user.target.wants/rc-local.service → /etc/systemd/system/rc-local.service.
[sudo] password for hero:
```

## 启动服务并检查状态

``` {.line-numbers highlight=[1-2]}
hero@hero:~$ sudo systemctl start rc-local.service
hero@hero:~$ sudo systemctl status rc-local.service
● rc-local.service - /etc/rc.local Compatibility
   Loaded: loaded (/etc/systemd/system/rc-local.service; enabled; vendor preset: enabled)
  Drop-In: /lib/systemd/system/rc-local.service.d
           └─debian.conf
   Active: active (exited) since Mon 2020-01-13 13:44:34 CST; 8s ago
  Process: 4400 ExecStart=/etc/rc.local start (code=exited, status=0/SUCCESS)

1月 13 13:44:34 hero systemd[1]: Starting /etc/rc.local Compatibility...
1月 13 13:44:34 hero systemd[1]: Started /etc/rc.local Compatibility.
hero@hero:~$
```

## 重启后检查test.log文件是否已经存在

``` {.line-numbers highlight=[1]}
hero@hero:~$ cat /usr/local/test.log
测试脚本执行成功
hero@hero:~$
```
