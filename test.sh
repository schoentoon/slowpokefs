#!/bin/bash
TMP=`mktemp -d -p .`
mkdir mnt
./slowpokefs -F${TMP} mnt
touch ${TMP}/a ${TMP}/b ${TMP}/c #Filling our directory..
echo "Executing ls on ${TMP}"
time ls ${TMP}
echo "Executing ls on mnt (slowpokefs)"
time ls mnt
fusermount -uz mnt
killall -9 slowpokefs
rm -rf ${TMP} mnt
