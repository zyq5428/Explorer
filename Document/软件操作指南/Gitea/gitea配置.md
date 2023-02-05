# Gitea配置

## gitea支持https

- 一、配置nginx：vim nginx/conf.d/herozhou.conf
    - ``` {.line-numbers}
		server {
            listen 443 ssl;
            server_name www.herozhou.top youku.herozhou.top herozhou.top;
            charset utf-8;
            ssl_certificate /etc/letsencrypt/live/herozhou.top/fullchain.pem;
            ssl_certificate_key /etc/letsencrypt/live/herozhou.top/privkey.pem;
            ssl_protocols TLSv1.2;
            ssl_ciphers ECDHE-RSA-AES256-GCM-SHA512:DHE-RSA-AES256-GCM-SHA512:ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-SHA384;
            ssl_ecdh_curve secp384r1;
            ssl_prefer_server_ciphers on;
            ssl_session_cache shared:SSL:10m;
            ssl_session_timeout 10m;
            ssl_session_tickets off;
            keepalive_timeout 70;
            access_log  /var/log/nginx/xxxx.access.log;
            error_log /var/log/nginx/xxx.error.log;
            root /usr/share/nginx/html;
            location / {
                index  index.html;
            }
            location /git/ {
            proxy_redirect off;
            proxy_pass http://127.0.0.1:3000/;
            proxy_http_version 1.1;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection "upgrade";
            proxy_set_header Host $host;
            # Show real IP in v2ray access.log
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            }
        }
		```

- 二、配置api.ini：vim /etc/gitea/app.ini
    - ``` {.line-numbers highlight=[4, 5]}
		[server]
        SSH_DOMAIN       = herozhou.top
        DOMAIN           = herozhou.top
        HTTP_PORT        = 3000
        ROOT_URL         = https://herozhou.top/git/
        DISABLE_SSH      = false
        SSH_PORT         = 26751
        LFS_START_SERVER = true
        LFS_CONTENT_PATH = /var/lib/gitea/data/lfs
        LFS_JWT_SECRET   = ITfso_BbQnFbMzgaRGVI-Vp0dm_JWBEGGfvIc2jQoak
        OFFLINE_MODE     = false
		```

## gitea支持markdown公式
- 一、配置api.ini：vim /etc/gitea/app.ini
    -``` {.line-numbers highlight=[1, 2]}
		[markdown]
        ENABLE_HARD_LINE_BREAK_IN_COMMENTS = false
		```

- 二、配置[Auto-render Extension](https://katex.org/docs/autorender.html)
    - 可以 在文档或另一个延迟脚本中调用renderMathInElement何时（或之后） DOMContentLoaded事件触发。这种方法对于指定或计算选项很有用，或者如果您不想使用defer或onload属性。
    - 编写配置脚本：vim /var/lib/gitea/custom/templates/custom/footer.tmpl
``` {.line-numbers}
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.15.1/dist/katex.min.css" integrity="sha384-R4558gYOUz8mP9YWpZJjofhk+zx0AS11p36HnD2ZKj/6JR5z27gSSULCNHIRReVs" crossorigin="anonymous">
<script defer src="https://cdn.jsdelivr.net/npm/katex@0.15.1/dist/katex.min.js" integrity="sha384-z1fJDqw8ZApjGO3/unPWUPsIymfsJmyrDVWC8Tv/a1HeOtGmkwNd/7xUS0Xcnvsx" crossorigin="anonymous"></script>
<script defer src="https://cdn.jsdelivr.net/npm/katex@0.15.1/dist/contrib/auto-render.min.js" integrity="sha384-+XBljXPPiv+OzfbB3cVmLHf4hdUFHlWNZN5spNQ7rmHTXpd7WvJum6fIACpNNfIR" crossorigin="anonymous"></script>
<script>
    document.addEventListener("DOMContentLoaded", function() {
        renderMathInElement(document.body, {
          // customised options
          // • auto-render specific keys, e.g.:
          delimiters: [
              {left: '$$', right: '$$', display: true},
              {left: '$', right: '$', display: false},
              {left: '\\(', right: '\\)', display: false},
              {left: '\\[', right: '\\]', display: true}
          ],
          // • rendering keys, e.g.:
          throwOnError : false
        });
    });
</script>
```

## 解除上传文件过大的限制

- 增加client_max_body_size 即可
    - 配置nginx.conf：vim /etc/nginx/nginx.conf
    - ``` {.line-numbers}
		http {
            ##
            # Basic Settings
            ##
            sendfile on;
            tcp_nopush on;
            tcp_nodelay on;
            keepalive_timeout 65;
            types_hash_max_size 2048;
            # server_tokens off;
            # server_names_hash_bucket_size 64;
            # server_name_in_redirect off;
            include /etc/nginx/mime.types;
            default_type application/octet-stream;
            client_max_body_size 1024m;
        }
		```

## gitea支持Office DOCX，未成功！！
- 一、配置api.ini：vim /etc/gitea/app.ini
    -``` {.line-numbers highlight=[1, 2]}
        [markup.docx]
        ENABLED = true
        FILE_EXTENSIONS = .docx
        RENDER_COMMAND = "pandoc --from docx --to html --self-contained --template /var/lib/gitea/custom/templates/custom/basic.html"
        [markup.sanitizer.docx.img]
        ALLOW_DATA_URI_IMAGES = true
		```

- 二、安装pandoc：sudo apt-get install pandoc
    - 编写配置脚本：vim /var/lib/gitea/custom/templates/custom/basic.html
``` {.line-numbers}
$body$
```