FFPROBE_PATH=/usr/local/bin/ffprobe

if uname | grep -i "darwin"; then
    DATE_CMD=gdate
else
    DATE_CMD=date
fi

function ffprobe_nb_frames() {
    nb_frames=$($FFPROBE_PATH -v error -select_streams v:0 -show_entries stream=nb_frames -of default=noprint_wrappers=1:nokey=1 $1)
    echo $nb_frames
}

function ffprobe_fps() {
  fps=$($FFPROBE_PATH -v error -select_streams v:0 -show_entries stream=avg_frame_rate -of default=noprint_wrappers=1:nokey=1 $1)
  echo $(python -c "print int(${fps}.0 + 0.5)")
}

function ffprobe_rotation() {
    rotation=$($FFPROBE_PATH -v error -select_streams v:0 -show_entries side_data=rotation -of default=noprint_wrappers=1:nokey=1 $1)
    echo $rotation
}

function ffprobe_width() {
  width=$($FFPROBE_PATH -v error -select_streams v:0 -show_entries stream=width -of default=noprint_wrappers=1:nokey=1 $1)
  echo $width
}

function ffprobe_height() {
  height=$($FFPROBE_PATH -v error -select_streams v:0 -show_entries stream=height -of default=noprint_wrappers=1:nokey=1 $1)
  echo $height
}

function ffprobe_duration() {
  duration=$($FFPROBE_PATH -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 $1)
  echo $duration
}

function extract_width() {
  echo "$1" | grep 'rawvideo' | grep -oE 'yuv420p, [0-9]+x[0-9]+' | grep -oE '[0-9]+x' | grep -oE '[0-9]+'
}

function extract_height() {
  echo "$1" | grep 'rawvideo' | grep -oE 'yuv420p, [0-9]+x[0-9]+' | grep -oE 'x[0-9]+' | grep -oE '[0-9]+'
}

function extract_time() {
    echo "$1" | grep -oE 'real\s*[0-9]+(\.[0-9]+)?' | grep -oE '[0-9]+(\.[0-9]+)?'
}

function extract_x264_psnr() {
    echo "$1" | grep '] PSNR' | grep -oE 'Avg:[0-9]+\.[0-9]+' | grep -oE '[0-9]+\.[0-9]+'
}

function extract_psnr_filter() {
    echo "$1" | grep '] PSNR' | grep -oE 'average:[0-9]+\.[0-9]+' | grep -oE '[0-9]+\.[0-9]+'
}

function extract_ssim_filter() {
    echo "$1" | grep '] SSIM' | grep -oE 'All:[0-9]+\.[0-9]+' | grep -oE '[0-9]+\.[0-9]+'
}

function extract_size() {
    ls -l $1 | awk -F ' ' '{print $5}'
}

function extract_bitrate() {
    ffprobe $1 2>&1 | grep 'Video:' | grep -oE '[0-9]+ kb/s' | grep -oE '[0-9]+'
}

function ffprobe_get_input_info() {
    streams_stream_0_side_data_list_side_data_0_rotation=
    streams_stream_0_height=
    streams_stream_0_width=

    input=$1
    output=$($FFPROBE_PATH -v error -select_streams v:0 -show_entries stream=width,height,avg_frame_rate:side_data=rotation -of flat=s=_ $input)
    eval "$output"
    input_rotation=$streams_stream_0_side_data_list_side_data_0_rotation
    if [ -n "$input_rotation" -a $((input_rotation / 90 % 2)) -ne 0 ]; then
        input_width=$streams_stream_0_height
        input_height=$streams_stream_0_width
    else
        input_width=$streams_stream_0_width
        input_height=$streams_stream_0_height
    fi
}

function calc_crop_width_height() {
    ratio_w=$1
    ratio_h=$2

    if [ $((input_width < input_height)) -ne $((ratio_w < ratio_h)) ]; then
        tmp=$ratio_w
        ratio_w=$ratio_h
        ratio_h=$tmp
    fi

    if [ $((input_width * ratio_h)) -gt $((input_height * ratio_w)) ]; then
        # should crop along x axis
        exp_height=$input_height
        exp_width=$((input_height * ratio_w / ratio_h))
        #make sure exp_width is always even
        exp_width=$((exp_width / 2 * 2))
    else
        # crop along y axis
        exp_width=$input_width
        exp_height=$((input_width * ratio_h / ratio_w))
        # make sure exp_height is always even
        exp_height=$((exp_height / 2 * 2))
    fi

    crop_width=$exp_width
    crop_height=$exp_height
}

function calc_scale_width_height() {
    if [ $((input_width < input_height)) -eq $((ec_width < ec_height)) ]; then
        scale_width=$ec_width
        scale_height=$ec_height
    else
        # swap width & height
        scale_width=$ec_height
        scale_height=$ec_width
    fi
}
