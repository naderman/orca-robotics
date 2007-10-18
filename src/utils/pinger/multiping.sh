#!/bin/sh

if [ $# -lt 1 ]; then
    echo "host: $1"
    echo "USAGE  : $0 <host> [count=3] [minexp=3] [maxexp=16] [interval=1] [filename="pingresults.txt"]"
    echo "EXAMPLE: $0 foo 101 4 16 0.25"
    echo "         will ping foo 101 times for each payload size from 2^4 to 2^16 bytes with interval of 0.5s"
    exit 1
else
    HOST=$1
fi

if [ $# -lt 2 ]; then
    COUNT=3
else
    COUNT=$2
fi

if [ $# -lt 3 ]; then
    MINEXP=3    # 2**3=8
else
    MINEXP=$3
fi

if [ $# -lt 4 ]; then
    MAXEXP=16   # 2**12=4096
else
    MAXEXP=$4
    # max ping size is 65507, which is close to 2**16, we'll adjust it later
    if [ $MAXEXP -gt 16 ]; then
        MAXEXP=16
    fi
fi

if [ $# -lt 5 ]; then
    INTERVAL=1
else
    INTERVAL=$5
fi

if [ $# -lt 6 ]; then
    FILENAME=pingresults.txt
else
    FILENAME=$6
fi

# delete file contents
echo "" 1>$FILENAME

for ((e=MINEXP; e <= MAXEXP; e++))  # Double parentheses, and "MAXEXP" with no "$".
do
    # packet size is the power of 2
    PACKETSIZE=$((2 ** e))
    if [ $e -eq 16 ]; then
        # this is maximum packet size in Linux
        PACKETSIZE=65507
    fi
    # payload size (subtract header)
    #PAYLOADSIZE=`expr $PACKETSIZE - 8`
    # payload size
    PAYLOADSIZE=$PACKETSIZE
 
    echo "size $PACKETSIZE bytes" 1>>$FILENAME

    echo; echo "ping $HOST -c $COUNT -s $PAYLOADSIZE -i $INTERVAL -q"
    ping $HOST -c $COUNT -s $PAYLOADSIZE -i $INTERVAL -q 1>>$FILENAME
done          
