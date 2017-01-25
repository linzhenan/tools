#!/bin/bash

echo "" > run.log

source $(dirname $0)/common.sh

SRC_VIDEOS_DIR=../selected50
ENC_CONFIG_DIR=conffinal
ENC_REPORT_DIR=encode_reports
ENC_VIDEOS_DIR=encoded_videos

FFMPEG_PATH=/home/zhangjiejun/dev/kuaishou-ffmpeg/ffmpeg

function encode_x264() {
    input=$1
    output=$2

    calc_crop_width_height $ec_width $ec_height
    calc_scale_width_height $ec_widht $ec_height

    crop_filter="$crop_width:$crop_height"
    scale_filter="$scale_width:$scale_height"

    run_cmd="$FFMPEG_PATH -i $input \
        -vf crop=$crop_filter,scale=$scale_filter \
        -an \
        -threads $ec_threads \
        -movflags +faststart \
        -preset:v $ec_preset \
        -r:v $ec_fps \
        -b:v $ec_bitrate \
        -c:v libx264 \
        -x264-params $ec_params \
        -y $output"
    #echo $run_cmd>>run.log

    start_time=$($DATE_CMD +"%s%3N")
    stdout=$($run_cmd 2>&1)
    end_time=$($DATE_CMD +"%s%3N")
}

function encode_x265() {
    input=$1
    output=$2

    calc_crop_width_height $ec_width $ec_height
    calc_scale_width_height $ec_widht $ec_height

    crop_filter="$crop_width:$crop_height"
    scale_filter="$scale_width:$scale_height"

    run_cmd="$FFMPEG_PATH -i $input \
        -vf crop=$crop_filter,scale=$scale_filter \
        -an \
        -threads $ec_threads \
        -movflags +faststart \
        -preset:v $ec_preset \
        -r:v $ec_fps \
        -b:v $ec_bitrate \
        -c:v libx265 \
        -x265-params $ec_params \
        -y $output"
    #echo $run_cmd>>run.log

    start_time=$($DATE_CMD +"%s%3N")
    stdout=$($run_cmd 2>&1)
    end_time=$($DATE_CMD +"%s%3N")
}

function encode_qy265() {
    input=$1
    output=$2

    calc_crop_width_height $ec_width $ec_height
    calc_scale_width_height $ec_widht $ec_height

    crop_filter="$crop_width:$crop_height"
    scale_filter="$scale_width:$scale_height"

    run_cmd="sudo $FFMPEG_PATH -i $input \
        -vf crop=$crop_filter,scale=$scale_filter \
        -an \
        -threads $ec_threads \
        -movflags +faststart \
        -preset:v $ec_preset \
        -r:v $ec_fps \
        -b:v $ec_bitrate \
        -c:v libqy265 \
        -qy265-params $ec_params \
        -y $output"
    echo $run_cmd>>run.log

    start_time=$($DATE_CMD +"%s%3N")
    stdout=$($run_cmd 2>&1)
    echo "$stdout">>run.log
    end_time=$($DATE_CMD +"%s%3N")
}

function encode_h264_qsv() {
    input=$1
    output=$2
  
    calc_crop_width_height $ec_width $ec_height
    calc_scale_width_height $ec_widht $ec_height

    crop_filter="$crop_width:$crop_height"
    scale_filter="$scale_width:$scale_height"

    run_cmd="$FFMPEG_PATH -i $input \
        -vf crop=$crop_filter,scale=$scale_filter \
        -an \
        -threads $ec_threads \
        -movflags +faststart \
        -preset:v $ec_preset \
        -r:v $ec_fps \
        -b:v $ec_bitrate \
        -c:v h264_qsv \
        -y $output"
    #echo $run_cmd>>run.log

    start_time=$($DATE_CMD +"%s%3N")
    stdout=$($run_cmd 2>&1)
    end_time=$($DATE_CMD +"%s%3N")
}

