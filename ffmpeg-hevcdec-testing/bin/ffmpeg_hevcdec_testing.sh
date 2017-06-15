BITSTREAM_FOLDER=/sdcard/zhenan/hevc-bitstream
BITSTREAM_EXTNAME=h265
FFMPEG_PATH=/system/zhenan/bin/ffmpeg

echo $FFMPEG_PATH
for resolution in 360p 480p 720p
do
	for filename in `ls $BITSTREAM_FOLDER/$resolution/*.$BITSTREAM_EXTNAME`
	do
		echo $filename
		
		#runcmd="$FFMPEG_PATH -i $filename -f rawvideo -pix_fmt yuv420p -y /dev/null"
		#stderr=$($runcmd 2>&1)
		#echo "$stderr" # no <CR> without ""
		
		ffmpeg_log=$filename.ffmpeg
		top_log=$filename.top
		rm -f $ffmpeg_log
		rm -f $top_log
		
		#FFREPORT_CFG=FFREPORT=file=$ffmpeg_log
		#$FFREPORT_CFG $FFMPEG_PATH -i $filename -f rawvideo -pix_fmt yuv420p -y /dev/null 2>&1 &
		$FFMPEG_PATH -report -threads 1 -i $filename -f rawvideo -pix_fmt yuv420p -y /dev/null 1>/dev/null 2>&1 &
		
		stdout=$(top -n 1 | grep ffmpeg)
		while [ "$stdout" != "" ]
		do
			echo "$stdout" >> $top_log
			stdout=$(top -n 1 | grep ffmpeg)
		done
	done
done
