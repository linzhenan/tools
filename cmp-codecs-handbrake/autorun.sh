SEQS=(\
'1280x640_0.y4m' \
'1280x720_0.y4m' \
'1280x720_1.y4m' \
'1280x720_2.y4m' \
'1280x720_3.y4m' \
'1280x720_4.y4m' \
'1920x1080_0.y4m' \
'1920x1080_1.y4m' \
'3840x2160_0.y4m' \
'4096x2048_0.y4m' \
'720x480_0.y4m' \
'720x480_1.y4m' \
'720x480_2.y4m' \
'720x576_0.y4m' \
'720x576_1.y4m' \
'720x576_2.y4m' \
'720x576_3.y4m' \
'720x576_4.y4m' \
'720x576_5.y4m' \
'720x576_6.y4m' \
'720x576_7.y4m' \
'720x576_8.y4m')

SRCDIR=root@10.10.144.161:/mnt/ec-data1/codec_compare_y4m
FFMPEG=/data/ffmpeg-3.3.3/ffmpeg
OUTPUT=result.txt

source $(dirname $0)/common.sh

rm -f $OUTPUT

for file in ${SEQS[*]}
do
	res=${file%_*}

	if [ "$res"x == "720x480"x ]
	then
		res_abbr="NTSC"
		bit_rate=600
	elif [ "$res"x == "720x576"x ]
	then
		res_abbr="PAL"
		bit_rate=600
	elif [ "$res"x == "1280x640"x ]
	then
		res_abbr="1280x640"
		bit_rate=1500
	elif [ "$res"x == "1280x720"x ]
	then
		res_abbr="HD720"
		bit_rate=1500
	elif [ "$res"x == "1920x1080"x ]
	then
		res_abbr="HD1080"
		bit_rate=2500
	elif [ "$res"x == "3840x2160"x ]
	then
		res_abbr="UHD2160"
		bit_rate=6000
	elif [ "$res"x == "4096x2048"x ]
	then
		res_abbr="4096x2048"
		bit_rate=6000
	fi

	scp $SRCDIR/$file .

	$FFMPEG -i $file -ss 1 -frames 1 -y $file.jpg
	
	start_time=$($DATE_CMD +"%s%3N")
	./HandBrakeCLI --crop 0:0:0:0 -i $file -e qsv_h264 -x "tu=1:trellis=3:b-pyramid=1:ref=4" --vb $bit_rate -o $file.qsv.mp4
	end_time=$($DATE_CMD +"%s%3N")
	qsv_time=$((end_time - start_time))
	$FFMPEG -i $file -i $file.qsv.mp4 -lavfi psnr=stats_file=$file.psnr.log -f null -
	qsv_psnr=$(parse_psnr_log $file.psnr.log)
	qsv_bitrate=$(extract_bitrate $file.qsv.mp4)
	qsv_frame_num=$(ffprobe_nb_frames $file.qsv.mp4)
	qsv_frame_rate=$(ffprobe_fps $file.qsv.mp4)
	rm $file.psnr.log
	rm $file.qsv.mp4

	start_time=$($DATE_CMD +"%s%3N")
	./HandBrakeCLI --crop 0:0:0:0 -i $file -e x264  --encoder-preset  veryslow --vb $bit_rate -o $file.x264.mp4
	end_time=$($DATE_CMD +"%s%3N")
	x264_time=$((end_time - start_time))
	$FFMPEG -i $file -i $file.x264.mp4 -lavfi psnr=stats_file=$file.psnr.log -f null -
	x264_psnr=$(parse_psnr_log "$file.psnr.log")
	x264_bitrate=$(extract_bitrate $file.x264.mp4)
	x264_frame_num=$(ffprobe_nb_frames $file.x264.mp4)
	x264_frame_rate=$(ffprobe_fps $file.x264.mp4)
	rm $file.psnr.log
	rm $file.x264.mp4

	rm $file

	echo "$res $res_abbr $file \
		QSV $qsv_frame_num $qsv_frame_rate $qsv_bitrate $qsv_psnr $qsv_time \
		X264 $x264_frame_num $x264_frame_rate $x264_bitrate $x264_psnr $x264_time" >> $OUTPUT
done

