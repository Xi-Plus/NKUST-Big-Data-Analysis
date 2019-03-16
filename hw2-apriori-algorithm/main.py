import argparse
import time


parser = argparse.ArgumentParser()
parser.add_argument('input', default=None)
parser.add_argument('output', default=None)
parser.add_argument('support', default=None, type=int)
args = parser.parse_args()

start_time = time.time()

inputpath = args.input
outputpath = args.output
support = args.support
print('file input: {}'.format(inputpath))
print('file output: {}'.format(outputpath))
print('support: {}'.format(support))

fin = open(inputpath, 'rb')
fout = open(outputpath, 'w')


def dfs(now, item):
    if now['level'] == grouplen:
        if now['support'] >= support:
            Lits.append(item.copy())
            Lsup.append(now['support'])
    else:
        for nextitemid in now['child']:
            nextitem = item.copy()
            nextitem.append(nextitemid)
            dfs(now['child'][nextitemid], nextitem)


def readint():
    buf = fin.read(4)
    if not buf:
        return None
    return int.from_bytes(buf, byteorder='little')


Cits = []
Csup = {}
grouplen = 1
C1 = {}

while True:
    tempn = readint()
    if tempn is None:
        break
    readint()
    cnt = readint()
    while cnt > 0:
        tempn = readint()
        if tempn not in C1:
            C1[tempn] = 0
        C1[tempn] += 1
        cnt -= 1
fin.close()

Lits = []
Lsup = []
for item in C1:
    if C1[item] >= support:
        Lits.append([item])
        Lsup.append(C1[item])

Llensum = 0
while len(Lits) > 0:
    # output file
    for i in range(len(Lits)):
        fout.write('{}'.format(Lits[i][0]))
        for j in range(1, grouplen):
            fout.write(',{}'.format(Lits[i][j]))
        fout.write(':{}\n'.format(Lsup[i]))
    print('L{}: {}'.format(grouplen, len(Lits)))
    Llensum += len(Lits)

    Ctemp = []
    Ctemplen = 0
    # cross join
    for i in range(len(Lits)):
        for j in range(i + 1, len(Lits)):
            k = 0
            while k < grouplen - 1 and Lits[i][k] == Lits[j][k]:
                k += 1
            if k == grouplen - 1:
                Ctemp.append(Lits[i].copy())
                Ctemplen += 1
                tempa = Lits[i][grouplen - 1]
                tempb = Lits[j][grouplen - 1]
                if tempa > tempb:
                    tempa, tempb = tempb, tempa
                Ctemp[Ctemplen - 1][grouplen - 1] = tempa
                Ctemp[Ctemplen - 1].append(tempb)

    print('C{}temp: {}'.format(grouplen + 1, len(Ctemp)))

    grouplen += 1

    # remove not in Lits
    Lset = set()
    for i in range(len(Lits)):
        Lset.add(tuple(Lits[i]))
    Cits.clear()
    for i in range(len(Ctemp)):
        j = 0
        while j < grouplen:
            if tuple(Ctemp[i][:j] + Ctemp[i][j + 1:]) not in Lset:
                break
            j += 1
        if j == grouplen:
            Cits.append(Ctemp[i])

    print('C{}dits: {}'.format(grouplen, len(Cits)))

    Csup.clear()

    root = {'val': -1, 'level': 0, 'child': {}}
    for i in range(len(Cits)):
        now = root
        for j in range(grouplen):
            if Cits[i][j] not in now['child']:
                now['child'][Cits[i][j]] = {
                    'val': Cits[i][j],
                    'level': j + 1,
                    'child': {}
                }
            now = now['child'][Cits[i][j]]
        now['support'] = 0

    # cnt sup
    fin = open(inputpath, 'rb')
    nownode = []
    while True:
        tempn = readint()
        if tempn is None:
            break
        readint()
        cnt = readint()
        nownode.clear()
        nownode.append(root)
        while cnt > 0:
            tempn = readint()
            for i in range(len(nownode) - 1, -1, -1):
                if tempn in nownode[i]['child']:
                    if nownode[i]['child'][tempn]['level'] == grouplen:
                        nownode[i]['child'][tempn]['support'] += 1
                    else:
                        nownode.append(nownode[i]['child'][tempn])
            cnt -= 1
    fin.close()

    Lits.clear()
    Lsup.clear()
    dfs(root, [])

fout.close()

print('Lall: {}'.format(Llensum))
print('It tooks {0:.0f} milliseconds'.format(
    (time.time() - start_time) * 1000))
