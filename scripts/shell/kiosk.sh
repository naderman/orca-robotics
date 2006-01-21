#!/bin/bash

while true; do
  dcop kwin KWinInterface setCurrentDesktop 2 >/dev/null
  first_konqueror=`dcop | grep konqueror --max-count 1`
  dcop $first_konqueror konqueror-mainwindow#1 reload
  sleep 5
  dcop kwin KWinInterface setCurrentDesktop 3 >/dev/null
  sleep 5
done
        