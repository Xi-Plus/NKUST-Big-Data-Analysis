import argparse
import os
import random

import cv2
import numpy as np


class KMeans():
    def __init__(self, k):
        self.set_k(k)

    def set_k(self, k):
        self.k = k

    def calc_dis(self, v1, v2):
        # np.sqrt(np.sum((v1 - v2)**2))
        return np.linalg.norm(v1 - v2)

    def cluster(self, centers):
        img = self.img
        height, width, _ = img.shape
        newgroup = [[] for i in range(self.k)]
        for h in range(height):
            # print(h)
            for w in range(width):
                mindis = self.calc_dis(img[h, w], centers[0])
                mingroup = 0
                for k in range(1, self.k):
                    newdis = self.calc_dis(img[h, w], centers[k])
                    if newdis < mindis:
                        mindis = newdis
                        mingroup = k
                # print(h, w, mindis, mingroup)
                newgroup[mingroup].append((h, w))
        # for i in range(self.k):
        #     print(len(newgroup[i]))

        return newgroup

    def index_to_color(self, idx):
        return self.img[idx]

    def calc_group_center(self, group):
        return [np.mean(np.array(list(map(self.index_to_color, group[i]))), axis=0) for i in range(self.k)]

    def run(self, inputpath):
        img = cv2.imread(inputpath, cv2.IMREAD_UNCHANGED)
        self.img = img
        height, width, _ = img.shape
        print(height, width, height * width)
        centers = []
        while len(centers) < self.k:
            h = random.randint(0, height - 1)
            w = random.randint(0, width - 1)
            if tuple(img[h, w]) not in centers:
                centers.append(tuple(img[h, w]))
        oldgroup = self.cluster(centers)
        step = 1
        while True:
            print('step', step)
            centers = self.calc_group_center(oldgroup)
            print('centers', centers)
            newimg = np.empty((height, width, 3), dtype=np.uint8)
            for i in range(self.k):
                for item in oldgroup[i]:
                    # print(i, item)
                    newimg[item] = centers[i]
                    pass
            cv2.imwrite('{0}-{1}.jpg'.format(os.path.splitext(inputpath)[0], step), newimg)

            newgroup = self.cluster(centers)
            if oldgroup == newgroup:
                break

            oldgroup = newgroup
            step += 1


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', nargs='?', default='img.jpg')
    parser.add_argument('k', nargs='?', type=int, default=5)
    args = parser.parse_args()
    print(args)

    kmeans = KMeans(args.k)
    kmeans.run(args.input)
