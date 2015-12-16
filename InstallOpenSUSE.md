# Add the repository #

OSD Lyrics 已被纳入进 openSUSE 12.2，你可以直接从 OSS 源中安装，我们不再单独提供安装包。我们也将继续提供社区支持，你也可以通过添加社区源安装，

OSD Lyrics have been included in openSUSE 12.2, you can install it from OSS repository. And we will still provide community support. You can also add these community repository to install.

对于使用低于版本号 12.2 的 openSUSE 用户，你得通过添加我们的社区源进行安装，请使用下面的命令完成操作：

For uses of openSUSE older than 12.2, you need to add our repository to your system before installation. Just follow the instruction below:

## openSUSE 12.1 ##
```
sudo zypper ar http://download.opensuse.org/repositories/multimedia:/apps/openSUSE_12.1/ multimedia:apps
```

## openSUSE 11.4 ##
```
sudo zypper ar http://download.opensuse.org/repositories/multimedia:/apps/openSUSE_11.4/ multimedia:apps
```

## openSUSE Tumbleweed ##
```
sudo zypper ar http://download.opensuse.org/repositories/multimedia:/apps/openSUSE_Tumbleweed/ multimedia:apps
```

# Install OSD Lyrics #

添加好安装源后，你可以通过 zypper 命令进行安装：

After adding repository, you can use zypper to install it:
```
sudo zypper in osdlyrics osdlyrics-lang
```

你也可以在 http://software.opensuse.org 中搜索“osdlyrics”进行一键安装，这样安装源会自动添加到你的系统中。

You can also search osdlyrics in http://software.opensuse.org and choose _Include users' home projects_ to 1-Click install it. The repository will be added into your system automaticly.