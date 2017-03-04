input=$1
output=$2
mkdir -p tmp
ffmpeg -i $input -an -vf showinfo tmp/%10d.png 2>tmp/info.txt
grep -oE pts_time:-?[0-9.]+ tmp/info.txt | awk '{print substr($1,10)}' > $output
rm -fr tmp
