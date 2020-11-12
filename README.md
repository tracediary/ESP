# ESP
ESP MCU connect to Ali Cloud and Ali Genie. 天猫精灵

# 环境准备
该代码工程需要结合ESP官方链接中环境搭建使用
请参考如下链接搭建环境

[RTOS SDK环境搭建](https://github.com/espressif/ESP8266_RTOS_SDK)

[ESP ALI SDK环境搭建](https://github.com/espressif/esp-ali-smartliving)

[官方指导文档](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html#guides)

# 其他说明
环境搭建好之后，请将esp-ali-smartliving的路径设置环境变量  ***export ALI_PATH="${your_path}/esp-ali-smartliving"***
否则该工程将编译不通过。此环境变量将在Makefile中使用到

该工程使用不包含三元组信息，使用者需要自己在阿里云上创建产品然后烧录三元组，三元组获取及烧录方法请参考[官方链接](https://github.com/espressif/esp-ali-smartliving/blob/master/config/mass_mfg/README.md)

此工程是直接借用官方的代码修改而来，请选阅读[原官方代码](https://github.com/espressif/esp-ali-smartliving/tree/master/examples/solutions/smart_light)的帮助内容

原官方代码下载到开发板后，不能直接被天猫精灵控制，原因是官方代码对接的是旧的天猫精灵平台，阿里云后来将天猫精灵平台合并后为生活物联网平台，很多参数做了修改
对接生活物联网平台的将可以直接使用此工程，目前仅支持天猫精灵生态产品，自有产品的开发需要修改部分代码，主要是在配网部分。


# 以后规划
本人将不定期上传修改和整理之后代码
