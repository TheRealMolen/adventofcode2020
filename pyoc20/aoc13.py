

# 17,x,13,19  => 3417

#     17x = N
# 13y - 2 = N
# 19z - 3 = N


# 3,5  => 9 .. 24 .. 
# 5,3  => 10

# 3,5,7  => 


# 100000000000000
# 623293961619332
# 623,293,961,619,332

def readbuses(input):
    buses = []
    for i,v in enumerate(input.split(',')):
        if v != 'x':
            buses.append([int(v),i])
    return buses


def printbuses(buses):
    offs = dict([(b,a) for a,b in buses])
    els = max([b[1] for b in buses])
    s = ''
    first = True
    for i in range(els+1):
        if not first:
            s += ','
        else:
            first = False

        if i in offs:
            s += str(offs[i])
        else:
            s += 'x'
    return s



input = '3,x,x,5,x,7'

buses = readbuses(input)
#buses = [[3,0],[5,1],[7,3],[17,4]]
#buses = [[17,0],[13,1],[19,10]]
prod = 1
for bus in buses:
    prod *= bus[0]

maxbus = -1
maxoff = 100000
for b,o in buses:
    if b > maxbus:
        maxbus = b
        maxoff = o

mostbuses = [b for b in buses if b[0] != maxbus]

step = int(prod / maxbus)


def is_valid_time(t, buses):
    for busnum,offs in buses:
        if (t + offs) % busnum != 0:
            return False
    return True

def find_one_valid_time(buses):
    attempt = 0
    checks = 0
    for attempt in range(1000):
        for i in range(maxbus):
            checks += 1
            t = attempt + step * i

            if is_valid_time(t, buses):
                print(t,'is valid time; found after',checks,'checks')
                return t

t = find_one_valid_time(mostbuses)





# el = len(buses) - 1
# scatter = ''
# for toffs in range(1,prod):
#     if toffs in [b[1] for b in buses]:
#         continue
#     #print('\n----- trying offset',toffs,'for element',el,'-----')
#     buses[el][1] = toffs

#     i = 0
#     found = False
#     while not found:
#         found = True
#         for busnum,offs in buses:
#             if (i + offs) % busnum != 0:
#                 found = False
#                 break
#         if found:
#             scatter += '%d %d ' % (toffs, i)
#             break

#         i += buses[0][0]


def find_line(buses):
    print('for', printbuses(buses), '...')
    solutions = []
    i = 0
    while len(solutions) < 5:
        found = True
        for busnum,offs in buses:
            if (i + offs) % busnum != 0:
                found = False
                break
        if found:
            solutions.append(i)

        i += buses[0][0]

    # y = mx + c
    m = solutions[1] - solutions[0]
    c = solutions[0] - m
    x_cross = -c / m
    print('line is: y =', str(m) + 'x', '+', c)
    print('crosses x axis at', x_cross, ' = ', -c, '/', m)

    return (m,c)





# scatter = ''.join(['%d %d ' % (i+1,s) for i,s in enumerate(solutions)])
# print(scatter)

# import clipboard
# clipboard.copy(scatter)
#print(prod)