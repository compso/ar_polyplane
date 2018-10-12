
def printVidx(divs=2):
    vidx=[]
    a=0
    v=0
    for i in range(divs):
        print ""
        for c in range(divs):
            r = []
            v=a+c
            r.append(v)
            v+=1
            r.append(v)
            r += [ v+divs+1, v+divs]
            vidx += r
            print ','.join(str(f) for f in r)
        a=v+1


def printVlist(divs=3, width=10):
    for v in range(divs+1):
        posz = (float(width)-(v*(float(width)/divs)))-(width/2)
        for n in range(divs+1):
            posx = (float(width)/float(divs))*n-(float(width)/2.0)
            posy = 0.0
            print ','.join(str(f) for f in [posx, posy, posz])
