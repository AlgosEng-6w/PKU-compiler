# 
### Lv1.1记录
### Lv1.2记录
1. Windows和Linux换行符不匹配
> 建议在docker中运行下面命令
> sed -i -e '1s/^\xEF\xBB\xBF//' -e 's/\r$//' /root/src/sysy.l

2. git提交问题，可以设置提交时将 CRLF 转换为 LF，检出时LF 转换为 CRLF
> git config core.autocrlf true

docker run -it --rm -v D:\vscode_code\PKU-compiler:/root maxxing/compiler-dev bash
make
build/compiler -koopa hello.c -o hello.koopa
build/compiler -riscv hello.c -o hello.riscv