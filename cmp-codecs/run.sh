#!/bin/bash

echo "" > run.log

source $(dirname $0)/common.sh

FFMPEG_LOG_LEVEL=error

SRC_VIDEOS_DIR=selected
ENC_CONFIG_DIR=conf
ENC_REPORT_DIR=encode_reports
ENC_VIDEOS_DIR=encoded_videos

FFMPEG_PATH=/home/zhangjiejun/dev/kuaishou-ffmpeg/ffmpeg

function encode_x264() {
    input=$1
    output=$2

    run_cmd="$FFMPEG_PATH -v $FFMPEG_LOG_LEVEL -i $input \
        -threads $ec_threads \
        -vf crop=$crop_filter,scale=$scale_filter
        -movflags +faststart \
        -preset:v $ec_preset \
        -b:v $ec_bitrate \
        -c:v libx264 \
        -psnr \
        -x264-params $ec_params \
        -y $output"
    #echo "$run_cmd">>run.log

    start_time=$($DATE_CMD +"%s%3N")
    stdout=$($run_cmd 2>&1)
    end_time=$($DATE_CMD +"%s%3N")
}

function encode_x265() {
    input=$1
    output=$2

    run_cmd="$FFMPEG_PATH -v $FFMPEG_LOG_LEVEL -i $input \
        -threads $ec_threads \
        -vf crop=$crop_filter,scale=$scale_filter
        -movflags +faststart \
        -preset:v $ec_preset \
        -b:v $ec_bitrate \
        -c:v libx265 \
        -psnr \
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

    run_cmd="sudo $FFMPEG_PATH -v $FFMPEG_LOG_LEVEL -i $input \
        -threads $ec_threads \
        -vf crop=$crop_filter,scale=$scale_filter
        -movflags +faststart \
        -preset:v $ec_preset \
        -b:v $ec_bitrate \
        -c:v libqy265 \
        -qy265-params "$ec_params" \
        -y $output"
    #echo $run_cmd>>run.log

    start_time=$($DATE_CMD +"%s%3N")
    stdout=$($run_cmd 2>&1)
    end_time=$($DATE_CMD +"%s%3N")
}

function encode_h264_qsv() {
    input=$1
    output=$2
  
    run_cmd="$FFMPEG_PATH -v $FFMPEG_LOG_LEVEL -i $input \
        -threads $ec_threads \
        -vf crop=$crop_filter,scale=$scale_filter
        -movflags +faststart \
        -async_depth $ec_async_depth \
        -preset:v $ec_preset \
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

    start_time=$($DATE_CMD +"%s%3N")

    echo "encode_h264_qsv $1 $2 $3" >> run.log
    
    run_cmd="$FFMPEG_PATH -v $FFMPEG_LOG_LEVEL -i $input \
        -threads $ec_threads \
        -vf crop=$crop_filter,scale=$scale_filter
        -movflags +faststart \
        -async_depth $ec_async_depth \
        -preset:v $ec_preset \
        -b:v "$ec_bitrate" \
        -avbr_accuracy 10 \
        -avbr_convergence 1 \
        -c:v hevc_qsv \
        -load_plugin $ec_encoder \
        -y $output"
    #echo $run_cmd>>run.log
    
    start_time=$($DATE_CMD +"%s%3N")
    stdout=$($run_cmd 2>&1)
    end_time=$($DATE_CMD +"%s%3N")
}

function prepare_yuv() {
    calc_crop_width_height
    calc_scale_width_height
    crop_filter="$crop_width:$crop_height"
    scale_filter="$scale_width:$scale_height"
    yuv_input_size="$scale_filter"
    echo "convert $1 to yuv $2, crop_filter: $crop_filter, scale_filter: $scale_filter" >> run.log
    ffmpeg -v $FFMPEG_LOG_LEVEL -i "$1" -vf crop=$crop_filter,scale=$scale_filter -f rawvideo -pix_fmt yuv420p -y $2 >> run.log
}

mkdir -p $ENC_REPORT_DIR
mkdir -p $ENC_VIDEOS_DIR

