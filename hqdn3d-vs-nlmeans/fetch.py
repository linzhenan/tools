import urllib2
import sys
import json
import time
import os

FEEDS_TO_FETCH = 1000
PHOTOS_PER_FEED = 20
OUTPUT_DIR = "./output.10000"
photos = {}

def fetch_thumbnails():
    """ http://wiki.corp.kuaishou.com/pages/viewpage.action?pageId=9994903 """
    pcursor = 0
    for i in xrange(1, FEEDS_TO_FETCH / PHOTOS_PER_FEED):
        cookie = "cdi=web_7120130681; JSESSIONID=0E4FE8347A60239B65A7F0AD80D39E8D; Hm_lvt_86a27b7db2c5c0ae37fee4a8a35033ee=1477484644,1478622296; Hm_lpvt_86a27b7db2c5c0ae37fee4a8a35033ee=1479455227"
        opener = urllib2.build_opener()
        opener.addheaders.append(('Cookie', cookie))
        url = "http://www.kuaishou.com/rest/photos"
        if pcursor == 0:
            c = opener.open("%s?count=%d" % (url, PHOTOS_PER_FEED))
        else:
            c = opener.open("%s?pcursor=%d&count=%d" % (url, pcursor, PHOTOS_PER_FEED))
        d = c.read()
        response = json.loads(d)
        pcursor = int(response["pcursor"])

        new = 0
        for p in response["photos"]:
            photo_id = p["photoId"]
            if photo_id not in photos:
                photos[photo_id] = p
                new += 1
            else:
                pass
                #print p
            #print json.dumps(p["photoId"], indent=4)
            #print p["mp4Url"]
            #print p["thumbnailUrl"]
        print "loop %d, pcursor: %d, got %d photos, new: %d, total: %d" % (i, pcursor, len(response["photos"]), new, len(photos))
        time.sleep(1)
    #sys.exit(0)

    for photo_id in photos:
        orig_png_file = "%s/%d.png" % (OUTPUT_DIR, photo_id)
        photo = photos[photo_id]

        json_file = "%s/%d.json" % (OUTPUT_DIR, photo_id)
        with open(json_file, 'w') as f: 
            print >> f, json.dumps(photo, indent=4)
        
        
        cmd = "ffmpeg -f mp4 -i %s -vframes 1 -f image2 -y %s 2>&1" % (photo["mp4Url"], orig_png_file)
        os.system(cmd)
        print photo["mp4Url"]
        print photo["thumbnailUrl"]

