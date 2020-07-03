# 64-github
<h3>
0x01<br>
项目名称：64位加壳框架<br>
功能：可以对64位加壳保护，兼容32位<br>
编译器：vs2019 和 intel XE 2020<br>
编写语言：C++ win32(开发模式：单例模式)<br>
项目模块：两个模块，“加壳框架”窗口程序和“Stub”动态库<br>
<br>
0x02<br>
需要注意的一些问题：<br>
1、编译64位时，请用intel编译器，intel编译器支持x64内联汇编，如果不想使用intel编译器，那么源码中用到的x64内联汇编就要自行处理掉，或者变换为C/C++，或者使用64位纯汇编，或者使用shellcode来替代。<br>
2、编译Stub.dll时，要编译成发布版(Release),关掉SDL检查，关掉安全检查。想要调试程序，Stub.dll也要编译成发布版，调试时候可以把“加壳框架”中的“Stub"引用去掉，然后把Stub项目移除，”加壳框架“只需要加载Stub.dll的发布版即可。<br>
3、Stub的运行库要和加壳框架的运行库一致。<br>
<br>
0x03<br>
加壳流程：<br>
外壳引导段放到Stub动态库中，负责加壳的功能模块由加壳框架的Pack类来承担。<br>
其他说明：<br>
1、这个加壳框架实现了IAT加密，有几个反调试的功能和实现了一个反虚拟的功能。具体参考代码。<br>