function encode_hevc_qsv() {
    input=$1
    output=$2
  
    calc_crop_width_height $ec_width $ec_height
    calc_scale_width_height $ec_widht $ec_height

    crop_filter="$crop_width:$crop_height"
    scale_filter="$scale_width:$scale_height"

    start_time=$($DATE_CMD +"%s%3N")
    
    run_cmd="$FFMPEG_PATH -i $input \
        -vf crop=$crop_filter,scale=$scale_filter \
        -an \
        -threads $ec_threads \
        -movflags +faststart \
        -preset:v $ec_preset \
        -tune:v $ec_preset \
        -r:v $ec_fps \
        -b:v "$ec_bitrate" \
        -c:v hevc_qsv \
        -load_plugin $ec_encoder \
        -y $output"
    #echo $run_cmd>>run.log
    
    start_time=$($DATE_CMD +"%s%3N")
    stdout=$($run_cmd 2>&1)
    end_time=$($DATE_CMD +"%s%3N")
}

mkdir -p $ENC_REPORT_DIR
mkdir -p $ENC_VIDEOS_DIR

conf_files=$(ls -1 $ENC_CONFIG_DIR/*.conf)
for conf in $conf_files; do
    mkdir -p $ENC_VIDEOS_DIR/$(basename $conf .conf)
    rm -f $ENC_REPORT_DIR/$(basename $conf .conf)*
    rm -f $ENC_VIDEOS_DIR/$(basename $conf .conf)/*
done

for vid_file in $(ls -1 $SRC_VIDEOS_DIR | sort -n)
do
    echo -e "$vid_file"
    input=$SRC_VIDEOS_DIR/$vid_file
    ffprobe_get_input_info $input

    for conf_file in $conf_files
    do
        conf_name=$(basename $conf_file .conf)
        
        eval "$(cat $conf_file)"
        br1=300k
        br2=400k
        br3=800k
        br4=1200k
        if [[ "$ec_encoder" = "x264" ]] || [[ "$ec_encoder" = "h264_qsv" ]]; then
            br1=500k
            br2=1000k
            br3=1500k
            br4=2000k
        fi
        
        for br in $br1 $br2 $br3 $br4
        do
            ec_bitrate=$br
            report_file=$ENC_REPORT_DIR/$conf_name_$ec_bitrate.txt
            output=$ENC_VIDEOS_DIR/$conf_name/$(basename $input .mp4)_$ec_bitrate.mp4

            if [ "$ec_encoder" = "x264" ]; then
                encode_x264 $input $output
            fi
            if [ "$ec_encoder" = "h264_qsv" ]; then
                encode_h264_qsv $input $output
            fi
            if [ "$ec_encoder" = "hevc_sw" ]; then
                encode_hevc_qsv $input $output
            fi
            if [ "$ec_encoder" = "hevc_hw" ]; then
                 encode_hevc_qsv $input $output
            fi
            if [ "$ec_encoder" = "x265" ]; then
                encode_x265 $input $output
            fi
            if [ "$ec_encoder" = "qy265" ]; then
                encode_qy265 $input $output
            fi
            
            process_time=$((end_time - start_time))
            bitrate=$(extract_bitrate $output )
            nb_frames=$(ffprobe_nb_frames $output 2>/dev/null)
            process_fps=$(bc <<< "scale=2; $nb_frames * 1000 / $process_time")
            size=$(extract_size $output)
            
            tmp=tmp.mp4
            
            run_cmd="$FFMPEG_PATH -i $input -vf crop=$crop_filter,scale=$scale_filter -an -threads 8 -movflags +faststart -r $ec_fps -c:v libx264 -preset ultrafast -crf 0 -y $tmp"
            echo $run_cmd>>run.log
            stdout=$($run_cmd 2>&1)
            
            run_cmd="$FFMPEG_PATH -i $output -i $tmp -lavfi ssim;[0:v][1:v]psnr -f null -"
            echo $run_cmd>>run.log
            stdout=$($run_cmd 2>&1)
            
            psnr=$(extract_psnr_filter "$stdout")
            ssim=$(extract_ssim_filter "$stdout")
            
            rm $tmp
        
            line="$conf_name \\t $ec_encoder \\t $vid_file \\t $process_fps \\t $psnr \\t $ssim \\t $size \\t $bitrate"
            echo -e "$line"
            echo -e "$line" >> $report_file
        done
    done
done
