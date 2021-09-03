# Git服务器的搭建

## 客户端

### 生成SSH公钥

-  首先，你需要确认自己是否已经
拥有密钥。 默认情况下，用户的 SSH 密钥存储在其 ~/.ssh 目录下。 进入该目录并列出其中内容，你便可以快
速确认自己是否已拥有密钥：
	- ``` {.line-numbers}
		cd ~/.ssh
		ls
		```
- 我们需要寻找一对以 id_dsa 或 id_rsa 命名的文件，其中一个带有 .pub 扩展名。 .pub 文件是你的公钥，另
一个则是与之对应的私钥。 如果找不到这样的文件（或者根本没有 .ssh 目录），你可以通过运行 sshkeygen 程序来创建它们。 在 Linux/macOS 系统中，ssh-keygen 随 SSH 软件包提供；在 Windows 上，该程
序包含于 MSysGit 软件包中。
	- ``` {.line-numbers}
		ssh-keygen -o
		```
- 首先 ssh-keygen 会确认密钥的存储位置（默认是 .ssh/id_rsa），然后它会要求你输入两次密钥口令。 如
果你不想在使用密钥时输入口令，将其留空即可。 然而，如果你使用了密码，那么请确保添加了 -o 选项，它会
以比默认格式更能抗暴力破解的格式保存私钥。 你也可以用 ssh-agent 工具来避免每次都要输入密码。
- 现在，进行了上述操作的用户需要将各自的公钥发送给任意一个 Git 服务器管理员 （假设服务器正在使用基于公
钥的 SSH 验证设置）。 他们所要做的就是复制各自的 .pub 文件内容，并将其通过邮件发送。
- 可通过 ssh-copy-id 命令自动完成，这样就不必手动复制并安装公钥了。

``` {.line-numbers}
ssh-copy-id -i .ssh/id_rsa.pub -p 22 git@80.251.222.141
```

## 服务器

### 创建一个操作系统用户 git，并为其建立一个 .ssh 目录

``` {.line-numbers}
$ sudo adduser git
$ su git
$ cd
$ mkdir .ssh && chmod 700 .ssh
$ touch .ssh/authorized_keys && chmod 600 .ssh/authorized_keys
$ cat /tmp/id_rsa.john.pub >> ~/.ssh/authorized_keys
```

### 创建一个空仓库，如果到该项目目录中运行 git init 命令，并加上 --shared 选项， 那么 Git 会自动修改该仓库目录的组权限为可写。

``` {.line-numbers}
git@hero:~$ mkdir hero.git
git@hero:~$ cd hero.git
git@hero:~/hero.git$ ls
git@hero:~/hero.git$ git init --bare --shared
Initialized empty shared Git repository in /home/git/hero.git/
git@hero:~/hero.git$ ls
branches  config  description  HEAD  hooks  info  objects  refs
git@hero:~/hero.git$
```

### 如果已经上传过密钥，则可以通过以下方式克隆仓库。

``` {.line-numbers}
debian@arm:~/git$ git clone ssh://git@herozhou.top:26751/home/git/hero.git
Cloning into 'hero'...
The authenticity of host '[herozhou.top]:26751 ([80.251.222.141]:26751)' can't be established.
ECDSA key fingerprint is SHA256:IBQKJUuuHs55DLgSa58GzY4f2V3SFJs/bFHfglEZn/A.
Are you sure you want to continue connecting (yes/no)? yes
Warning: Permanently added '[herozhou.top]:26751,[80.251.222.141]:26751' (ECDSA) to the list of known hosts.
git@herozhou.top's password:
warning: You appear to have cloned an empty repository.
debian@arm:~/git$ ls
hero
debian@arm:~/git$ cd hero
debian@arm:~/git/hero$ ls -al
total 12
drwxr-xr-x 3 debian debian 4096 Sep  3 11:14 .
drwxr-xr-x 3 debian debian 4096 Sep  3 11:14 ..
drwxr-xr-x 7 debian debian 4096 Sep  3 11:14 .git
debian@arm:~/git/hero$
```