conf_files=$(ls -1 $ENC_CONFIG_DIR/*.conf)
for conf in $conf_files; do
    mkdir -p $ENC_VIDEOS_DIR/$(basename $conf .conf)
    rm -f $ENC_REPORT_DIR/$(basename $conf .conf)*
    rm -f $ENC_VIDEOS_DIR/$(basename $conf .conf)/*
done

for vid_file in $(ls -1 $SRC_VIDEOS_DIR | grep "\.mp4$" | sort -n)
do
    echo -e "$vid_file"
    mp4_input=$SRC_VIDEOS_DIR/$vid_file
    yuv_input=$SRC_VIDEOS_DIR/$vid_file.yuv
    ffprobe_get_input_info $mp4_input

    for conf_file in $conf_files
    do
        conf_name=$(basename $conf_file .conf)
        
        ec_async_depth=4
        eval "$(cat $conf_file)"
        prepare_yuv $mp4_input $yuv_input

        br1=300k
        br2=600k
        br3=1200k
        br4=2400k
        if [[ "$ec_encoder" = "x264" ]] || [[ "$ec_encoder" = "h264_qsv" ]]; then
            br1=400k
            br2=800k
            br3=1600k
            br4=3200k
        fi
        
        for br in $br1 $br2 $br3 $br4
        do
            echo encoding "$mp4_input" for conf "$conf_name" bitrate "$br" >> run.log
            ec_bitrate=$br
            report_file=$ENC_REPORT_DIR/$conf_name_$ec_bitrate.txt
            output=$ENC_VIDEOS_DIR/$conf_name/$(basename $mp4_input .mp4)_$ec_bitrate.mp4

            encoder_reported_psnr=NULL
            if [ "$ec_encoder" = "x264" ]; then
                encode_x264 $mp4_input $output $scale_filter
                encoder_reported_psnr=$(extract_x264_psnr "$stdout")
            fi
            if [ "$ec_encoder" = "h264_qsv" ]; then
                encode_h264_qsv $mp4_input $output $scale_filter
            fi
            if [ "$ec_encoder" = "hevc_sw" ]; then
                encode_hevc_qsv $mp4_input $output $scale_filter
            fi
            if [ "$ec_encoder" = "hevc_hw" ]; then
                 encode_hevc_qsv $mp4_input $output $scale_filter
            fi
            if [ "$ec_encoder" = "x265" ]; then
                encode_x265 $mp4_input $output $scale_filter
                encoder_reported_psnr=$(extract_x265_psnr "$stdout")
            fi
            if [ "$ec_encoder" = "qy265" ]; then
                encode_qy265 $mp4_input $output $scale_filter
                encoder_reported_psnr=$(extract_qy265_psnr "$stdout")
            fi

            echo "$stdout" >> run.log

            process_time=$((end_time - start_time))
            bitrate=$(extract_bitrate $output )
            nb_frames=$(ffprobe_nb_frames $output 2>/dev/null)
            process_fps=$(bc <<< "scale=2; $nb_frames * 1000 / $process_time")
            size=$(extract_size $output)

            # Convert output mp4 to yuv, and assert if input yuv and output yuv have the same size
            yuv_output=$ENC_VIDEOS_DIR/$conf_name/$(basename $mp4_input .mp4)_${ec_bitrate}.yuv
            echo ">> converting output $output to yuv $yuv_output" >> run.log
            echo ffmpeg -v $FFMPEG_LOG_LEVEL -i "$output" -f rawvideo -pix_fmt yuv420p -y "$yuv_output" >> run.log
            ffmpeg -v $FFMPEG_LOG_LEVEL -i "$output" -f rawvideo -pix_fmt yuv420p -y "$yuv_output" &>> run.log
            yuv_input_size=$(get_file_size "$yuv_input")
            yuv_output_size=$(get_file_size "$yuv_output")
            if [ "$yuv_input_size" = "$yuv_output_size" ]; then
                ssim_stats_file="${output}.ssimstats"
                psnr_stats_file="${output}.psnrstats"
                run_cmd="$FFMPEG_PATH \
                    -f rawvideo -pix_fmt yuv420p -s $scale_width:$scale_height -i $yuv_output \
                    -f rawvideo -pix_fmt yuv420p -s $scale_width:$scale_height -i $yuv_input \
                    -lavfi ssim=stats_file=${ssim_stats_file};[0:v][1:v]psnr=stats_file=${psnr_stats_file} \
                    -f null -"
                echo $run_cmd &>> run.log
                stdout=$($run_cmd 2>&1)
                echo "$stdout" >> run.log
                psnr=$(extract_psnr_filter "$stdout")
                ssim=$(extract_ssim_filter "$stdout")

                average_psnr_per_frame=$(parse_psnr_log "$psnr_stats_file")
                echo ">> average_psnr_per_frame: $average_psnr_per_frame" >> run.log

                # rm -f "$ssim_stats_file"
                # rm -f "$psnr_stats_file"
            else
                echo ">> ERROR! input_size: $yuv_input_size output size: $yuv_output_size does not match" &>> run.log
                psnr=ERR
                ssim=ERR
                average_psnr_per_frame=ERR
                encoder_reported_psnr=ERR
            fi

            rm "$yuv_output"
            
            line="$conf_name \\t $ec_encoder \\t $vid_file \\t $process_fps \\t $psnr \\t $ssim \\t $size \\t $bitrate \\t $average_psnr_per_frame \\t $encoder_reported_psnr"
            echo -e "$line"
            echo -e "$line" &>> $report_file
        done
        echo "deleting yuv" &>> run.log
        rm "$yuv_input"
    done
done
