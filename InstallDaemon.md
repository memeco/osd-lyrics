A new daemon will be introduced in OSD Lyrics 0.5. The new daemon is written by python, so some additional python packages are required.

# Dependencies #
## Basic Tools ##
  * autoconf
  * automake
  * libtool
  * intltool
  * gcc
  * python >= 2.6

## Python packages ##
  * [chardet](http://chardet.feedparser.org/)
  * [dbus-python](http://dbus.freedesktop.org/doc/dbus-python/)
  * [pygobject](http://live.gnome.org/PyGObject)
  * sqlite3

## C libraries ##
  * libgtk2.0-dev >= 2.12.0
  * _(will be remove)libdbus-glib-1-dev_
  * _(will be remove)libcurl3-dev_
  * libnotify-dev
  * _(will be remove)libsqlite3-dev >= 3.3.0_
  * libmpd-dev (this is unnecessary if you configure with --disable-mpd option)
  * libxmmsclient-dev (this is unnecessary if you configure with --disable-xmms2 option)
  * libappindicator-dev (Optional for Ubuntu 10.04 or later, support for indicator-applet)

# Get source code of 0.5 series #

Clone the source repository:

```
git clone git://github.com/osdlyrics/osdlyrics.git
cd osdlyrics
```

Change to 0.5 branch:
```
git checkout --track origin/0.5-series
```

Now you get the code of OSD Lyrics 0.5

# Compile and Install #

When the source code is ready, run configure script:

```
./autogen.sh --prefix=/usr
```

Now you can build and install the program:

```
make
sudo make install
```

When you launch OSD Lyrics, the daemon will be launched automatically. If you want to launch the daemon only, run `osdlyrics-daemon`.