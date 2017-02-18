#!/usr/bin/env python

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

selected_result_files = ['selected50.txt', 'selected50_faster.txt']

for result_file in selected_result_files:
    with open (result_file) as f:
        line = f.readline()
        while line:
            line = line.replace(" ","")
            line = line.replace("\n","")
            elems = line.split('\t')
            if len(elems) == 1:
                current_video_name = elems[0]
                if not results.has_key(current_video_name):
                    results[current_video_name] = {}
            else:
                current_encoder_name = elems[0]
                if not results[current_video_name].has_key(current_encoder_name):
                    results[current_video_name][current_encoder_name] = encoder() 
                    results[current_video_name][current_encoder_name].set_encoder(elems[1], elems[0].split('_')[1], float(elems[3]))
                print elems
                # results[current_video_name][current_encoder_name].add_record(elems[8], elems[5], elems[6], elems[7])
                results[current_video_name][current_encoder_name].add_record(elems[4], elems[5], elems[6], elems[7])
            line = f.readline()

#the first metric_set is reference, and the second one is tested.
#print results['VID_20160725_091502.mp4']['x264_veryfast'].get_psnr_metric()
#print results['VID_20160725_091502.mp4']['x265_veryfast'].get_psnr_metric()
#print bdpsnr_limited.bdrate(results['VID_20160725_091502.mp4']['x264_veryfast'].get_psnr_metric(), results['VID_20160725_091502.mp4']['x265_veryfast'].get_psnr_metric())

def time_inc_rate(fps_ref, fps_test):
    return (fps_ref / fps_test - 1) * 100

tested_encoder_list = ['x264_medium', 'hevc_qsv_hw_veryfast', 'hevc_qsv_sw_veryfast', 'qy265_veryfast', 'x264_veryfast', 'qy265_faster', 'hevc_qsv_hw_faster', 'hevc_qsv_sw_faster']
anchor_encoder = 'x264_veryfast'

f = open('BDBR_PSNR_selected50.txt', 'w')
print >>f, 'filename', '\t', '\t'.join(tested_encoder_list)
for video_name, encoders in results.iteritems():
    line = video_name
    for encoder_name in tested_encoder_list:
        bdrate = bdpsnr_limited.bdrate(encoders[anchor_encoder].get_psnr_metric(), encoders[encoder_name].get_psnr_metric())
        line = line + '\t' + str(bdrate)
    print >>f, line
f.close()

f = open('BDBR_SSIM_selected50.txt', 'w')
print >>f, 'filename', '\t', '\t'.join(tested_encoder_list)
for video_name, encoders in results.iteritems():
    line = video_name
    for encoder_name in tested_encoder_list:
        bdrate = bdpsnr_limited.bdrate(encoders[anchor_encoder].get_ssim_metric(), encoders[encoder_name].get_ssim_metric())
        line = line + '\t' + str(bdrate)
    print >>f, line
f.close()


f = open('TimeInc.txt', 'w')
print >>f, 'filename', '\t', '\t'.join(tested_encoder_list)
for video_name, encoders in results.iteritems():
    line = video_name
    for encoder_name in tested_encoder_list:
        incrate = time_inc_rate(encoders[anchor_encoder].speed, encoders[encoder_name].speed)
        line = line + '\t' + str(incrate)
    print >>f, line
f.close()
