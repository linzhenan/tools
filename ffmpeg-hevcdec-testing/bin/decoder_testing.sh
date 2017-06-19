WORKDIR_PATH=/sdcard/zhenan
BITSTREAM_EXTNAME=h265
FFMPEG_PATH=/data/local/tmp/zhenan/bin/ffmpeg

PLATFORM=$1
THREAD=$2
RE=$3
VERSION=3.2.4

FFREPORT_PATH=${WORKDIR_PATH}/${PLATFORM}_thread${THREAD}_${VERSION}_ffreport
TOPINFO_PATH=${WORKDIR_PATH}/${PLATFORM}_thread${THREAD}_${VERSION}_topinfo
rm -fr $FFREPORT_PATH
rm -fr $TOPINFO_PATH
mkdir -p $FFREPORT_PATH
mkdir -p $TOPINFO_PATH

echo $FFMPEG_PATH
echo $FFREPORT_PATH
echo $TOPINFO_PATH

rm -f *.log

for resolution in 360p 480p 720p
do
	bitstream_path=${WORKDIR_PATH}/hevc-bitstream/${resolution}
	for filename in `ls ${bitstream_path}/*.${BITSTREAM_EXTNAME}`
	do
		echo $filename
		
		topinfo=${filename}.top
		rm -f $topinfo
		
		$FFMPEG_PATH -report -threads $THREAD $RE -i $filename -f rawvideo -pix_fmt yuv420p -y /dev/null 1>/dev/null 2>&1 &
		
		stdout=$(top -n 1 | grep ffmpeg)
		while [ "$stdout" != "" ]
		do
			echo "$stdout" >> $topinfo
			stdout=$(top -n 1 | grep ffmpeg)
		done
	done
	mv ${bitstream_path}/*.top $TOPINFO_PATH
done

cp *.log $FFREPORT_PATH
rm -f *.log
