import sys
import re
import gflags
import logging
import matplotlib.pyplot as plt
import numpy as np

FLAGS = gflags.FLAGS
gflags.DEFINE_string('nowlog', 'now.log', "The log now")
gflags.DEFINE_string('orilog', 'ori.log', "The log origin")
gflags.DEFINE_string('scene', 'gpsnr', 'The plot scene')

def extract_qp(file):
    arr_qp = {}
    with open(file, 'r') as fp:
        for line in fp:
            result = re.search(r'\s+([0-9]+)\s+[IPBb]\s+([0-9|\.]+)', line)
            if result:
                arr_qp[int(result.group(1))] = result.group(2)
    return arr_qp

def extract_gpsnr(file):
    arr_gpsnr = {}
    with open(file, 'r') as fp:
        for line in fp:
            result = re.search(r'\s+([0-9]+)\s+[IPBb]\s+(.*)$', line)
            if result:
                sarr = result.group(2).split()
                gpsnr = float(sarr[2]) * 0.75 + float(sarr[3]) * 0.125 + float(sarr[4]) * 0.125
                arr_gpsnr[int(result.group(1))] = str(gpsnr)
    return arr_gpsnr

def print_arr(qpori, qpnow):
    listori = [float(v) for (k, v) in qpori.items()]
    listnow = [float(v) for (k, v) in qpnow.items()]
    xori = [int(k) for (k, v) in qpori.items()]
    xnow = [int(k) for (k, v) in qpnow.items()]

    plt.figure(0)
    plt.scatter(xori, listori, marker='*')
    plt.scatter(xnow, listnow, marker='|')
    plt.figure(1)
    plt.scatter(xnow, np.array(listnow) - np.array(listori), marker='*')
    plt.show()

def main(argv):
    FLAGS(argv)

    if FLAGS.scene == 'qp':
        arr_qp_now = extract_qp(FLAGS.nowlog)
        arr_qp_ori = extract_qp(FLAGS.orilog)
        print_arr(arr_qp_ori, arr_qp_now)
    elif FLAGS.scene == 'gpsnr':
        arr_gpsnr_now = extract_gpsnr(FLAGS.nowlog)
        arr_gpsnr_ori = extract_gpsnr(FLAGS.orilog)
        print_arr(arr_gpsnr_ori, arr_gpsnr_now)

if __name__ == '__main__':
    main(sys.argv)