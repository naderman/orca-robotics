#!/bin/sh
#
# Tries to add licenses in a directory structure.
# This thing isn't guaranteed to work 100% reliably, it
# may need some manual fixing-up.
#
# Author: Alex Brooks
#

USAGE_ARGS="<top_directory> <license_file>"

MY_LICENSE_FILE=license.txt
UPDATE_MY_LICENSE=no
REPLACE_GPL=no
FOR_REAL=no
VERBOSE=no

# Any file containing this string is assumed to have a license.
license_search_string="Copyright"

# This is the signature for the license we want to be there.
my_license_signature="Components for robotics"

# If a file contains this, it's assumed to be GPL-licensed
gpl_signature="GNU General"
# If a file contains this, it's assumed to be Ice-licensed
ice_license_string="ZeroC, Inc. All rights reserved"

# ----------------------------------------------------------------------

if [ $# -lt 2 ]; then
    echo "USAGE: $0 $USAGE_ARGS"
    exit 1
fi
toplevel=$1
if ! [ -d $toplevel ]; then
    echo "Doesn't seem to be a directory: $toplevel"
    exit 1
fi
MY_LICENSE_FILE=$2
if ! [ -f $MY_LICENSE_FILE ]; then
    echo "Couldn't find license file: $MY_LICENSE_FILE"
    exit 1
fi


for file in `find $toplevel -name \*.h`   \
            `find $toplevel -name \*.cpp` \
            `find $toplevel -name \*.c`   \
            `find $toplevel -name \*.ice` \
            `find $toplevel -name \*.dox`
do

    # Ignore ice-licensed files
    if `grep -q "$ice_license_string" $file`; then
        # echo "Ignoring ice-licensed file: $file"
        continue
    fi

    if `grep -q "$license_search_string" $file`; then

        # Has a license 
        if [ "$UPDATE_MY_LICENSE" == "yes" ] || [ "$REPLACE_GPL" == "yes" ]; then

            license_end_line=`grep --line-number --max-count=1 "*/" $file | sed s/:.*//`
            total_num_lines=`wc --lines $file | sed s/\ .*//`

            if [ "$license_end_line" == "" ]; then
                echo "Screwy file: $file"
                exit 1
            fi

            if [ "$REPLACE_GPL" == "yes" ]; then
                if `head $file --lines=$license_end_line | grep -q "$gpl_signature"`; then
                    cat $MY_LICENSE_FILE > temp
                    cat $file | tail --lines=`expr $total_num_lines - $license_end_line` >> temp
                    num_licenses=`grep -c "$license_search_string" temp`
                    # echo " -- num_licenses: $num_licenses"
                    if [ $num_licenses -gt 1 ]; then
                        echo "Oops: $file ended up with multiple licenses..."
                    #exit 1
                    fi

                    echo "Replacing GPL license in $file"
                    if [ "$VERBOSE" == "yes" ]; then
                        echo "==========================================="
                        head $file
                    fi
                    if [ $FOR_REAL == "yes" ]; then
                        cat temp > $file
                    fi
                fi
            fi
            
            if [ "$UPDATE_MY_LICENSE" == "yes" ]; then
                if `head $file --lines=$license_end_line | grep -q "$my_license_signature"`; then
                    cat $MY_LICENSE_FILE > temp
                    cat $file | tail --lines=`expr $total_num_lines - $license_end_line` >> temp
                    num_licenses=`grep -c "$license_search_string" temp`
                    # echo " -- num_licenses: $num_licenses"
                    if [ $num_licenses -gt 1 ]; then
                        echo "Oops: $file ended up with multiple licenses..."
                    #exit 1
                    fi

                    echo "Updating ORCA license in $file"
                    if [ $FOR_REAL == "yes" ]; then
                        cat temp > $file
                    fi
                fi
            fi
        fi

    else
        # No license found -- add one.
         echo "Adding license to $file"
         if [ "$VERBOSE" == "yes" ]; then
             echo "==========================================="
             head $file
         fi
         cat $MY_LICENSE_FILE > temp
         cat $file >> temp
         if [ $FOR_REAL == "yes" ]; then
             cat temp > $file
         fi
    fi

done