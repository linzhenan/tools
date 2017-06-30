import glob
import os
import re
import sys

BITSTREAM_INFO=[#filename, resolution, framerate(fps), filesize(B)
['afterMakeup_1280x720p.mp4.h265','1280x720',59.94,22404934,],
['badmitton_1280x720p.mp4.h265','1280x720',25,21195393,],
['basketball_1280x720p.mp4.h265','1280x720',23.98,2418238,],
['blessTheCountry_1280x720p.mp4.h265','1280x720',29.83,708419,],
['bmw_1280x720p.mp4.h265','1280x720',25,11808514,],
['cucumber_1280x720p.mp4.h265','1280x720',30,2843683,],
['femaleSinger_1280x720p.mp4.h265','1280x720',30,26631571,],
['flying_1280x720p.mp4.h265','1280x720',25,9196620,],
['fourSinger_1280x720p.mp4.h265','1280x720',23,16166522,],
['hyundaiMotor_1280x720p.mp4.h265','1280x720',29.97,10082247,],
['iphone6Show_1280x720p.mp4.h265','1280x720',23.98,9751846,],
['mobilephoneGame_1280x720p.mp4.h265','1280x720',25,24476116,],
['mouseImageVideo_1280x720p.mp4.h265','1280x720',30,4325516,],
['mtv_1280x720p.mp4.h265','1280x720',25,20101165,],
['poscheShow_1280x720p.mp4.h265','1280x720',25,19973097,],
['riverJumper_1280x720p.mp4.h265','1280x720',29.97,16228625,],
['runningMenTV_1280x720p.mp4.h265','1280x720',25,12341023,],
['skiing_1280x720p.mp4.h265','1280x720',23.98,4519701,],
['tjEntertainment_1280x720.mp4.h265','1280x720',25,19852829,],
['trappedGoat_1280x720p.mp4.h265','1280x720',30,38148248,],
['colorfulChina_640x352p.mp4.h265','640x352',15,17039823,],
['daPengInjection_640x360p.mp4.h265','640x360',25,2083020,],
['finacialNews_640x360p.mp4.h265','640x360',25,3085784,],
['footballGerman_640x360p.mp4.h265','640x360',25,4241500,],
['helicopterGirl_640x360p.mp4.h265','640x360',29.97,6784571,],
['kilivilaIsland_640x360p.mp4.h265','640x360',15,9842993,],
['liyiranInterview_640x360p.mp4.h265','640x360',15,2037798,],
['chineseFootball_854x480p.mp4.h265','854x480',25,12666062,],
['kobeInterview_854x480p.mp4.h265','854x480',25,14868749,],
['lilianjie_854x480p.mp4.h265','854x480',29.97,20123570,],
['northKoreaGirl_854x480p.mp4.h265','854x480',15,14444846,],
['pgc1_854x480p.mp4.h265','854x480',23.98,2928600,],
['pgc2_854x480p.mp4.h265','854x480',29.97,19928992,],
['pgc3_854x480p.mp4.h265','854x480',30,6185370,],
['pgc4_854x480p.mp4.h265','854x480',25,5884653,],
['pgc5_854x480p.mp4.h265','854x480',30,10652418,],
['pgc6_854x480p.mp4.h265','854x480',25,8074117,],
['simpleGraphics_854x480p.mp4.h265','854x480',25,2065918,],
['snailGame_854x480p.mp4.h265','854x480',30,7328950,],
]

class Result:
	file_name  = ""
	resolution = ""
	frame_rate = 0.0#fps
	file_size  = 0	#B
	frame_cnt  = 0
	dec_speed  = 0.0#fps
	q          = 0.0
	yuv_size   = 0	#KB
	duration   = "" #mm:ss.0
	duration_s = 0.0#sec
	yuvrate    = 0.0#kbps 
	bitrate    = 0.0#bps
	speedup    = 0.0
	max_cpu    = 0  #%
	max_vss    = 0  #KB
	max_rss    = 0  #KB
        min_cpu    = 0  #%
        min_vss    = 0  #KB
        min_rss    = 0  #KB
	avg_cpu    = 0.0#%
	avg_vss	   = 0.0#KB
	avg_rss    = 0.0#KB
	def __init__(self):
		pass

