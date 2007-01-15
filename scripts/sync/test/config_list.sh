#
# Gets the list of common configuration files and their destinations
# for both robots and the base-station.
#
# puts them in the variables 'files', 'dests', and 'asroot',
#   where 'asroot' is "yes" for system config files.
#

force mkdir -p ~/tmp

i=-1

# same file name to dir
i=`expr $i + 1`
files[$i]=file0
dests[$i]=~/tmp/
asroot[$i]=no

# same file name to file
i=`expr $i + 1`
files[$i]=file1
dests[$i]=~/tmp/file1
asroot[$i]=no

# different file name to file
i=`expr $i + 1`
files[$i]=file2
dests[$i]=~/tmp/abcd
asroot[$i]=no

# different file name to file as root
i=`expr $i + 1`
files[$i]=file3
dests[$i]=~/tmp/qwrt
asroot[$i]=yes
