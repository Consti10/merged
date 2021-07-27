#!/bin/sh
a=0
a=$1

if [ $a -eq 2 ];then
	/app/rkmedia_audio_test 2 8000 /app/8k_1_16.g711a 
elif [ $a -eq 1 ];then
	/app/rkmedia_audio_test 1 8000 /app/testAudio.g711a
else	
	echo "usage: ./test_audio.sh 2 ====audio adec ,source file at /app/8k_1_16.g711a"
	echo "./test_audio.sh 1 =====audio aenc ,file generate at /app/testAudio.g711a"
fi
