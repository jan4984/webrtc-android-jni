根据Android.mk扯过来，修改的Makefile
1.生成动态库：直接在target处写xxxx.so
2.生成静态库：直接在target处写xxxx.a
3.生成可执行程序：务必不要包含.a和.so