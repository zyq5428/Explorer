# 优雅的提交你的 Git Commit Message

## 起因

- commit message 是开发的日常操作, 写好 log 不仅有助于他人 review, 还可以有效的输出 CHANGELOG, 对项目的管理实际至关重要, 但是实际工作中却常常被大家忽略.希望通过本文, 能够帮助大家重视和规范 commit message 的书写.
- 借助工具，才能够写得即规范，又格式化，还能够支持后续分析。
目前比较建议的是，使用终端工具 commitizen/cz-cli + commitizen/cz-conventional-changelog + conventional-changelog/standard-version 一步解决提交信息和版本发布。
甚至，如果想更狠一点，在持续集成里面加入 marionebl/commitlint 检查 commit 信息是否符合规范，也不是不可以。

## Commit Message 格式

- 目前规范使用较多的是 Angular 团队的规范, 继而衍生了 Conventional Commits specification. 很多工具也是基于此规范, 它的 message 格式如下:

``` {.line-numbers}
<type>(<scope>): <subject>
<BLANK LINE>
<body>
<BLANK LINE>
<footer>
```

- 我们通过 git commit 命令带出的 vim 界面填写的最终结果应该类似如上这个结构, 大致分为三个部分(使用空行分割):
    - 标题行: 必填, 描述主要修改类型和内容
    - 主题内容: 描述为什么修改, 做了什么样的修改, 以及开发的思路等等
    - 页脚注释: 放 Breaking Changes 或 Closed Issues

- 分别由如下部分构成:
    - type: commit 的类型
    - feat: 新特性
    - fix: 修改问题
    - refactor: 代码重构
    - docs: 文档修改
    - style: 代码格式修改, 注意不是 css 修改
    - test: 测试用例修改
    - chore: 其他修改, 比如构建流程, 依赖管理.
    - scope: commit 影响的范围, 比如: route, component, utils, build...
    - subject: commit 的概述, 建议符合 50/72 formatting
    - body: commit 具体修改内容, 可以分为多行, 建议符合 50/72 formatting
    - footer: 一些备注, 通常是 BREAKING CHANGE 或修复的 bug 的链接.

## 个人体验示例

- 如果你只是个人的项目, 或者想尝试一下这样的规范格式, 那么你可以为 git 设置 commit template, 每次 git commit 的时候在 vim 中带出, 时刻提醒自己:
    - 修改 ~/.gitconfig, 添加:
    - ``` {.line-numbers}
        [commit]
        template = ~/.gitmessage
        ```
    - 新建 ~/.gitmessage 内容可以如下:
    - ``` {.line-numbers}
        # head: <type>(<scope>): <subject>
        # - type: feat, fix, docs, style, refactor, test, chore
        # - scope: can be empty (eg. if the change is a global or difficult to assign to a single component)
        # - subject: start with verb (such as 'change'), 50-character line
        #
        # body: 72-character wrapped. This should answer:
        # * Why was this change necessary?
        # * How does it address the problem?
        # * Are there any side effects?
        #
        # footer:
        # - Include a link to the ticket, if any.
        # - BREAKING CHANGE
        #

        # type: commit 的类型
        # feat: 新特性
        # fix: 修改问题
        # refactor: 代码重构
        # docs: 文档修改
        # style: 代码格式修改, 注意不是 css 修改
        # test: 测试用例修改
        # chore: 其他修改, 比如构建流程, 依赖管理.
        # scope: commit 影响的范围, 比如: route, component, utils, build...
        # subject: commit 的概述, 建议符合 50/72 formatting
        # body: commit 具体修改内容, 可以分为多行, 建议符合 50/72 formatting
        # footer: 一些备注, 通常是 BREAKING CHANGE 或修复的 bug 的链接.
        ```

## 工具化规范commit

- debian安装npm：
    - [npm安装指导网址](https://github.com/nodesource/distributions#deb)
    - ``` {.line-numbers}
        # Using Ubuntu
        curl -fsSL https://deb.nodesource.com/setup_17.x | sudo -E bash -
        sudo apt-get install -y nodejs

        # Using Debian, as root
        curl -fsSL https://deb.nodesource.com/setup_17.x | bash -
        apt-get install -y nodejs
        ```

- commitizen/cz-cli, 我们需要借助它提供的 git cz 命令替代我们的 git commit 命令, 帮助我们生成符合规范的 commit message.除此之外, 我们还需要为 commitizen 指定一个 Adapter 比如: cz-conventional-changelog (一个符合 Angular团队规范的 preset). 使得 commitizen 按照我们指定的规范帮助我们生成 commit message.
    - 全局安装
    - ``` {.line-numbers}
        npm install -g commitizen cz-conventional-changelog
        su git
        cd
        pwd
        echo '{ "path": "cz-conventional-changelog" }' > ~/.czrc
        ```

- 如果全局安装过 commitizen, 那么在对应的项目中执行 git cz or npm run commit 都可以.
    - ``` {.line-numbers}
        git add -A
        git cz
        ```
