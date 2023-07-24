# 在中国大陆的网络环境下流畅的使用GitHub

## Windows 11

<以下内容无效>

[编辑"hosts"文件](C:\Windows\System32\drivers\etc\hosts)

在文本文件末尾添加以下内容

```
199.232.69.194 github.global.ssl.fastly.net
140.82.114.4 github.com
```

退出编辑文件并保存

在终端中输入`ipconfig /flushdns`

</以上内容无效>

## 补充说明

实际上，这是一个值得琢磨的问题 
目前我还没有办法解决

但以下参考或许有用
[1](https://gfw.report/publications/usenixsecurity23/zh/) :
这个参考介绍了中国长城防火墙对部分境外流量的阻隔原理，我估计GitHub也在被阻隔之列
