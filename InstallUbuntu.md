
# Install from PPA (recommend) #
It is recommend to install OSD Lyrics from PPA in Launchpad.net so that you can get update notification in time.

## Ubuntu 9.10 or later ##
Import the PPA repository:
```
sudo add-apt-repository ppa:osd-lyrics/ppa
```

Update & install:
```
sudo apt-get update
sudo apt-get install osdlyrics
```

## Ubuntu 9.04 or older ##
Edit your apt source:
```
sudo gedit /etc/apt/sources.list
```

Append the following ppa address:
Ubuntu 8.10 (Intrepid):
```
deb http://ppa.launchpad.net/osd-lyrics/ppa/ubuntu intrepid main 
deb-src http://ppa.launchpad.net/osd-lyrics/ppa/ubuntu intrepid main
```

Ubuntu 9.04 (Jaunty):
```
deb http://ppa.launchpad.net/osd-lyrics/ppa/ubuntu jaunty main 
deb-src http://ppa.launchpad.net/osd-lyrics/ppa/ubuntu jaunty main
```

Add signing key:
```
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 4865CF4F
```

Update & install:
```
sudo apt-get update
sudo apt-get install osdlyrics
```

# Install from deb archive #

Download the deb archive according to your architecture, then double click it :)