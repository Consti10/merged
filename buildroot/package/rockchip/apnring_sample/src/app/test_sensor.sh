#!/bin/sh
a=0
a=$1
if [ $a -eq 0 ];then
	/app/rkmedia_vi_venc_rtsp_test  -I 0
else
	/app/rkmedia_vi_venc_rtsp_test -I 1
fi	