def preprocess_thumbnails():
    html = open("1.html", "w")
    i = 0
    for f in os.listdir(OUTPUT_DIR):
        if f.endswith(".json"): continue

        print f
        #if (i < 100): i+=1; continue 
        #cmd = "ffmpeg -i %s/%s -vf pp=al:f/hqdn3d=1.5:1.5:6:6/ac -f image2 -y processed/%s 2>&1" % (OUTPUT_DIR, f, f)
        #cmd = "ffmpeg -i %s/%s -vf pp=al:f/ac -f image2 -y processed/%s 2>&1" % (OUTPUT_DIR, f, f)
        #cmd = "ffmpeg -i %s/%s -vf pp=al\|f -f image2 -y processed/%s 2>&1" % (OUTPUT_DIR, f, f)
        #cmd = "ffmpeg -i %s/%s -vf curves=preset=darker -f image2 -y processed/%s 2>&1" % (OUTPUT_DIR, f, f)
        #cmd = "ffmpeg -i %s/%s -vf curves=preset=increase_contrast -f image2 -y processed/%s 2>&1" % (OUTPUT_DIR, f, f)
        cmd = "time ffmpeg -i %s/%s -vf \"hqdn3d=luma_spatial=7,pp=al|f,unsharp=luma_msize_x=7:luma_msize_y=7:luma_amount=1,pp=ac\" -f image2 -y processed/%s 2>&1" % (OUTPUT_DIR, f, f)

        #cmd = "time ffmpeg -i %s/%s -vf \"ocv=filter_name=smooth:filter_params=bilateral,pp=al|f,unsharp=luma_msize_x=5:luma_msize_y=5:luma_amount=1,pp=ac\" -f image2 -y processed.opencv/%s 2>&1" % (OUTPUT_DIR, f, f)
        print cmd
        os.system(cmd)
        
        #cmd = "time ffmpeg -i %s/%s -vf \"hqdn3d,pp=al|f,unsharp=luma_msize_x=5:luma_msize_y=5:luma_amount=1,pp=ac\" -f image2 -y processed.opencv/%s 2>&1" % (OUTPUT_DIR, f, f)
        #cmd = "time ffmpeg -i %s/%s -vf \"nlmeans,pp=al|f,unsharp=luma_msize_x=5:luma_msize_y=5:luma_amount=1,pp=ac\" -f image2 -y processed.opencv/%s 2>&1" % (OUTPUT_DIR, f, f)

        #cmd = "time ffmpeg -i %s/%s -vf \"pp=al|f/ac,curves=red='0/0 0.5/0.58 1/1':blue='0/0 0.5/0.58 1/1',nlmeans,unsharp=luma_msize_x=7:luma_msize_y=7:luma_amount=1\" -f image2 -y processed.opencv/%s 2>&1" % (OUTPUT_DIR, f, f)

        cmd = "time ffmpeg -i %s/%s -vf \"pp=al|f/ac,nlmeans=p=5:r=7,unsharp=luma_msize_x=7:luma_msize_y=7:luma_amount=1,pp=ac\" -f image2 -y processed.opencv/%s 2>&1" % (OUTPUT_DIR, f, f)
        #cmd = "time ffmpeg -i %s/%s -vf \"pp=al|f/ac,hqdn3d=luma_spatial=7,unsharp=luma_msize_x=7:luma_msize_y=7:luma_amount=1\" -f image2 -y processed.opencv/%s 2>&1" % (OUTPUT_DIR, f, f)
        os.system(cmd)
        #cmd = "./unsharp -i %s/%s -o processed.opencv/%s 2>&1" % (OUTPUT_DIR, f, f)
        #os.system(cmd)

        #cmd = "cwebp -q 75 -m 6 %s/%s -o ./%s.webp/%s" % (OUTPUT_DIR, f, f) 
        #os.system(cmd)
        #cmd = "cwebp -q 75 -m 6 processed/%s -o ./processed.webp/%s" % (f, f) 
        #os.system(cmd)
        #cmd = "cwebp -q 55 -m 6 processed/%s -o ./processed.webp55/%s" % (f, f) 
        #os.system(cmd)
        #cmd = "cwebp -q 55 -m 6 -af -f 50 processed/%s -o ./processed.webp55.filter/%s" % (f, f) 
        #os.system(cmd)

        #print >>html,  "<img src=\"%s/%s\" /><img src=\"processed/%s\"><img src=\"processed.opencv/%s\"> <br />" % (OUTPUT_DIR, f, f, f)
        print >>html,  "<img src=\"%s/%s\" /><img src=\"processed/%s\"><img src=\"processed.opencv/%s\"> <br />" % (OUTPUT_DIR, f, f, f)
        html.flush()
        i += 1
        if (i > 1000): break
        #print >>html,  "<img src=\"%s/%s\" /><img src=\"%s.webp/%s\" /><img src=\"processed.webp/%s\"> <img src=\"processed.webp55/%s\"> <br />" % (OUTPUT_DIR, f, OUTPUT_DIR, f, f, f)
        #print >>html,  "<img src=\"%s.webp/%s\" /><img src=\"processed.webp55/%s\"><img src=\"processed.webp55.filter/%s\"> <br />" % (OUTPUT_DIR, f, f, f)

if __name__ == "__main__":
    #fetch_thumbnails()
    preprocess_thumbnails()