resultSet = {}
for bs in BITSTREAM_INFO:
	resultSet[bs[0]] = Result()
	resultSet[bs[0]].file_name  = bs[0]
	resultSet[bs[0]].resolution = bs[1]
	resultSet[bs[0]].frame_rate = bs[2]
	resultSet[bs[0]].file_size  = bs[3]

class Record:
	PID = ""
	PR = ""
	CPU = ""
	S = ""
	THR = ""
	VSS = ""
	RSS = ""
	PCY = ""
	UID = ""
	Name = ""
	def __init__(self, PID, PR, CPU, S, THR, VSS, RSS, PCY, UID, Name):
		self.PID = PID
		self.PR  = PR
		self.CPU = CPU
		self.S   = S
		self.THR = THR
		self.VSS = VSS
		self.RSS = RSS
		self.PCY = PCY
		self.UID = UID
		self.Name= Name

class RecordSet:
	records = ()
	sum_CPU = 0#%
	sum_VSS = 0#k
	sum_RSS = 0#K
	max_CPU = 0
	max_VSS = 0
	max_RSS = 0
	min_CPU = 10000000
	min_VSS = 10000000
	min_RSS = 10000000
	def __init__(self):
		pass
	def add(self, record):
		self.records = self.records + (record, )
		self.max_CPU = max(self.max_CPU, record.CPU)
		self.max_VSS = max(self.max_VSS, record.VSS)
		self.max_RSS = max(self.max_RSS, record.RSS)
		self.min_CPU = min(self.min_CPU, record.CPU)
		self.min_VSS = min(self.min_VSS, record.VSS)
		self.min_RSS = min(self.min_RSS, record.RSS)
		self.sum_CPU = self.sum_CPU + record.CPU
		self.sum_VSS = self.sum_VSS + record.VSS
		self.sum_RSS = self.sum_RSS + record.RSS
	def avg_CPU(self):
		return 1.0 * self.sum_CPU / len(self.records)
	def avg_VSS(self):
		return 1.0 * self.sum_VSS / len(self.records)
	def avg_RSS(self):
		return 1.0 * self.sum_RSS / len(self.records)

def GetFileName(path):
	(file_path, full_file_name) = os.path.split(path)
	(file_name, file_extension) = os.path.splitext(full_file_name)
	return file_name

def ProcessTopLog(path):
	recordSet = RecordSet()
	with open (path) as f:
		#segs = path.split('/')
		#seq = segs[len(segs) - 1]
		#seq = bs[:-4]
		seq = GetFileName(path)
		line = f.readline()
		while line:
			items = line.split()
			if len(items) == 11:
				#PID, "Shell", 20, 0, CPU
				#S, THR, VSS, RSS
				#"", UID, Name
				record = Record(items[0], "", (int)(items[4].strip("%")),\
					items[5], items[6], (int)(items[7].strip("K")), (int)(items[8].strip("K")),\
					"", items[9], items[10])
			elif len(items) == 10:#anchor
				#PID, PR, CPU
				#S, THR, VSS, RSS
				#PCY, UID, Name
				record = Record(items[0], items[1], (int)(items[2].strip("%")), \
					items[3], items[4], (int)(items[5].strip("K")), (int)(items[6].strip("K")), \
					items[7], items[8], items[9]) 
			else:
				#PID, PR, CPU
				#S, THR, VSS, RSS
				#"", UID, Name
				record = Record(items[0], items[1], (int)(items[2].strip("%")), \
					items[3], items[4], (int)(items[5].strip("K")), (int)(items[6].strip("K")), \
					"", items[7], items[8]) 
			recordSet.add(record)
			line = f.readline()
	'''
	print ("%s %d %d %d %d %d %d %.1f %.1f %.1f" %(\
		seq,\
		recordSet.max_CPU,   recordSet.max_VSS,   recordSet.max_RSS,\
		recordSet.min_CPU,   recordSet.min_VSS,   recordSet.min_RSS,\
		recordSet.avg_CPU(), recordSet.avg_VSS(), recordSet.avg_RSS()))
	'''
	resultSet[seq].max_cpu = recordSet.max_CPU
	resultSet[seq].max_vss = recordSet.max_VSS
	resultSet[seq].max_rss = recordSet.max_RSS
	resultSet[seq].min_cpu = recordSet.min_CPU
	resultSet[seq].min_vss = recordSet.min_VSS
	resultSet[seq].min_rss = recordSet.min_RSS
	resultSet[seq].avg_cpu = recordSet.avg_CPU()
	resultSet[seq].avg_vss = recordSet.avg_VSS()
	resultSet[seq].avg_rss = recordSet.avg_RSS()

