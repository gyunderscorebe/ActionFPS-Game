#!/bin/bash

# usage : package_assaultcube.sh API_KEY GITHUB_USER

# dependencies : https://github.com/aktau/github-release

# ** CHANGE THE BELOW 4 VARIABLES BEFORE EXECUTING THE SCRIPT ** #

# Please ensure this path is NOT inside your "PATHTOACDIR" folder, as
# this will be the place the tarball package gets saved.
SAVETARBALLPATH=.
# This will be the directory that gets packaged:
PATHTOACDIR=./game

cd "$(dirname "$0")"

# KSH won't work, as this script uses echo throughout it, echo's options are shell-specific:
if [ -z "$(echo "$BASH")" ]; then
  echo "FAILURE: Please run this script with BASH, not SH/KSH/ZSH, etc"
  exit
fi

# Set auto-aliases:
ACDIRFOLDERNAME=`basename $PATHTOACDIR`

cd "$PATHTOACDIR"
git fetch
git pull

# Checking "$PATHTOACDIR" is set correctly...
if [ -e source/src/main.cpp ]; then
  echo "Stated path to AssaultCube contains \"source/src/main.cpp\""
  echo "Hopefully this means the path is correct and contained files are good."
  echo -e "Proceeding to the next step...\n"
else
  echo -e "\033[1;31mERROR:\033[0m \"source/src/main.cpp\" did NOT exist at the PATHTOACDIR alias."
  echo "Open this shell file and modify that alias to fix it."
  exit
fi

cd source/src
make clean
make install

if [ -e ../../bin_unix ]; then
  cd ../../bin_unix
  rm -rf linux_*
  mv native_client linux_64_client
  mv native_server linux_64_server
  strip linux_64_client
  strip linux_64_server
else
  exit
fi

cd ../

# Clean out crufty files:
echo "Please wait: Cleaning out some crufty files..."
cd source/enet && make distclean
cd ../src && make clean	
cd ../../config && rm -f init.cfg killmessages.cfg saved.cfg servers.cfg
echo -e "// This file gets executed every time you start AssaultCube. \n\n// This is where you should put any scripts you may have created for AC." > autoexec.cfg
# Leave tutorial code here, just in case of future use:
cd .. && rm -f `(find ./demos/* -type f | grep -v "tutorial_demo.dmo")`
rm -f ./bin_unix/native_*
rm -f ./packages/maps/*.cgz
rm -f ./screenshots/*
# Delete gedit backups:
find . -type f -name "*~" -delete

# Set up ./config/securemaps.cfg - just in-case:
echo "... Generating ./config/securemaps.cfg"
echo "resetsecuremaps" > ./config/securemaps.cfg
find ./packages/maps/official/*.cgz | \
  xargs -i basename {} .cgz | \
  xargs -i echo -e "securemap" {} | \
  sort -u >> ./config/securemaps.cfg

cd ..

if [ -e AssaultCube ]; then
  cd AssaultCube
  rm -rf *
  cd ..
else
  mkdir AssaultCube
fi

# update checksum file
$PATHTOACDIR/source/dev_tools/generate_md5_checksums.sh

# copy files from temporary repository
cp -R $PATHTOACDIR/* AssaultCube

# client archive
tar cjvf $SAVETARBALLPATH/linux_client.tar.bz2 \
  --exclude=".*" \
  --exclude=*linux_64_server \
  --exclude=*.bat --exclude=*bin_win32* \
  --exclude=*source* \
  --exclude=*new_content.cfg \
  --exclude=*new_content.cgz \
  --exclude-vcs \
  AssaultCube/

# server archive
tar cjvf $SAVETARBALLPATH/linux_server.tar.bz2 \
  --exclude=".*" \
  --exclude=*.bat --exclude=*bin_win32* \
  --exclude=*source* \
  --exclude=*textures* \
  --exclude=*mods* \
  --exclude=*models* \
  --exclude=*misc* \
  --exclude=*local* \
  --exclude=*crosshairs* \
  --exclude=*audio* \
  --exclude=*demos* \
  --exclude=*scripts* \
  --exclude=*screenshots* \
  --exclude=*new_content.cfg \
  --exclude=*new_content.cgz \
  --exclude-vcs \
  AssaultCube/

# remove temporary repository
rm -rf AssaultCube

# Clean up:
cd $PATHTOACDIR
git clean -fdx && git reset --hard


cd source/src
AC_VERSION=`grep -oP '#define AC_VERSION \K\d+' cube.h`
# MC_VERSION=`grep -oP '#define MC_VERSION \K\d+' cube.h`
BUILD_VERSION="$AC_VERSION"

echo $BUILD_VERSION

cd ../../..

github-release release --security-token $1 --user $2 --repo woop --tag $BUILD_VERSION
github-release upload --security-token $1 --user $2 --repo woop \
    --tag $BUILD_VERSION --name "linux_client_$BUILD_VERSION.tar.bz2" --file $SAVETARBALLPATH/linux_client.tar.bz2
github-release upload --security-token $1 --user $2 --repo woop \
    --tag $BUILD_VERSION --name "linux_server_$BUILD_VERSION.tar.bz2" --file $SAVETARBALLPATH/linux_server.tar.bz2
github-release upload --security-token $1 --user $2 --repo woop \
    --tag $BUILD_VERSION --name "windows_client_$BUILD_VERSION.exe" --file $SAVETARBALLPATH/windows_client.exe
