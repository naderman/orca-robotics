#!/bin/sh

CHANGE_LICENSES=no
FOR_REAL=yes

toplevel=../..
search_string=" *  You should have received a copy of the GNU"
gpl_signature="GNU General"

for file in `find $toplevel -name *.h`   \
            `find $toplevel -name *.cpp` \
            `find $toplevel -name *.c`   \
            `find $toplevel -name *.ice`
do

    if `grep -q "$search_string" $file`; then

        # Has a license 
        if [ "$CHANGE_LICENSES" == "yes" ]; then

            # check if it's GPL
            if `grep -q "$gpl_signature" $file`; then
                license_end_line=`grep --line-number --max-count=1 "*/" $file | sed s/:.*//`
                # echo "$file contains the GPL (ends on line $license_end_line)"
                
                num_lines=`wc --lines $file | sed s/\ .*//`
                # echo " -- numLines: $num_lines"
                cat license.txt > temp
                cat $file | tail --lines=`expr $num_lines - $license_end_line` >> temp
                num_licenses=`grep -c "$search_string" temp`
                # echo " -- num_licenses: $num_licenses"
                if [ $num_licenses -gt 1 ]; then
                    echo "Oops: $file ended up with multiple licenses..."
                    #exit 1
                fi

                echo "Replacing license in $file"
                if [ $FOR_REAL == "yes" ]; then
                    cat temp > $file
                fi
            fi

        fi

    else
        # No license found -- add one.
         echo "Adding license to $file"
         cat license.txt > temp
         cat $file >> temp
         if [ $FOR_REAL == "yes" ]; then
             cat temp > $file
         fi
    fi

done