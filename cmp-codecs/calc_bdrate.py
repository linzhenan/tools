import bdpsnr_limited.py

class encoder:
    name = ""
    preset = ""
    speed = 0.0
    psnr = ()
    ssim = ()
    rate = ()
    size = ()
    bdrate_psnr = 0.0
    bdrate_ssim = 0.0
    def __init__(self):
        pass
    def set_encoder(self, name, preset, speed):
        self.name = name
        self.preset = preset
        self.speed = speed
    def add_record(self, psnr, ssim, size, rate):
        self.psnr = self.psnr + (psnr,)
        self.ssim = self.ssim + (ssim,)
        self.size = self.size + (size,)
        self.rate = self.rate + (rate,)
        if len(self.psnr) == 4:
            pass
    def cls_encoder(self):
        name = ""
        preset = ""
        speed = 0.0
        psnr = ()
        ssim = ()
        rate = ()
        size = ()
        bdrate_psnr = 0.0
        bdrate_ssim = 0.0

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

videos = ()

with open ('run4.info') as f:
    line = f.readline()
    v = video()
    e = encoder()
    while line:
        line = f.readline()
        line = line.replace(" ","")
        line = line.replace("\n","")
        elems = line.split('\t')
        if len(elems) > 1:
            pass    
        else:
            v.cls_encoder()
            v.set_vidname(elems[0])
        print elems

