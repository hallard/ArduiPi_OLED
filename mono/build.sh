#!/bin/bash

OLDPWD=`pwd`

BD=`dirname $0`
BASEDIR=`realpath $BD`

echo
echo "---> Build native library"

cd $BASEDIR/..
make

cd $BASEDIR

echo
echo "---> Ensure nuget installed"
apt-get install nuget

echo
echo "---> Restoring nuget packages"
nuget restore

echo
echo "---> Build managed library and test suite"
xbuild /nologo /verbosity:quiet /toolsversion:4.0 ArduiPi_OLED_Wrapper.sln

echo
echo "---> type $BASEDIR/run_test.sh"

cd $OLDPWD

