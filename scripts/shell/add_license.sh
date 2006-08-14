#!/bin/sh

UPDATE_ORCA_LICENSE=no
REPLACE_GPL=no
FOR_REAL=no
VERBOSE=no

toplevel=../..
search_string="Copyright"
gpl_signature="GNU General"
orca_license_signature="Components for robotics"
ice_license_string="ZeroC, Inc. All rights reserved"
for file in `find $toplevel -name *.h`   \
            `find $toplevel -name *.cpp` \
            `find $toplevel -name *.c`   \
            `find $toplevel -name *.ice` \
            `find $toplevel -name *.dox`
do

    # Ignore ice-licensed files
    if `grep -q "$ice_license_string" $file`; then
        # echo "Ignoring ice-licensed file: $file"
        continue
    fi

    if `grep -q "$search_string" $file`; then

        # Has a license 
        if [ "$UPDATE_ORCA_LICENSE" == "yes" ] || [ "$REPLACE_GPL" == "yes" ]; then

            license_end_line=`grep --line-number --max-count=1 "*/" $file | sed s/:.*//`
            total_num_lines=`wc --lines $file | sed s/\ .*//`

            if [ "$license_end_line" == "" ]; then
                echo "Screwy file: $file"
                exit 1
            fi

            if [ "$REPLACE_GPL" == "yes" ]; then
                if `head $file --lines=$license_end_line | grep -q "$gpl_signature"`; then
                    cat license.txt > temp
                    cat $file | tail --lines=`expr $total_num_lines - $license_end_line` >> temp
                    num_licenses=`grep -c "$search_string" temp`
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
            
            if [ "$UPDATE_ORCA_LICENSE" == "yes" ]; then
                if `head $file --lines=$license_end_line | grep -q "$orca_license_signature"`; then
                    cat license.txt > temp
                    cat $file | tail --lines=`expr $total_num_lines - $license_end_line` >> temp
                    num_licenses=`grep -c "$search_string" temp`
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
         cat license.txt > temp
         cat $file >> temp
         if [ $FOR_REAL == "yes" ]; then
             cat temp > $file
         fi
    fi

done