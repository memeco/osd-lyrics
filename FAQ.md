
# Installation #
## How to install OSD Lyrics ##
For Ubuntu users, please read [Ubuntu](Ubuntu.md).

For Fedora users, please read [Fedora](Fedora.md).

For openSUSE users, please read [openSUSE](http://code.google.com/p/osd-lyrics/wiki/InstallOpenSUSE).

For Gentoo users, please read [Gentoo](Gentoo.md).

Other users please read [Build](Build.md).

# Using OSD Lyrics #
## How to move OSD Lyrics ##
Right-click the trayicon of OSD Lyrics, uncheck ``Lock''. After that you can move it by dragging it with mouse.

## Where are the downloaded lyrics ##
The default path of the downloaded lyrics is the first path in the Path option. The default filename format is the pattern of the top entry in Filename option. Both option are in the Lyric Location tab in the preference window.

In the old version of OSD Lyrics, all lyrics are located in ~/.lyrics, in the format of ``artist - title.lrc''.

# Troubleshooting #

## I launch OSD Lyrics, but it quits immediately ##
Prior to 0.4.1, you need to launch a supported player before launching OSD Lyrics. In 0.4.1, a player chooser will be shown if no supported player is running.

## Why I can search lyrics, but fail downloading them ##
It's a bug before version 0.2.20100106. If the first directory matched to download lyrics to doesn't exists, the the lyrics are failed to be downloaded. Just make the directory of the first directory in lyrics path option will work. Generally, it's "~/.lyrics" that you need to make.

## Can not display tray icon ##
Try refreshing the icon cache:
```
sudo gtk-update-icon-cache /usr/share/icons/hicolor
```

## The text is aliasing ##
You need to use a compositing window manager to get the best visual effect. If you would like to stick with metacity, which is the default window manager of GNOME, you need to enable its composition feature by following the instruction below:
  * Install and launch gconf-editor
  * Select /apps/metacity/general in the left panel
  * Check the compositing\_manager box in the right panel