import browser_cookie3
import requests


# todo: https://stackoverflow.com/questions/11523918/python-start-a-function-at-given-time
def download_input(day):
    cookies = browser_cookie3.chrome(domain_name='.adventofcode.com')
    url = 'https://adventofcode.com/2020/day/%d/input' % day
    print(url)
    response = requests.get(url, verify=False, cookies=cookies, timeout=3)
    with open('../aoc20/data/day%d.txt'%day, 'wt') as outf:
        outf.write(response.text)

for day in range(24,26):
    download_input(day)


def run_day12():
    dirs = ['E','S','W','N']
    moves = [[1,0],[0,-1],[-1,0],[0,1]]
    def day12(input):
        dir = 'E'
        pos = [0,0]
        for line in input:
            line = line.strip()
            move = line[0]
            dist = int(line[1:])
            opos = [x for x in pos]
            if move in dirs:
                m = moves[dirs.index(move)]
                pos[0] += dist * m[0]
                pos[1] += dist * m[1]
            elif move == 'F':
                m = moves[dirs.index(dir)]
                pos[0] += dist * m[0]
                pos[1] += dist * m[1]
            elif move == 'R':
                dir = dirs[int((dirs.index(dir) + (dist/90)) % 4)]
            elif move == 'L':
                dir = dirs[int((dirs.index(dir) - (dist/90) + 4) % 4)]
            #print(opos, dir, ':', line.strip(), '->', pos, dir)

        return abs(pos[0]) + abs(pos[1])

    def day12_2(input):
        dir = 'E'
        pos = [0,0]
        wpos = [10,1]
        for line in input:
            line = line.strip()
            move = line[0]
            dist = int(line[1:])
            opos = [x for x in pos]
            if move in dirs:
                m = moves[dirs.index(move)]
                wpos[0] += dist * m[0]
                wpos[1] += dist * m[1]
            elif move == 'F':
                pos[0] += dist * wpos[0]
                pos[1] += dist * wpos[1]
            elif move == 'R':
                for i in range(0,int(dist/90)):
                    wpos = [wpos[1], -wpos[0]]
            elif move == 'L':
                for i in range(0,int(dist/90)):
                    wpos = [-wpos[1], wpos[0]]
            #print(opos, dir, ':', line.strip(), '->', pos, dir)

        return abs(pos[0]) + abs(pos[1])


    sample = '''F10
                N3
                F7
                R90
                F11'''
    input = []
    with open('../aoc20/data/day12.txt') as f:
        input = f.read()

    print(day12(sample.split('\n')))
    print(day12(input.split('\n')))
    print(day12_2(sample.split('\n')))
    print(day12_2(input.split('\n')))


#print('-------- day 12 ---------')
#run_day12()