import argparse
import logging
import os
import random

import cv2
import numpy as np


class DBSCAN:
    NOISE = -1
    img = None
    height = None
    width = None

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
        y, x = np.ogrid[-h:self.height - h, -w:self.width - w]
        mask = x * x + y * y <= self.eps * self.eps
        return np.where(mask & (self.img < 128))

    def run(self, inputpath):
        logging.info('Runing on %s', inputpath)
        filename = os.path.splitext(inputpath)[0]
        img = cv2.imread(inputpath, cv2.IMREAD_GRAYSCALE)
        self.img = img
        logging.info('Size %s', img.shape)
        self.height = img.shape[0]
        self.width = img.shape[1]

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
                logging.info('%s find %s neighbors',
                             (h, w), len(neighbors[0]))
                if len(neighbors[0]) < self.minPts:
                    label[neighbors] = self.NOISE
                    continue

                group_cnt += 1

                label[neighbors] = group_cnt
                used[neighbors] = 1

                queue = list(zip(neighbors[0], neighbors[1]))
                # print(queue)
                queue.remove((h, w))
                while len(queue) > 0:
                    h2, w2 = queue.pop(0)
                    if label[h2, w2] == self.NOISE:
                        label[h2, w2] = group_cnt
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

        newimg = np.zeros((self.height, self.width, 3), dtype=np.uint8)
        for i in range(1, group_cnt + 1):
            color = self._randomcolor()
            newimg[np.where(label == i)] = color
        cv2.imwrite('{}-out.jpg'.format(filename), newimg)

        logging.info('End')


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(levelname)s %(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('input', nargs='?', default='img.jpg')
    parser.add_argument('--eps', type=int, default=5)
    parser.add_argument('--minPts', type=int, default=3)
    args = parser.parse_args()
    print(args)

    dbscan = DBSCAN(args.eps, args.minPts)
    dbscan.run(args.input)
