#!/bin/sh

installl_patth=$1
usr_source_path=$2

mkdir -p $installl_patth/dev
mkdir -p $installl_patth/proc
mkdir -p $installl_patth/sys
mkdir -p $installl_patth/lib
mkdir -p $installl_patth/lib64
mkdir -p $installl_patth/etc/init.d
mkdir -p $installl_patth/usr
mkdir -p $installl_patth/lib/x86_64-linux-gnu

cp $usr_source_path/etc/init.d/rcS $installl_patth/etc/init.d/
cp $usr_source_path/usr/test_a $installl_patth/usr/
cp $usr_source_path/usr/test_d $installl_patth/usr/
cp -d $usr_source_path/lib/x86_64-linux-gnu/* $installl_patth/lib/x86_64-linux-gnu/
cp -d $usr_source_path/lib64/ld-linux-x86-64.so.2 $installl_patth/lib64/

exit
