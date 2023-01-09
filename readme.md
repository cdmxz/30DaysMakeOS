# 《30天自制操作系统》源代码

这是我阅读《30天自制操作系统》的抄的源代码，其中对部分源代码里面的注释做出了汉化。

## 运行

1、确保你拥有《30天自制操作系统》的光盘文件

2、clone本项目到光盘文件里的tolset文件夹下（确保30DayMakeOS与z_tools处于同一个文件夹）

3、双击“make_run.bat”或在含有make.bat文件夹下打开cmd，运行make run

## 注意

- 运行day27及之后的代码需要修改：光盘根目录\tolset\z_tools\haribote\haribote.rul
  
  **将**
  
      ../z_tools/haribote/harilibc.lib;
      ../z_tools/haribote/golibc.lib;
  
  **修改为**
  
      ../../../../z_tools/haribote/harilibc.lib;
      ../../../../z_tools/haribote/golibc.lib;

- 运行day27之前的代码
  **修改为**
  
      ../../../z_tools/haribote/harilibc.lib; 
      ../../../z_tools/haribote/golibc.lib;

- 在day29里的standlib文件夹里有部分C标准库函数的实现（抄自中文版644页）

- day29开始把日文显示代码删除，增加了中文显示代码，在cmd运行chklang即可显示中文

中文显示参考：[https://www.cnblogs.com/wunaozai/p/3858473.html]()


