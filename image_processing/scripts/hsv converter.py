
def convertfromPinta(H,S,V):
    h = round(H*179/360)
    s = round(S*255/100)
    v = round(V*255/100)
    return h,s,v