def ProcessFFReport(path):
	with open (path) as f:
		seq = ""
		cpuCnt = 0
		thrCnt = 0
		actCnt = 0
		line = f.readline()
		while line:
			if line.find("Parsing a group of option") != -1 and line.find(".mp4.h265") != -1 :
				segs = line.split('/')
				seq = segs[len(segs) - 1]
				seq = seq[:-2] # remove '.' and '\n'

			m = re.findall(r'CpuCnt=[0-9]+', line)
			if len(m) > 0:
				cpuCnt = int(m[0].split('=')[1])
			m = re.findall(r'ThrCnt=[0-9]+', line)
			if len(m) > 0:
				thrCnt = int(m[0].split('=')[1])
			m = re.findall(r'ActCnt=[0-9]+', line)
			if len(m) > 0:
				actCnt = int(m[0].split('=')[1])
				print ("%s: CpuCnt %d, ThrCnt %d, ActCnt %d" %(seq, cpuCnt, thrCnt, actCnt))
			
			if line.find("frame=") != -1 and \
			   line.find("fps=")   != -1 and \
			   line.find("q=")     != -1 and \
			   line.find("Lsize=") != -1 and \
			   line.find("time=")  != -1 and \
			   line.find("bitrate=") != -1 and \
			   line.find("speed=") != -1 :
				m = re.findall(r'[0-9|:|\.|\+|-]+', line)
				frame = int(m[0])	#frame number
				fps   = float(m[1])	#decoding speed (fps)
				q     = float(m[2])
				Lsize = int(m[3])	#YUV size	(kB)
				time  = m[4]		#duration time	(hh:mm:ss)
				bitrate = float(m[5])	#bitrate	(kbps)
				speed = float(m[6])	#speedup
				
				m = re.split(':', time)
				hour   = int(m[0])
				minute = int(m[1])
				second = float(m[2])
				dur    = hour * 3600 + minute * 60 + second
				'''
				print ("%s %d %.1f %.1f %d %s %.2f %.1f %.1f" %(seq, frame, fps, q, Lsize, time, dur, bitrate, speed))
				'''
				resultSet[seq].frame_cnt = frame
				resultSet[seq].dec_speed = fps
				resultSet[seq].q = q
				resultSet[seq].yuv_size = Lsize
				resultSet[seq].duration = time
				resultSet[seq].duration_s = dur
				resultSet[seq].yuvrate = bitrate
				resultSet[seq].speedup = speed
			line = f.readline()

def PrintPath(rootDir):
	for lists in os.listdir(rootDir):
		path = os.path.join(rootDir, lists)
		if os.path.isdir(path):
			PrintPath(path)
		else:
			if os.path.splitext(path)[1] == ".top" :
				ProcessTopLog(path)
			if os.path.splitext(path)[1] == ".log" :
				ProcessFFReport(path)			

PrintPath('.')

