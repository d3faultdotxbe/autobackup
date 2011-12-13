#!/bin/sh
read -n1 -r -p "make sure the device is mounted in: /media/FF64-0C05/" key
cd /media/FF64-0C05/DCIM/Camera/
cp -av ./* /mnt/750x1/droid/camera/
cp -av ./* /mnt/750x2/droid/camera/
cd ..
cd ..
cp -av ./recording-*.3gp /mnt/750x1/droidSoundRecorder/
cp -av ./recording-*.3gp /mnt/750x2/droidSoundRecorder/
cd ./DCIM/Camera/
shred -fuv --iterations=1 ./*
cd ..
cd ./.thumbnails/
shred -fuv --iterations=1 ./*
cd ..
cd ..
shred -fuv --iterations=1 ./recording-*.3gp
