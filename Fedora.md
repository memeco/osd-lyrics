
# Install from repository #
**Note: Only Fedora 12 supported. If you are using an older version of Fedora, please refer _Install from RPM files_.**

## Add the repository ##
Download [repository file](http://dl.dropbox.com/u/1352061/liangsuilong.repo) to /etc/yum.repos.d/
```
su -c 'wget http://dl.dropbox.com/u/1352061/liangsuilong.repo -P /etc/yum.repos.d/'
```

If the link above is unavailable, you can create the file by yourself. Just create a file /etc/yum.repos.d/liangsuilong.repo.

Write down the following in the file and save.
```
[liangsuilong]
name=Liang Suilong
baseurl=http://liangsuilong.fedorapeople.org/repo/$releasever/$basearch/
enabled=1
gpgcheck=0
```

## Install OSD Lyrics ##

Now you can install OSD Lyrics with yum easily :)
```
yum makecache && yum install osd-lyrics
```

# Install from RPM files #
You can find RPM files at http://liangsuilong.fedorapeople.org/yum/12/i386/

# RPM files for Fedora 14 #
Download from: http://code.google.com/p/osd-lyrics/downloads/list

# Install from source code #
Please refer [build instructions](Build.md).