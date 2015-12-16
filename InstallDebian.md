# Add the repository #

## Debian 6 ##
```
sudo echo 'deb http://download.opensuse.org/repositories/home:hillwood/Debian_6.0/ /' >> /etc/apt/sources.list
sudo apt-get update
```
This repository only support Debian 6.

# Install OSD Lyrics #

After adding repository, you can use apt-get to install it:
```
sudo apt-get install osdlyrics
```
Install osdlyrics beta:
```
sudo apt-get install osdlyrics-beta
```
Install develop version:
```
sudo apt-get install osdlyrics-git
```