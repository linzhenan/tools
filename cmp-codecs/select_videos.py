import os
import shutil

class video:
	id = 0
	x = 0.0
	y = 0.0
	w = 0
	h = 0
	fps = 0.0
	file = ""
	def __init__(self, id, s_cplx, t_cplx, file, width, height, fps):
		self.id = id
		self.x = s_cplx
		self.y = t_cplx
		self.file = file
		self.w = width
		self.h = height
		self.fps = fps
	def show(self):
		print ("%d %s %.9f %.9f %dx%d %.2ffps " %(self.id, self.file, self.x, self.y, self.w, self.h, self.fps, ))
	def calc_dis(self, s_cplx, t_cplx):
		return pow(s_cplx - x, 2) + pow(t_cplx - y, 2)

class point:
	x = 0.0
	y = 0.0
	def __init__(self, s_cplx, t_cplx):
		self.x = s_cplx
		self.y = t_cplx
	def show(self):
		print ("%.9f %.9f" %(self.x, self.y))

vtup = ()#video tuple
ptup = ()#point tuple
info = ()

i = 0
with open('./doc/sortdata.csv') as f:
	line = f.readline()
	while line:
		line = f.readline()
		elems = line.split(',')
		if len(elems) > 1:
			v = video(i, float(elems[1]), float(elems[2]), elems[9], int(elems[18]), int(elems[19]), float(elems[23]))
			vtup = vtup + (v,)
			info = info + (elems,)
			i = i + 1

with open('./doc/sortdata_anchor.txt') as f:
	line = f.readline()
	while line:
		elems = line.split(' ')
		if (len(elems)) > 1:
			p = point(float(elems[0]), float(elems[1]))
			ptup = ptup + (p,)
		line = f.readline()
		
blacklist = (\
"406434809_old.mp4",\
"407244002_old.mp4",\
"409272834_old.mp4",\
"409293865_old.mp4",\
"582044238_old.mp4",\
"687581167_old.mp4",\
"744660362_old.mp4",\
"946867135_old.mp4",\
"1077435601_old.mp4",\
"367901980_old.mp4",\
"860572611_old.mp4",\
"917462174_old.mp4",\
"1025300207_old.mp4",\
"607606389_old.mp4",\
"631562279_old.mp4",\
"415084503_old.mp4",\
"906529734_old.mp4",\
"839431850_old.mp4",\
"1039962179_old.mp4",\
"1109421933_old.mp4",\
"1136465591_old.mp4",\
"573347832_old.mp4",\
"956362692_old.mp4",\
"VID_20160708_095324.mp4",\
"467267382_old.mp4",\
"573905099_old.mp4",\
"573911023_old.mp4",\
)

sel = ()		
for p in ptup:
	min_dis = 10000.0
	min_id = -1
	for v in vtup:
		if v.w == 1088 or v.h == 1088:
			continue
		if v.id in sel:
			continue
		if v.file in blacklist:
			continue
		v_dis = pow(p.x - v.x, 2) + pow(p.y - v.y, 2)
		if v_dis < min_dis :
			discard = 0
			id = 0
			for id in sel:
				dis = pow(vtup[id].x - v.x, 2) + pow(vtup[id].y - v.y, 2)
				if (dis < 0.0382 * 0.0382): #0.1 - 0.0618 ^_^
					discard = 1
			if discard == 1:
				continue
			min_dis = v_dis
			min_id = v.id
	if min_id != -1:
		sel = sel + (min_id,)
		vtup[min_id].show()

srcdir = "./selected"
dstdir = "./selected50-2"
if os.path.exists(dstdir):
	for file in os.listdir(dstdir):
		os.remove(os.path.join(dstdir, file))
else:
	os.mkdir(dstdir)
for id in sel:
	#vtup[id].show()
	srcfile = os.path.join(srcdir, vtup[id].file)
	dstfile = os.path.join(dstdir, vtup[id].file)
	shutil.copy(srcfile, dstfile)
