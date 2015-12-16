**Our repository has been moved to Github:https://github.com/osdlyrics/osdlyrics**

---

OSD Lyrics is a lyrics show compatible with various media players. It is not a plugin but a standalone program. OSD Lyrics shows lyrics on your desktop, in the style similar to KaraOK. It also provides another displaying style, in which lyrics scroll from bottom to top. OSD Lyrics can download lyrics from the network automatically.

OSD Lyrics is released under GPL v3.

# Recent News #

## 2012-06-04: _OSD Lyrics 0.4.3 stable_ ##
You can download the source code tarball from [download page](http://code.google.com/p/osd-lyrics/downloads/detail?name=osdlyrics-0.4.3.tar.gz)

  * Player support:
    * Add [cmus](http://cmus.sourceforge.net/) support, thanks to alepulver
  * Bug fix:
    * Dir is not closed in `_prepend_subdirs` (`ol_app_info.c`)
    * Show player chooser if connected player quits in 1 minute

## 2012-01-19: _OSD Lyrics 0.4.2 stable_ ##
You can download the source code tarball from [download page](http://code.google.com/p/osd-lyrics/downloads/detail?name=osdlyrics-0.4.2.tar.gz)

<font color='red'><b>Important</b></font>: Try this [patch](http://code.google.com/p/osd-lyrics/downloads/detail?name=osdlyrics-0.4.2-glib.patch) if you fail to compile from source code.

  * UI improvements:
    * Player choose dialog redesigned
    * Fade out on the edge of lyric text in OSD mode if it is too long
    * Drag-to-seek in scrolling mode

  * Player support:
    * Bring back support for Rhythmbox prior to 0.13
    * Support [RhythmCat](http://code.google.com/p/rhythmcat/)
    * Auto-detect the launch command for Audacious 2 and 3
    * Do not require root privilege to launch MPD

  * Bug fixes:
    * Blurring in OSD mode will not cause a frame around lyrics
    * Fix compilation issue in BDS systems.
    * Fix track duration in MPRIS support
    * Deal with player launch commands with %f or %U as arguments
    * ttPlayer engine can search lyrics with ' in the title
    * CUE sheets are supported now
    * Don't show invalid search result of Xiami engine

[Full release announcement](http://groups.google.com/group/osd-lyrics/browse_thread/thread/ad2135a6afddc83e)

## 2011-11-22: _OSD Lyrics 0.4.2 beta2_ ##
Download the [source tarball](http://osd-lyrics.googlecode.com/files/osdlyrics-0.4.2-beta2.tar.gz) and try it at your own risk.

## 2011-11-22: _OSD Lyrics 0.4.2 beta1_ ##
Download the [source tarball](http://osd-lyrics.googlecode.com/files/osdlyrics-0.4.2-beta1.tar.gz) and try it at your own risk.

## 2011-07-08: _OSD Lyrics 0.4.1 stable_ ##
  * Choose player if no supported player running on launch
  * Outline blur on OSD mode
  * Support all MPRIS-compatible players
  * New lyrics search site: xiami.com
  * Fix ttPlayer search
  * Search lyrics from more than one sites
  * All files with the name of `osd-lyrics' are replaced to `osdlyrics'

## 2011-07-08: _OSD Lyrics 0.4.1 beta2_ ##
The only change in beta2 is that the version requirement of GTK+2 is decreased.

Try the [source tarball](http://osd-lyrics.googlecode.com/files/osdlyrics-0.4.1-beta2.tar.gz) at your own risk.

## 2011-07-03: _OSD Lyrics 0.4.1 beta1_ ##

Source tarball: [Download page](http://code.google.com/p/osd-lyrics/downloads/detail?name=osdlyrics-0.4.1-beta.tar.gz&can=2&q=#makechanges)

## 2011-06-26: _openSUSE support_ ##

Thanks to [@hillwood](https://hillwoodhome.info/), OSD Lyrics supports openSUSE now :)

You can follow our [wiki page](http://code.google.com/p/osd-lyrics/wiki/InstallOpenSUSE) to add the repository, or [download the stable version](http://code.google.com/p/osd-lyrics/downloads/list?can=2&q=distro=openSUSE&colspec=Filename%20Distro%20Arch%20Summary%20Uploaded%20Size%20DownloadCount) if your are under openSUSE 11.4.

## 2011-06-04: _OSD Lyrics 0.4.0 stable_ ##
  * Support more players: Clementine, Deciber-audio-player, Gmusicbrowser, Listen, and VLC.
  * Display OSD lyrics as a normal window.
  * Display lyrics in traditional scrolling mode.
  * Add proxy support.
  * Use ttPlayer instead of other lyric download sites.
  * Improve performance with MPRIS-compatible players.

# Documentation #
  * [How to build from source code](http://code.google.com/p/osd-lyrics/wiki/Build)
  * [Compatible players](http://code.google.com/p/osd-lyrics/wiki/Players)

# Screenshots #
![http://osd-lyrics.googlecode.com/svn/images/screenshots/osd-mode-single.png](http://osd-lyrics.googlecode.com/svn/images/screenshots/osd-mode-single.png)

OSD Mode with single line

![http://osd-lyrics.googlecode.com/svn/images/screenshots/osd-mode-double.png](http://osd-lyrics.googlecode.com/svn/images/screenshots/osd-mode-double.png)

OSD Mode with two lines

![http://osd-lyrics.googlecode.com/svn/images/screenshots/scroll-mode.png](http://osd-lyrics.googlecode.com/svn/images/screenshots/scroll-mode.png)

Scrolling mode

# Have Trouble? #
  * Read our [FAQ](FAQ.md)
  * Post to our [discussion group](http://groups.google.com/group/osd-lyrics) or send a mail to [osd-lyrics@googlegroups.com](mailto:osd-lyrics@googlegroups.com).
  * You can also fire an [issue](http://code.google.com/p/osd-lyrics/issues/list).

# Get Involved #

  * If you would like to translate OSD Lyrics to other languagues, please visit [our translation site](https://translations.launchpad.net/osd-lyrics)
  * If there is any feature request, suggestion, or bug, feel free to report them to  http://code.google.com/p/osd-lyrics/issues/list
  * If there is any Grammar mistake in both documentation and software interface, please let me know :)


---


# 疑难解答 #
  * 请首先查阅我们的[FAQ](FAQ.md)
  * 您可以到我们的[讨论组](https://groups.google.com/group/osd-lyrics)发帖寻问，或者发送邮件到[osd-lyrics@googlegroups.com](mailto:osd-lyrics@googlegroups.com)
  * 您也可以向我们发一个[问题报告](http://code.google.com/p/osd-lyrics/issues/list)

# 帮助我们 #

  * 如果有任何功能上的建议和改进意见，或者在使用过程中遇到了bug，欢迎提交到[问题报告](http://code.google.com/p/osd-lyrics/issues/list)
  * 如果有兴趣将OSD Lyrics翻译为其他语言，请访问[OSD Lyrics在launchpad上的翻译站点](https://translations.launchpad.net/osd-lyrics)