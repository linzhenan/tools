input=$1
output=$2

mkdir -p tmp

#method1
ffmpeg -i $input -an -vf showinfo tmp/%10d.png 2>tmp/info1.txt
grep -oE pts_time:-?[0-9.]+ tmp/info1.txt | awk '{print substr($1,10)}' > 1$output

#method2
ffmpeg -i $input -copyts -an -vf showinfo tmp/%10d.png 2>tmp/info2.txt
grep -oE pts_time:-?[0-9.]+ tmp/info2.txt | awk '{print substr($1,10)}' > 2$output

#method3
ffprobe -i 1.flv -show_packets -select_streams v 2>/dev/null | grep -oE "pts_time=-?[0-9.]+" | awk '{print substr($1,10)}' >3$output

rm -fr tmp
