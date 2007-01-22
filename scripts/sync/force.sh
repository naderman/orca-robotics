#!/bin/sh
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