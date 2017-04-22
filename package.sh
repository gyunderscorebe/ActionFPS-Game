#!/bin/bash
DIR=$(pwd)
rm -rf target
mkdir -p target/ActionFPS
cd target
cp -R $DIR/{packages,mods,config,bin_unix} ActionFPS/
rsync $DIR/{actionfps.sh,server.sh,install.sh} ActionFPS/
mv ActionFPS/bin_unix/{native_client,linux_64_client}
mv ActionFPS/bin_unix/{native_server,linux_64_server}
mkdir -p ActionFPS/{screenshots,demos}/
find . -iname shadows.dat | xargs rm -f
find . -iname .gitignore | xargs rm -f
rm -rf ActionFPS/config/{authkey,history,saved.cfg,servers.cfg,mapmodelattributes.cfg,opt,autosave.cfg,autostart}
for s in 16 22 32 48 64 128; do
  convert ActionFPS/packages/misc/icon.png -resize "${s}x${s}" "ActionFPS/packages/misc/icon-${s}.png";
done
tar cjf ActionFPS.tar.bz2 ActionFPS
