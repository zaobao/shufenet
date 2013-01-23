shufenet
========

上财网络认证客户端，为上财教育网和联通网特制，非通用802.1x客户端。

版本v0.9.1

更新内容：
=========
  1、修复一个bug。

  2、新增安装脚本。


安装方式：
=========
  1、安装libpcap和libnet。
  
  2、下载所有文件放在一个文件夹里。
  
  3、用root权限运行install.sh

使用方式1：
==========
  1、先确定网卡设置为DHCP自动。
  
  2、用root权限修改su.profile文件中的用户名 密码 网卡
  
  3、启动：用root权限运行service shufenet start
  
  4、停止：用root权限运行service shufenet stop
  
  5、重连：用root权限运行service shufenet restart
  

使用方式2：
==========
  1、先确定网卡设置为DHCP自动。
  
  2、启动：用root权限运行文件夹中的shufenet 用户名 密码 网卡（默认是eth0）
  
  3、停止：pkill shufenet


卸载方式：
=========
  1、用root权限运行 rm /etc/init.d/shufenet
  
  2、删除那个文件夹
  
  3、无残留
  

注意：
=====
  1、如果不知道是哪个网卡，使用ifconfig -a查看。
  
  2、Ubuntu用户，如果您用的是右上角图形界面来网卡，在ipv4中选DHCP自动，并在“需要ipv4地址完成这个连接”前打钩（重要），最好在“自动连接”前打钩，这些配置不会影响到ipv6的配置。
  
  3、如果你不想开机就启动或在联网前就使用DHCP客户端，可以在启动shufenet之前用root权限运行dhclient -4 -d eth0（根据你的网卡名），opensuse用户请使用dhcpcd，在dhclient超时睡眠前使用shufenet连接成功后，会提示获取ip地址。
  
  4、本项目开放源代码，并不得将此项目的代码用于商业用途，除了本项目中的代码，对于此项目的任何分支或是被他人恶意篡改的版本所带来的问题，本项目作者概不负责。请大家尽量用以下网址获取本项目，以防有人插入恶意代码：
    https://github.com/zaobao/shufenet

如果有任何问题，请发邮件至1205499934@shufe.com。
===============================================================================

The Shufenet project is a 100% C 802.1x client for Unix/Linux only designed for Shanghai University of Finance and Economy's network.

Shufenet is open source and is dual licensed using the BSD and GPL. You must abide by both when distributing shufenet.


BUILDING SHUFENET
=================
Download all the files into one directory. Run install.sh with the root privilege.

RUNNING SHUFENET
================
The directory you download files into is also the run directory when built from source.

To login:
  sudo shufenet {username} {password} [devicename]

To logoff:
  sudo eap_logoff
  
Or:
  sudo service shufenet start/stop/restart