class Summary:
	count = 0

	min_speed = 1000000.0
	max_speed = 0.0
	sum_speed = 0.0
	
	min_cpu = 100.0
	max_cpu = 0.0
	sum_cpu = 0.0

	min_vss = 1000000.0
	max_vss = 0.0
	sum_vss = 0.0
	
	min_rss = 1000000.0
	max_rss = 0.0
	sum_rss = 0.0

	def __init__(self):
		pass
	def add(self, speed, cpu, vss, rss):
		self.min_speed = min(self.min_speed, speed)
		self.max_speed = max(self.max_speed, speed)
		self.sum_speed += speed
		
		self.min_cpu = min(self.min_cpu, cpu)
		self.max_cpu = max(self.max_cpu, cpu)
		self.sum_cpu += cpu

		self.min_vss = min(self.min_vss, vss)
		self.max_vss = max(self.max_vss, vss)
		self.sum_vss += vss

		self.min_rss = min(self.min_rss, rss)
		self.max_rss = max(self.max_rss, rss)
		self.sum_rss += rss

		self.count += 1

	def avg_speed(self):
		return self.sum_speed / self.count
	def avg_cpu(self):
		return self.sum_cpu / self.count
	def avg_vss(self):
		return self.sum_vss / self.count
	def avg_rss(self):
		return self.sum_rss / self.count

summarySet = {}
summarySet['720p'] = Summary()
summarySet['480p'] = Summary()
summarySet['360p'] = Summary()

'''
print ("Sequence Bitstream, Resolution, Frame Rate (fps), File Size (B), Frame Cnt, Dec Speed (fps), q, YUV size (KB), Duration (mm:ss.0), Duration (sec), Bitrate (bps), Speedup, Max CPU (%), Max VSS (KB), Max RSS (KB), Min CPU (%), Min VSS (KB), Min RSS (KB), Avg CPU (%), Avg VSS (KB), Avg RSS (KB),") 
'''
for filename, result in resultSet.iteritems():
	result.bitrate = result.file_size * 8.0 / result.duration_s
	'''	
	print ("%s,%s,%.2f,%d,%d,%.1f,%.1f,%d,%s,%.1f,%.1f,%.1f,%d,%d,%d,%d,%d,%d,%.1f,%.1f,%.1f" %( 
		result.file_name,\
		result.resolution,\
		result.frame_rate,\
		result.file_size,\
		result.frame_cnt,\
		result.dec_speed,\
		result.q,\
		result.yuv_size,\
		result.duration,\
		result.duration_s,\
		result.bitrate,\
		result.speedup,\
		result.max_cpu,\
		result.max_vss,\
		result.max_rss,\
		result.min_cpu,\
		result.min_vss,\
		result.min_rss,\
		result.avg_cpu,\
		result.avg_vss,\
		result.avg_rss))
	'''
	res = 'unknown'
	if result.resolution.find('x720') != -1:
		res = '720p'
	if result.resolution.find('x480') != -1:
		res = '480p'
	if result.resolution.find('x360') != -1 or result.resolution.find('x352') != -1:
		res = '360p'
	summarySet[res].add(result.dec_speed, result.avg_cpu, result.avg_vss, result.avg_rss)

print("Resolution,Avg Speed (fps),Avg CPU (%),Avg VSS (MB), Avg RSS (MB),")
for res in ['720p', '480p', '360p']:
	print ("%s,%.2f (%.1f~%.1f),%.1f (%.1f~%.1f),%.1f (%.1f~%.1f),%.1f (%.1f~%.1f)," %(\
		res,\
		summarySet[res].avg_speed(), summarySet[res].min_speed, summarySet[res].max_speed,\
		summarySet[res].avg_cpu()       , summarySet[res].min_cpu       , summarySet[res].max_cpu,\
		summarySet[res].avg_vss() / 1000, summarySet[res].min_vss / 1000, summarySet[res].max_vss / 1000,\
		summarySet[res].avg_rss() / 1000, summarySet[res].min_rss / 1000, summarySet[res].max_rss / 1000)) 		
