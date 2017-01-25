import bdpsnr_limited

class encoder:
    name = ""
    preset = ""
    speed = 0.0
    psnr = ()
    ssim = ()
    rate = ()
    size = ()
    def __init__(self):
        pass
    def set_encoder(self, name, preset, speed):
        self.name = name
        self.preset = preset
        self.speed = speed
    def add_record(self, psnr, ssim, size, rate):
        self.psnr = self.psnr + (float(psnr),)
        self.ssim = self.ssim + (float(ssim),)
        self.size = self.size + (int(size),)
        self.rate = self.rate + (int(rate),)
    def get_psnr_metric(self):
        return zip(self.rate, self.psnr)
    def get_ssim_metric(self):
        return zip(self.rate, self.ssim)

class video:
    name = ""
    encoders = ()
    def __init__(self):
        name = ""
    def set_vidname(self, name):
        self.name = name
    def add_encoder(self, encoder):
        encoders = encoders + (encoder,)
    def cls_encoder(self):
        encoders = ()

results = {}
current_video_name = 'invalid_video_name'
currnet_encoder_name = 'invalid_encoder_name'

with open ('run4.info') as f:
    line = f.readline()
    v = video()
    e = encoder()
    while line:
        line = line.replace(" ","")
        line = line.replace("\n","")
        elems = line.split('\t')
        print elems
        if len(elems) == 1:
            current_video_name = elems[0]
            results[current_video_name] = {}
        else:
            current_encoder_name = elems[0]
            if not results[current_video_name].has_key(current_encoder_name):
                results[current_video_name][current_encoder_name] = encoder() 
                results[current_video_name][current_encoder_name].set_encoder(elems[1], elems[0].split('_')[1], float(elems[3]))
            results[current_video_name][current_encoder_name].add_record(elems[4], elems[5], elems[6], elems[7])
        line = f.readline()

print results['VID_20160725_091502.mp4']['x264_veryfast'].get_psnr_metric()
print results['VID_20160725_091502.mp4']['x265_veryfast'].get_psnr_metric()

#the first metric_set is reference, and the second one is tested.
print bdpsnr_limited.bdrate(results['VID_20160725_091502.mp4']['x264_veryfast'].get_psnr_metric(), results['VID_20160725_091502.mp4']['x265_veryfast'].get_psnr_metric())

encoder_list = ['h264_qsv_veryfast', 'hevc_qsv_hw_veryfast', 'hevc_qsv_sw_veryfast', 'x264_veryfast', 'x265_veryfast']#, 'qy265_veryfast']

for video_name, encoders in results.iteritems():
    line = video_name
    for encoder_name in encoder_list:
        bdrate = bdpsnr_limited.bdrate(encoders['x264_veryfast'].get_psnr_metric(), encoders[encoder_name].get_psnr_metric())
        line = line + ' ' + str(bdrate)
    print line
