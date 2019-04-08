import argparse
import logging
import math
import os
import random

import cv2
import numpy as np


class DBSCAN:
    NOISE = -1
    img = None
    height = None
    width = None
    buckets = None
    bucket_h = None
    bucket_w = None

    def __init__(self, eps, minPts):
        self.eps = eps
        self.minPts = minPts

    def _randomcolor(self):
        r = g = b = 0
        while r + g + b < 255:
            r = random.randint(0, 255)
            g = random.randint(0, 255)
            b = random.randint(0, 255)
        return (r, g, b)

    def _get_neighbors(self, h, w):
        res = ([], [])
        idxh, idxw = self._point_to_bucket_idx(h, w)
        for didxh in range(-1, 2, 1):
            for didxw in range(-1, 2, 1):
                nidxh = idxh + didxh
                nidxw = idxw + didxw
                for nh, nw in self.buckets[nidxh, nidxw]:
                    if self.img[nh, nw] < 128 and (nh - h)**2 + (nw - w)**2 <= self.eps**2:
                        res[0].append(nh)
                        res[1].append(nw)
        return res

    def _point_to_bucket_idx(self, h, w):
        return math.ceil(h / self.eps), math.ceil(w / self.eps)

    def _generate_buckets(self):
        self.bucket_h, self.bucket_w = self._point_to_bucket_idx(
            self.height, self.width)
        self.buckets = np.empty(
            (self.bucket_h + 2, self.bucket_w + 2), dtype=np.object)
        self.buckets[...] = [[[] for i in range(self.bucket_w + 2)]
                             for j in range(self.bucket_h + 2)]

        idx = np.where(self.img < 128)
        for h, w in list(zip(idx[0], idx[1])):
            self.buckets[self._point_to_bucket_idx(h, w)].append((h, w))

    def run(self, inputpath):
        logging.info('Runing on %s', inputpath)
        filename = os.path.splitext(inputpath)[0]
        img = cv2.imread(inputpath, cv2.IMREAD_GRAYSCALE)
        self.img = img
        logging.info('Size %s', img.shape)
        self.height = img.shape[0]
        self.width = img.shape[1]
        logging.info('Black rate %s', len(
            np.where(img < 128)[0]) / self.height / self.width)

        self._generate_buckets()
        # for bh in range(self.bucket_h):
        #     for bw in range(self.bucket_w):
        #         print(len(self.buckets[bh, bw]), end=' ')
        #     print()

        label = np.zeros(img.shape, dtype=np.int32)
        used = np.zeros(img.shape, dtype=np.uint8)
        group_cnt = 0
        for h in range(self.height):
            for w in range(self.width):
                if label[h, w] != 0:
                    continue
                if img[h, w] > 128:
                    continue
                neighbors = self._get_neighbors(h, w)
                # logging.debug('%s find %s neighbors',
                #               (h, w), len(neighbors[0]))
                if len(neighbors[0]) < self.minPts:
                    label[neighbors] = self.NOISE
                    continue

                group_cnt += 1

                label[h, w] = group_cnt
                used[neighbors] = 1

                queue = list(zip(neighbors[0], neighbors[1]))
                # print(queue)
                queue.remove((h, w))
                while len(queue) > 0:
                    h2, w2 = queue.pop(0)
                    if label[h2, w2] == self.NOISE:
                        label[h2, w2] = group_cnt
                        continue
                    if label[h2, w2] != 0:
                        continue
                    # logging.info('\t%s %s', (h2, w2), label[h2, w2])
                    label[h2, w2] = group_cnt
                    neighbors = self._get_neighbors(h2, w2)
                    # logging.info('\t%s find %s neighbors', (h2, w2), len(neighbors[0]))
                    if len(neighbors[0]) >= self.minPts:
                        for h3, w3 in list(zip(neighbors[0], neighbors[1])):
                            if not used[h3, w3]:
                                queue.append((h3, w3))
                                used[h3, w3] = 1
                        # logging.info('====== %s find %s neighbors: %s',
                        #              (h, w), len(neighbors[0]), neighbors)
        logging.info('Find %s groups', group_cnt)
        noices = np.where(label == self.NOISE)
        logging.info('%s noises', len(noices[0]))

        newimg = np.full((self.height, self.width, 3),
                         (255, 255, 255), dtype=np.uint8)
        for i in range(1, group_cnt + 1):
            color = self._randomcolor()
            newimg[np.where(label == i)] = color
        newimg[noices] = (0, 0, 0)
        cv2.imwrite('{}-out.jpg'.format(filename), newimg)

        logging.info('End')


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(levelname)s %(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('input', nargs='?', default='img.jpg')
    parser.add_argument('--eps', type=int, default=5)
    parser.add_argument('--minPts', type=int, default=3)
    args = parser.parse_args()
    print(args)

    dbscan = DBSCAN(args.eps, args.minPts)
    dbscan.run(args.input)
