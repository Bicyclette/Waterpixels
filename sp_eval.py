#! /usr/bin/python3
# -*- coding: utf-8 -*-

import os
from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

folders = np.array(["landscape", "tiger", "eskimo", "fish", "elephant"])
BR_imgs = dict()
avg_dist_imgs = dict()
med_dist_imgs = dict()

def valid_index(c, l, img):
    x,y,z = img.shape
    if c >= 0 and c < x:
        if l >= 0 and l < y:
            return True
    return False

def search_closest(col, line, img):
    radius = 1
    found = False
    x = -1
    y = -1
    dist = float('inf')
    while not found:
        # top
        l = line - radius
        for i in range(col - radius, col + radius + 1, 1):
            c = i
            if valid_index(c, l, img) and img[c][l][0] == 255:
                local_dist = np.sqrt(np.power(c - col, 2) + np.power(l - line, 2))
                if local_dist < dist:
                    dist = local_dist
                    x = c
                    y = l

        # bottom
        l = line + 1
        for i in range(col - radius, col + radius + 1, 1):
            c = i
            if valid_index(c, l, img) and img[c][l][0] == 255:
                local_dist = np.sqrt(np.power(c - col, 2) + np.power(l - line, 2))
                if local_dist < dist:
                    dist = local_dist
                    x = c
                    y = l
        
        # left
        c = c - radius
        for i in range(line - radius, line + radius + 1, 1):
            l = i
            if valid_index(c, l, img) and img[c][l][0] == 255:
                local_dist = np.sqrt(np.power(c - col, 2) + np.power(l - line, 2))
                if local_dist < dist:
                    dist = local_dist
                    x = c
                    y = l
        
        # right
        c = c + radius
        for i in range(line - radius, line + radius + 1, 1):
            l = i
            if valid_index(c, l, img) and img[c][l][0] == 255:
                local_dist = np.sqrt(np.power(c - col, 2) + np.power(l - line, 2))
                if local_dist < dist:
                    dist = local_dist
                    x = c
                    y = l

        radius += 1
        if x != -1 and y != -1:
            found = True
    
    return (x,y,int(dist))

def boundary_recall(sp, gt, name):
    sp_data = np.asarray(sp)
    gt_data = np.asarray(gt)
    x, y = gt_data.shape
    total = 0.0
    BR = 0.0

    closest = []
    
    for c in range(x):
        for l in range(y):
            if gt_data[c][l]:
                total += 1.0
                index = search_closest(c, l, sp_data)
                closest.append(index[2])
                if index[2] < 3:
                    BR += 1.0

    avg_dist_imgs[name] = np.mean(closest)
    med_dist_imgs[name] = np.median(closest)
    
    BR /= total
    BR_imgs[name] = BR
    BR = 0.0
    total = 0.0

def graph(f):
    x = [5, 10, 15, 20, 25, 30]
    plt.xlabel("grid step")
    plt.grid(True, "both")
    
    # BOUNDARY RECALL
    title = f + " image\nboundary-recall"
    plt.title(title, fontweight = "bold")
    plt.ylabel("boundary-recall")

    y = []
    for i in range(5, 31, 5):
        y.append(BR_imgs[f + "_" + str(i)])
    plt.scatter(x, y, c="blue")
    for cx, cy in zip(x, y):
        plt.text(cx, cy, "({:d}, {:.3f})".format(cx, cy))
    plt.plot(x, y)
    plt.savefig("graphs/" + f + "/" + f + "_boundary_recall.png")
    plt.clf()
    
    # AVG CLOSEST DISTANCE TO GT BORDER PIXEL
    plt.xlabel("grid step")
    plt.grid(True, "both")
    title = f + " image\nsuperpixel border average distance to ground truth border"
    plt.title(title, fontweight = "bold")
    plt.ylabel("average closest distance")

    y = []
    for i in range(5, 31, 5):
        y.append(avg_dist_imgs[f + "_" + str(i)])
    plt.scatter(x, y, c="blue")
    for cx, cy in zip(x, y):
        plt.text(cx, cy, "({:d}, {:.3f})".format(cx, cy))
    plt.plot(x, y)
    plt.savefig("graphs/" + f + "/" + f + "_avg_closest_dist_SP_GT.png")
    plt.clf()
    
    # MEDIAN CLOSEST DISTANCE TO GT BORDER PIXEL
    plt.xlabel("grid step")
    plt.grid(True, "both")
    title = f + " image\nsuperpixel border median distance to ground truth border"
    plt.title(title, fontweight = "bold")
    plt.ylabel("median closest distance")
    y = []
    for i in range(5, 31, 5):
        y.append(med_dist_imgs[f + "_" + str(i)])
    plt.scatter(x, y, c="blue")
    for cx, cy in zip(x, y):
        plt.text(cx, cy, "({:d}, {:.3f})".format(cx, cy))
    plt.plot(x, y)
    plt.savefig("graphs/" + f + "/" + f + "_med_closest_dist_SP_GT.png")
    plt.clf()

def compute_dist_to_closest_sp_pixel():
    for f in folders:
        print("computing image : " + f)
        gt_img = Image.open("imgs/" + f + "_groundTruth.png")
        for i in range(5, 31, 5):
            sp_img = Image.open("build/" + f + "/" + f + "_" + str(i) + "/" + "contours.jpg")
            boundary_recall(sp_img, gt_img, f + "_" + str(i))
        os.system("mkdir graphs/" + f)
        graph(f)

compute_dist_to_closest_sp_pixel()
