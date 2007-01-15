#!/bin/bash

USAGE_ARGS="[-f(orce)|-t(est)] dir hostfile"
nonInteractive=no
testMode=no
if [ $# -gt 3 ]; then
    echo "Usage: $0 $USAGE_ARGS"; exit 1
fi
if [ $# -eq 3 ]; then
    if [ "$1" == "-f" ]; then
        nonInteractive=yes
    elif [ "$1" == "-t" ]; then
        testMode=yes
    else
        echo "Usage (>4): $0 $USAGE_ARGS"; exit 1
    fi
    srcDir="$2"
    hostsFile="$3"
fi
if [ $# -eq 2 ]; then
    srcDir="$1"
    hostsFile="$2"
fi
if [ $# -lt 2 ]; then
    echo "Usage ($#<3): $0 $USAGE_ARGS"; exit 1
fi

if ! [ -d $srcDir ]; then
    echo "Error: Directory does not exist: $srcDir"
    exit 1
fi
if ! [ -f $srcDir/config_list.sh ]; then
    echo "Error: Directory $srcDir does not contain file config_list.sh"
    exit 1
fi
if ! [ -f $hostsFile ]; then
    echo "Error: File $hostsFile does not exist"
    exit 1
fi

#
# lookup IP address based on the hostname
#
myHostname=`hostname`
IP=`cat $hostsFile | grep $myHostname | tail -1 | sed -e 's/ \+.*//'`
if [ -z $IP ]; then
    echo "$0: Bugger, couldn't work out my IP from $hostsFile..."
    exit 1
fi
echo "The IP of this hosts seems to be: $IP"

#
# Author: Alex Brooks
#
# Tries to execute the command, exits on failure.
#
# '-v' causes it to echo the command to stderr before trying it.
# '-t' causes it to echo the command to stderr but not execute it.
#

function force {

    USAGE_ARGS="[-v|-t] <command> [<args>]"

    if [ $# -lt 1 ]; then
        echo "Usage: force $USAGE_ARGS"; exit 1
    fi

    verbose=0
    if [ "$1" == "-t" ]; then
        test=1
        shift
        echo "force (test only): $*" >&2
        return
    elif [ "$1" == "-v" ]; then
        verbose=1
        shift
        echo "force: $*" >&2
    fi

    $*
    result=$?
    if [ $result -ne 0 ]; then
        echo "force: fatal error: '$*' failed." >&2
        exit 1
    fi

}

#!/bin/sh

function check_config_list {

    USAGE_ARGS="<files> <dests> <asroot>"
    files=$1
    dests=$2
    asroot=$3
    if [ $# -lt 3 ]; then
        echo "Usage: check_config_list $USAGE_ARGS"; exit 1
    fi

    # Sanity check
    if [ ${#files[*]} -ne ${#dests[*]} ] || [ ${#files[*]} -ne ${#asroot[*]} ]; then
        echo "Oops: mismatch in lengths of files, dests, and asroot."
        echo "{#files[*]}:  ${#files[*]}"
        echo "{#dests[*]}:  ${#dests[*]}"
        echo "{#asroot[*]}: ${#asroot[*]}"
        exit 1
    fi

    for file in ${files[*]}; do
        if ! [ -f $file ]; then
            echo "Error: File does not exist: $file"
            exit 1
        fi
    done

    filesMax=`expr ${#files[*]} - 1`
    for i in `seq 0 $filesMax`; do

        if [ -z "${files[$i]}" ] || [ -z "${dests[$i]}" ] || [ -z "${asroot[$i]}" ]; then
            echo "Not properly initialised: element $i:"
            echo -e "${files[$i]} \t -> ${dests[$i]}"
        fi

        if [ "${asroot[$i]}" != "yes" ] &&  [ "${asroot[$i]}" != "no" ]; then
            echo "For $i'th file ${files[$i]} -> ${dests[$i]}:"
            echo "unexpected asroot: ${asroot[$i]}"
            exit 1
        fi

        if ! [ -f ${files[$i]} ]; then
            echo "Error: File does not exist: $file"
            exit 1
        fi

    done
}

function print_config_list {

    USAGE_ARGS="<files> <dests> <asroot>"
    files=$1
    dests=$2
    asroot=$3
    if [ $# -lt 3 ]; then
        echo "Usage: print_config_list $USAGE_ARGS"; exit 1
    fi

    echo "FILES TO INSTALL:"
    filesMax=`expr ${#files[*]} - 1`
    for i in `seq 0 $filesMax`; do
        if  [ "${asroot[$i]}" == "yes"  ]; then
            echo -e "  $i: ${files[$i]} \t -> ${dests[$i]} \t (as root)"
        else
            echo -e "  $i: ${files[$i]} \t -> ${dests[$i]}"
        fi
    done

    check_config_list $files $dests $asroot
}

function pre_install_check {

    USAGE_ARGS="<fileToInstall> <destFile> <nonInteractive>"
    fileToInstall=$1
    dest=$2
    nonInteractive=$3
    if [ $# -lt 2 ]; then
        echo "Usage: pre_install_check $USAGE_ARGS"; exit 1
    fi

    # Check if it exists
    if [ -e $destFile ]; then
        if ! `diff $fileToInstall $destFile > /dev/null 2>&1`; then
            while true; do
                echo
                echo    "WARNING: File: $destFile"
                echo    "         exists, and differs from the file we're about to install."

                if [ "$nonInteractive" == "yes" ]; then
                    return 0
                fi

                echo -n "Proceed? [Y/n/diff] "
                read a
                if [ "$a" == "Y" ] || [ "$a" == "y" ] || [ "$a" == "" ]; then
                    return 0
                elif [ "$a" == "N" ] || [ "$a" == "n" ]; then
                    echo "OK, skipping $destFile"
                    return 1
                else
                    # Show the user the diff
                    echo "('<' is fileToInstall, '>' is currently-installed version)"
                    diff $fileToInstall $destFile
                fi
            done
        fi
    fi
}

#
# Note: can't deal with substituting '#'s
#
function create_substitutions_file {
    USAGE_ARGS="<subsFrom> <subsTo> <file>"
    subsFrom=$1
    subsTo=$2
    file=$3
    if [ $# -ne 3 ]; then
        echo "Usage: create_substitutions_file $USAGE_ARGS"; exit 1
    fi

    if [ ${#subsFrom[*]} -ne ${#subsTo[*]} ]; then
        echo "length mismatch on subsFrom and subsTo:"
        echo "subsFrom: $subsFrom"
        echo "subsTo:   $subsTo"
        exit 1
    fi

    # remove anything old
    force rm -f $file

    # write out the new rules
    iMax=`expr ${#subsFrom[*]} - 1`
    for i in `seq 0 $iMax`; do
        #echo "${subsFrom[$i]} --> ${subsTo[$i]}"
        echo "s#${subsFrom[$i]}#${subsTo[$i]}#" >> $file
    done
}

#
# subsFrom & subsTo are arrays of substitutions: from one value to another.
#
function install_config_list {

    USAGE_ARGS="<files> <dests> <asroot> <subsFrom> <subsTo> <testMode> <nonInteractive>"
    files=$1
    dests=$2
    asroot=$3
    subsFrom=$4
    subsTo=$5
    testMode=$6
    nonInteractive=$7
    if [ $# -ne 7 ]; then
        echo "Usage: print_config_list $USAGE_ARGS"; exit 1
    fi
    if [ "$testMode" != "yes" ] && [ "$testMode" != "no" ]; then
        echo "Invalid testMode: $testMode"
    fi
    if [ "$nonInteractive" != "yes" ] && [ "$nonInteractive" != "no" ]; then
        echo "Invalid nonInteractive: $nonInteractive"
    fi

    forceFlag="-v"
    if [ "$testMode" == "yes" ]; then
        forceFlag="-t"
    fi

    #
    # Check inputs
    #
    check_config_list $files $dests $asroot

    filesMax=`expr ${#files[*]} - 1`
    for i in `seq 0 $filesMax`; do

        # Do we need to install as root?
        if [ "${asroot[$i]}" == "yes" ]; then
            prefix="sudo"
        else
            prefix=""
        fi

        # there are no subdirs inside source dir
#         filebase=`force basename ${files[$i]}`
        filebase=${files[$i]}
        filefull=${files[$i]}
        destFile=${dests[$i]}
        
        # Deal with the case where 'dest' is a directory
        if [ -d $destFile ]; then
            destFile=${dests[$i]}/$filebase
            # remove double-slashes, they're pissing me off.
            destFile=`echo $destFile | sed s#//#/#`
        fi

        # Make substitutions
        subsFile=/tmp/subs.sed
        create_substitutions_file $subsFrom $subsTo $subsFile
        cat $filefull | sed -f $subsFile > /tmp/$filebase

        # sanity check and ask user if it's OK.
        if ! pre_install_check "/tmp/$filebase" "$destFile" $nonInteractive; then
            continue
        fi

        # Install the file
        force $forceFlag $prefix mv /tmp/$filebase $destFile

        # Possibly make it executable
        if [ -x $filefull ]; then
            force $forceFlag $prefix chmod +x $destFile
        fi

        # If we're installing into /etc/init.d, set things up to start at boot-time
        if `echo $destFile | grep -q /etc/init.d/`; then
            echo "Setting up $destFile to start at boot-time"
            force $forceFlag sudo update-rc.d `basename $destFile` defaults 99
        fi

    done
}

#
# Set up the list of files, their destinations and required permissions
#
cd $srcDir
source config_list.sh

print_config_list $files $dests $asroot

echo "Proceed with installation? [Y/n]"
read answer
if [ "$answer" == "n" ]; then
    cd ..
    exit 1
fi

subsFrom[0]=%HOST%
subsTo[0]=`hostname`
subsFrom[1]=%IP%
subsTo[1]=$IP
install_config_list $files $dests $asroot $subsFrom $subsTo $testMode $nonInteractive
cd ..
