# MainforecastSystem
使用百度Easydl结构化数据API实现的，超声数据-拉力值预测软件，使用HTTPs协议。

# 引言
本软件基于单一超声探头的检测数据与相应的拉力值，构建和训练了深度神经网络模型。该软件能够根据实际检测到的超声数据，预测管道在相应位置的性能。为了减轻软件在运行时对设备的负担，软件通过HTTP协议与服务器进行通信。服务器充当运算单元，执行人工智能任务。因此，使用本软件需要联网，如图1所示，当断网情况下，无法正常使用。
 
![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/a1292edc-c9de-4278-823b-558a43990903)

# 软件概述
如图2所示，为本软件的用户界面。菜单栏包含文件、设置和帮助选项。在文件选项中，用户可以新建相应的数据表格，并进行保存，如图3~4所示。通过打开选项，用户能够通过浏览文件导入已整理好的数据。此外，导出功能可用于在完成推测后导出结果折线图和热力图。
 
![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/e494f7da-e002-42f9-a56c-6a1366976f2a)

![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/b4772140-940d-44ee-94f2-73ee3f11b6df)

![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/dbc8d217-9ebc-48b4-abee-a110e211bf59)

设置目前支持对于推测功能速度的调节，速度越高效率越高，速度越慢稳定性越好，如图5所示。帮助选框包含了一些操作与数据处理提示。
 
![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/639ddc0f-bcc7-4bea-bd5c-ae10d56edf64)

软件主界面包括左上角推断按键，不论是单次推测、多次推测、整体推测均通过点击该按钮实现。在右侧输入对应的基准敏感度、耦合灵敏度、波幅、dB值即可进行单次推测。而需要连续多次推测时，可通过浏览按键，打开csv文件或者txt文件。数据格式要求如表1~2所示，导入文件界面如图6所示。

基准灵敏度	耦合灵敏度	波幅	dB值
表1 多次推测数据格式
![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/a616e8de-49ac-40e9-9add-b8d2dc0cd7ac)

X轴坐标	Y轴坐标	基准灵敏度	耦合灵敏度	波幅	dB值
表2 整体推测数据格式
![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/6d17e6d1-c366-4609-acbe-b05b4fd182a3)

![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/918e956f-cb48-4b84-af4f-570d1bcb684c)

第三章 实验效果
单次推测时，会将结果输出到结果栏中，可以通过不断输入来进行多次推测，如图7所示。
 
![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/568f13e1-dfed-4642-99d6-7eea56328e5c)

多次推测时，主要使用场景为一条测试样本的所有位置拉力值预测，因此在产生对应结果的同时，也会生成对应的折线图，该折线图可以通过菜单栏-文件-导出的流程进行输出，如图8所示。
 
![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/7fa46d65-dc2d-4356-bc6a-2efdca89490f)

而对于整体预测，为了更直观看出整管/板的性能，因此对应生成了热力图。其中当预测值不满足要求时，会用红色记号标明。同时鼠标放置于热力图上时，可以查看该点对应的拉力值，如图9所示。值得注意的是，为了展示预测效果，本次预测数据来源于两块性能差别较大的板，可以看到性能优越的部分为红色，不满足要求的部分为蓝色，且伴有红色记号。在下方同样存在状态栏，显示当前鼠标所指位置预测的拉力值，以方便对应查看。
 
![image](https://github.com/xiangxu05/MainforecastSystem/assets/112842118/e6904f56-fada-4e79-b47d-758b48e74cf4)