### 如果从本地克隆，则可以通过以下方式克隆仓库。

``` {.line-numbers}
git@hero:~$ git clone /home/git/hero.git
Cloning into 'hero'...
warning: You appear to have cloned an empty repository.
done.
git@hero:~$ ls
hero  hero.git  id_rsa.pub  ihug.git  spider  spider.git
```

### 目前所有（获得授权的）开发者用户都能以系统用户 git 的身份登录服务器从而获得一个普通shell。 如果你想对此加以限制，则需要修改 /etc/passwd 文件中（git 用户所对应）的 shell 值。借助一个名为 git-shell 的受限 shell 工具，你可以方便地将用户 git 的活动限制在与 Git 相关的范围内。 该工具随 Git 软件包一同提供。如果将 git-shell 设置为用户 git 的登录 shell（login shell）， 那么该用户便不能获得此服务器的普通 shell 访问权限。 若要使用 git-shell，需要用它替换掉 bash 或 csh，使其成为该用户的登录 shell。 为进行上述操作，首先你必须确保git-shell 的完整路径名已存在于 /etc/shells文件中：

``` {.line-numbers}
root@hero:~# cat /etc/shells
# /etc/shells: valid login shells
/bin/sh
/bin/dash
/bin/bash
/bin/rbash
root@hero:~# which git-shell
/usr/bin/git-shell
root@hero:~# echo -e '/usr/bin/git-shell' >> /etc/shells
root@hero:~# cat /etc/shells
# /etc/shells: valid login shells
/bin/sh
/bin/dash
/bin/bash
/bin/rbash
/usr/bin/git-shell
root@hero:~#
```

### 现在你可以使用 chsh <username> -s <shell> 命令修改任一系统用户的 shell：
``` {.line-numbers}
sudo chsh git -s $(which git-shell)
```

### 这样，用户 git 就只能利用 SSH 连接对 Git 仓库进行推送和拉取操作，而不能登录机器并取得普通 shell。 如果试图登录，你会发现尝试被拒绝，像这样：

``` {.line-numbers}
$ ssh git@gitserver
fatal: Interactive git shell is not enabled.
hint: ~/git-shell-commands should exist and have read and execute access.
Connection to gitserver closed.
```

### 此时，用户仍可通过 SSH 端口转发来访问任何可达的 git 服务器。 如果你想要避免它，可通过root账户编辑authorized_keys 文件并在所有想要限制的公钥之前添加以下选项：

``` {.line-numbers}
no-port-forwarding,no-X11-forwarding,no-agent-forwarding,no-pty
```

### 其结果如下：

``` {.line-numbers}
$ cat ~/.ssh/authorized_keys
no-port-forwarding,no-X11-forwarding,no-agent-forwarding,no-pty ssh-rsa
AAAAB3NzaC1yc2EAAAADAQABAAABAQCB007n/ww+ouN4gSLKssMxXnBOvf9LGt4LojG6rs6h
PB09j9R/T17/x4lhJA0F3FR1rP6kYBRsWj2aThGw6HXLm9/5zytK6Ztg3RPKK+4kYjh6541N
YsnEAZuXz0jTTyAUfrtU3Z5E003C4oxOj6H0rfIF1kKI9MAQLMdpGW1GYEIgS9EzSdfd8AcC
IicTDWbqLAcU4UpkaX8KyGlLwsNuuGztobF8m72ALC/nLF6JLtPofwFBlgc+myivO7TCUSBd
LQlgMVOFq1I2uPWQOkOWQAHukEOmfjy2jctxSDBQ220ymjaNsHT4kgtZg2AYYgPqdAv8JggJ
ICUvax2T9va5 gsg-keypair
no-port-forwarding,no-X11-forwarding,no-agent-forwarding,no-pty ssh-rsa
AAAAB3NzaC1yc2EAAAADAQABAAABAQDEwENNMomTboYI+LJieaAY16qiXiH3wuvENhBG...
```
