#!/bin/sh

search_string=" *  You should have received a copy of the GNU"

for file in `find .. -name *.h` `find .. -name *.cpp` `find .. -name *.c`; do

    if `grep -q "$search_string" $file`; then
        true
    else
        echo "Adding license to $file"
        cat license.txt > temp
        cat $file >> temp
        cat temp > $file
    fi

done