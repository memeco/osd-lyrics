OSD Lyrics has been added to [gentoo-china](http://code.google.com/p/gentoo-china-overlay/) overlay. If you didn't add gentoo-china overlay, you need to add it first. Otherwise you can skip adding the gentoo-china overlay.


_Note: The following instruction requires root privilege._

# Add gentoo-china overlay #
## Emerge layman 1.2.3 or later ##
```
emerge -av app-portage/layman-1.2.3
```
_Note: if you are using stable branch, you'll need to unmask layman-1.2.3 manually._
## Enable gentoo-china overlay ##
```
sed -i -e 's/\(nocheck.*\)no/\1yes/' /etc/layman/layman.cfg
echo "source /usr/local/portage/layman/make.conf" >> /etc/make.conf
mkdir -p /usr/local/portage/layman
echo PORTDIR_OVERLAY=\"\" > /usr/local/portage/layman/make.conf
USE="-nowebdav" emerge subversion
layman -f -a gentoo-china
```
## Update overlays ##
```
sudo layman -S
```
# Install OSD Lyrics #
## Install the latest version ##
```
echo "=media-plugins/osd-lyrics-9999 **" >> /etc/portage/package.keywords
emerge -av osd-lyrics
```
## Install packaged version ##
```
echo "media-plugins/osd-lyrics ~x86" >> /etc/portage/package.keywords
emerge -av osd-lyrics
```