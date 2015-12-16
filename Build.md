# Dependencies #
## For deb systems ##
  * build-essential
  * autoconf automake libtool intltool
  * libgtk2.0-dev >= 2.12.0
  * libdbus-glib-1-dev
  * libcurl3-dev
  * libnotify-dev
  * libsqlite3-dev >= 3.3.0
  * libmpd-dev (this is unnecessary if you configure with --disable-mpd option)
  * libxmmsclient-dev (this is unnecessary if you configure with --disable-xmms2 option)
  * libappindicator-dev (Optional for Ubuntu 10.04 or later, support for indicator-applet)
## For rpm systems ##
  * gcc gcc-c++
  * autoconf automake libtool intltool
  * gtk2-devel >= 2.12.0
  * dbus-glib-devel
  * libcurl-devel
  * libnotify-devel
  * sqlite-devel
  * libmpd-devel (this is unnecessary if you configure with --disable-mpd option)
  * xmms2-devel (this is unnecessary if you configure with --disable-xmms2 option)

# Download sources #
You have two ways to obtain the latest source codes.
## Clone from github ##
```
git clone git://github.com/tigersoldier/osd-lyrics.git
cd osd-lyrics
```

See https://github.com/tigersoldier/osd-lyrics for more information.
### (Optional) Checkout the develop branch ###
By default, you will checkout the master branch which will be updated only when a new version is released.

To test new feature or not-released bug fix, you need to checkout the develop branch. Just use follow the instruction below after cloning from github:
```
git checkout --track origin/develop
```

## Download source package ##
You can get source packages from http://code.google.com/p/osd-lyrics/downloads/list

Extract it into a directory
```
tar zxvf osdlyrics-a.b.yyyymmdd.tar.gz
cd osdlyrics-a.b.yyyymmdd
```
Where a.b.yyyymmdd is the version of OSD Lyrics you download.

# Build and install #
Make sure you are in the source directory.

Then follow the instructions:
```
aclocal
autoheader
autoconf
automake --add-missing
./configure
make
sudo make install
```
By ./configure, you can append some options after it. The available options you may be insterested in are list below. To get all available options, just ./configure --help

## Available Options ##
You can use following options when invoking configure script
  * --disable-mpd: Disables MPD support. If you don't use MPD, you may be happy with it. Disabling MPD support also frees you from having dev package of libmpd installed.
  * --disable-xmms2: Disables XMMS2 support. Similar to MPD support.
  * --enable-amarok1: Enable Amarok 1.4 support. Amarok 1.4 is no longer supported by default.