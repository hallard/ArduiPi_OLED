#!/bin/bash

OLDPWD=`pwd`

BD=`dirname $0`
BASEDIR=`realpath $BD`

echo
echo "---> Build native library"

cd $BASEDIR/..
make

echo
echo "---> Build managed library and test suite"

cd $BASEDIR
echo "from `pwd`"
xbuild /nologo /verbosity:quiet /toolsversion:4.0 ArduiPi_OLED_Wrapper.sln

echo
echo "---> type $BASEDIR/run_test.sh"

cd $OLDPWD

