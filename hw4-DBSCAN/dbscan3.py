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
    showNoise = False
    mask = None
    maskadd = None
    maskremove = None
    label = None
    core_point = None
    disjoint_sets = None
    new_label = None
    prevw = None
    prevneighbors = None

    def __init__(self, eps, minPts):
        self.eps = eps
        self.useprevlimit = max(int(eps / 2), 1)
        self.minPts = minPts
        self._get_mask(eps)

    def _randomcolor(self):
        r = g = b = 0
        while r + g + b < 255:
            r = random.randint(0, 255)
            g = random.randint(0, 255)
            b = random.randint(0, 255)
        return (r, g, b)

    def _get_neighbors(self, h, w):
        res = ([], [])
        for dh, dw in self.mask:
            nh = h + dh
            nw = w + dw
            if nh < 0 or nh >= self.height or nw < 0 or nw >= self.width:
                continue
            if self.img[nh, nw] < 128:
                res[0].append(nh)
                res[1].append(nw)
        return res

    def _get_mask(self, eps):
        y, x = np.ogrid[-eps:eps + 1, -eps:eps + 1]
        mask = x * x + y * y <= eps * eps
        mask = mask.astype(np.int8)
        oldmask = np.zeros((eps * 2 + 1, eps * 2 + 2), dtype=np.int8)
        newmask = np.zeros((eps * 2 + 1, eps * 2 + 2), dtype=np.int8)
        oldmask[:, :-1] = mask
        newmask[:, 1:] = mask
        # print(oldmask)
        # print(newmask)
        maskdiff = newmask - oldmask
        # print(maskdiff)

        idx = np.where(mask)
        self.mask = np.array(list(zip(idx[0], idx[1]))) - (eps, eps)
        idx = np.where(maskdiff == 1)
        self.maskadd = np.array(list(zip(idx[0], idx[1]))) - (eps, eps + 1)
        # print(self.maskadd)
        idx = np.where(maskdiff == -1)
        self.maskremove = np.array(list(zip(idx[0], idx[1]))) - (eps, eps + 1)
        # print(self.maskremove)

    def _method1(self, h, w):
        if self.img[h, w] > 128:
            return

        neighbors = self._get_neighbors(h, w)
        if len(neighbors[0]) < self.minPts:
            if self.label[h, w] == 0:
                self.label[h, w] = self.NOISE
            return

        self.core_point[h, w] = True

        if self.label[h, w] > 0:
            this_label = self.label[h, w]
        else:
            # logging.info('Find new gorup at %s (A)', (h, w))
            self.new_label += 1
            this_label = self.new_label
            self.disjoint_sets.append(self.new_label)

        for nh, nw in list(zip(neighbors[0], neighbors[1])):
            if self.label[nh, nw] != this_label and self.label[nh, nw] > 0 and self.core_point[nh, nw]:
                # logging.info('Merge %s and %s',
                #              self.label[nh, nw], this_label)
                self._merge(self.label[nh, nw], this_label)
                continue
            self.label[nh, nw] = this_label

    def _method2(self, h, w):
        if self.img[h, w] > 128:
            return

        if w - self.prevw > self.useprevlimit:
            neighbors = self._get_neighbors(h, w)
            self.prevneighbors = list(zip(neighbors[0], neighbors[1]))
        else:
            # logging.info('Now at %s %s', (h, w), self.prevw)
            # logging.info('Old nei %s', self.prevneighbors)
            for dw in range(self.prevw - w + 1, 1):
                # logging.info('dw %s', dw)
                for mh, mw in self.maskremove:
                    runh = h + mh
                    runw = w + mw + dw
                    # logging.info('Try remove %s', (runh, runw))
                    if self.img[runh, runw] <= 128:
                        try:
                            self.prevneighbors.remove((runh, runw))
                        except Exception as e:
                            print(h, w)
                            print(runh, runw)
                            print(self.prevneighbors)
                            raise e
                        # logging.info('Remove %s', (runh, runw))
                for mh, mw in self.maskadd:
                    runh = h + mh
                    runw = w + mw + dw
                    # logging.info('Try add %s', (runh, runw))
                    if self.img[runh, runw] <= 128:
                        self.prevneighbors.append((runh, runw))
                        # logging.info('Add %s', (runh, runw))

        if len(self.prevneighbors) < self.minPts:
            if self.label[h, w] == 0:
                self.label[h, w] = self.NOISE
            return

        self.core_point[h, w] = True

        if self.label[h, w] > 0:
            this_label = self.label[h, w]
        else:
            # logging.info('Find new gorup at %s (B)', (h, w))
            self.new_label += 1
            self.disjoint_sets.append(self.new_label)
            this_label = self.new_label

        for nh, nw in self.prevneighbors:
            if self.label[nh, nw] != this_label and self.label[nh, nw] > 0 and self.core_point[nh, nw]:
                # logging.info('Merge %s and %s',
                #              self.label[nh, nw], this_label)
                self._merge(self.label[nh, nw], this_label)
                continue
            self.label[nh, nw] = this_label

        self.prevw = w

    def _merge(self, x, y):
        x_parent = self._get_parent(x)
        y_parent = self._get_parent(y)
        min_label = min(x_parent, y_parent)
        max_label = max(x_parent, y_parent)
        self.disjoint_sets[max_label] = min_label

    def _get_parent(self, x):
        if self.disjoint_sets[x] == x:
            return x
        self.disjoint_sets[x] = self._get_parent(self.disjoint_sets[x])
        return self.disjoint_sets[x]

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

        self.label = np.zeros(img.shape, dtype=np.int32)
        self.core_point = np.zeros(img.shape, dtype=bool)
        self.disjoint_sets = [0]
        self.new_label = 0

        for h in range(0, self.eps):
            for w in range(self.width):
                self._method1(h, w)

        for h in range(self.eps, self.height - self.eps):
            # print(h)
            for w in range(0, self.eps):
                # print(h, w)
                self._method1(h, w)

            self.prevw = -100
            self.prevneighbors = None
            for w in range(self.eps, self.width - self.eps):
                # print(h, w)
                # self._method1(h, w)
                self._method2(h, w)

            for w in range(self.width - self.eps, self.width):
                # print(h, w)
                self._method1(h, w)

        for h in range(self.height - self.eps, self.height):
            for w in range(self.width):
                self._method1(h, w)

        logging.info('Calc end. Drawing')
        # logging.info('disjoint set %s', self.disjoint_sets)
        logging.info('new_label %s', self.new_label)

        newimg = np.full((self.height, self.width, 3),
                         (255, 255, 255), dtype=np.uint8)

        colors = [self._randomcolor() for i in range(self.new_label + 1)]
        # logging.info('colors %s', colors)
        for h in range(self.height):
            for w in range(self.width):
                if img[h, w] <= 128:
                    if self.label[h, w] == self.NOISE:
                        if self.showNoise:
                            newimg[h, w] = (0, 0, 0)
                        pass
                    else:
                        newimg[h, w] = colors[self._get_parent(
                            self.label[h, w])]
        # for i in range(1, self.new_label + 1):
        #     where = np.where(self.label == i)
        #     if len(where[0]) == 0:
        #         continue
        #     # print(where)
        #     # print(i, self.disjoint_sets[i])
        #     newimg[where] = colors[self._get_parent(i)]
        group_cnt = sum(
            [self.disjoint_sets[i] == i for i in range(1, self.new_label + 1)])

        logging.info('Find %s groups', group_cnt)

        noices = np.where(self.label == self.NOISE)
        logging.info('%s noises', len(noices[0]))
        cv2.imwrite('{}-out-eps{}-min{}-group{}.bmp'.format(
            filename, self.eps, self.minPts, group_cnt
        ), newimg)

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
