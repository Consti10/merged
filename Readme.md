This code originated from https://pan.baidu.com/s/10ZlibhmVzezX7H5eWrBEYA 
Intital steps:
1) unzip the newer sdk
2) remove buildroot/build directory and replace it with the corresponding rep from the "older sdk"
3) opened build.sh and uncommented the crap in line 778

To make the A201 board compatible with imx415 connected to csi0, I have created new .dts and .mk files \
1) kernel/arch/arm/boot/dts created rv1126-openhd.dts
2) device/rockchip/rv1126_rv1109 created BoardConfig-openhd (copy paste BoardConfig-aybering & specified to use the above .dts file)
 
 
# Building info for development team
1) similar as described in the A201 doc, the 2 following commands have to performed once before building.
NOTE: Use the openhd config file, as described in the following !

source envsetup.sh 
-> select 78 rockchip_rv1126_rv1109

build.sh lunch
-> select 8 BoardConfig-openhd.mk

2) once the above steps have been performed, you can build with ./build.sh \
this might take a considerable amount of time the first time.
After the build was successfull, you should see a new directory under "IMAGE" called "RV1126-OPENHD_20210727.0227_RELEASE_TEST" or similar.

# Flashing info for development team
After a succesfull build, you can flash the generated update.img file onto the dev board
1) open the rkdev tool (windows onyl)
2) change to bootloader mode
3) flash the image by selecting the generated update.img file, then clicking flash. this should take not more than ~30 seconds.

NOTE: There are probably easier ways, figure them out ;)

# Advanced flashing
Re-building and re-flashing the whole image is sometimes the only way, but not always.
For example, if I modify code under the "external" directory, one can just re-build this part and manually update it on the running dev board
either via ssh or adb (I prefer adb). Look at my 2 scripts in the root of how to do so (consti_flash_my_bin.sh / consti_rebuild_rkmedia.sh )

NOTE: to use adb, you have to have an usb connection - for ssh you need to have an ethernet connection.

# Using the imx415
1) connect the sensor to the csi0 port
2) Depending how the imx415.c driver is setup, you need different iqfiles to get an image. Ask in telegram for that.
3) You can use the binaries under /oem/usr/bin to test and create a simple raw h264 udp stream (telegram).




