#!/bin/sh

FFMPEG_PATH=/home/zhangjiejun/dev/kuaishou-ffmpeg/ffmpeg

input=1018992431_old_1080x1920.yuv
ec_threads=8
crop_filter=1080:1920
scale_filter=1080:1920
ec_preset=veryfast
ec_bitrate=500k
ec_params=

output=1018992431_old_1080x1920_x265.265
    run_cmd="$FFMPEG_PATH -s 1080x1920 -i $input -psnr \
        -threads $ec_threads \
        -vf crop=$crop_filter,scale=$scale_filter
        -movflags +faststart \
        -preset:v $ec_preset \
        -b:v $ec_bitrate \
        -c:v libx265 \
        -y $output"

if false; then
output=1018992431_old_1080x1920_qy265.265
    run_cmd="sudo $FFMPEG_PATH -s 1080x1920 -i $input -psnr \
        -threads $ec_threads \
        -vf crop=$crop_filter,scale=$scale_filter
        -movflags +faststart \
        -preset:v $ec_preset \
        -b:v $ec_bitrate \
        -c:v libqy265 \
        -qy265-params psnr=1 \
        -y $output"
fi

if false; then
output=1018992431_old_1080x1920_x264.264
    run_cmd="$FFMPEG_PATH -s 1080x1920 -i $input -psnr \
        -threads $ec_threads \
        -vf crop=$crop_filter,scale=$scale_filter
        -movflags +faststart \
        -preset:v $ec_preset \
        -b:v $ec_bitrate \
        -c:v libx264 \
        -psnr \
        -y $output"
fi

echo "$run_cmd"
stdout=$($run_cmd 2>&1)
echo "$stdout"

ffmpeg -i $output -y $output.yuv

scale_width=1080
scale_height=1920
yuv_output=$output.yuv
yuv_input=$input
ssim_stats_file=ssimstats.txt
psnr_stats_file=psnrstats.txt
                run_cmd="$FFMPEG_PATH \
                    -s $scale_width:$scale_height -i $yuv_output \
                    -s $scale_width:$scale_height -i $yuv_input \
                    -lavfi ssim=stats_file=${ssim_stats_file};[0:v][1:v]psnr=stats_file=${psnr_stats_file} \
                    -f null -"
stdout=$($run_cmd)

./main -w 1080 -h 1920 $input $output.yuv > psnrcalc.txt

rm $output
rm $output.yuv
