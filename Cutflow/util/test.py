import string

replacements = {'(':'', ')':'', '_v1':'', '_v2':'', '_p1':'', '_p2':'', '_p3':'', '_tlp2':'', '_p4':'', '_tlp1':'', 'DataVector<':'', '>':''}
outfile = open('/home/thoresen/Desktop/output.txt', 'w')
infile = open('/home/thoresen/Desktop/input.txt')

for line in infile:
    if not line[72:].startswith('HLT'):
        for src, target in replacements.iteritems():            
            line = line.replace(src, target)
        #if len(line) > 0:
            #outfile.write(line[72:])
        line2 = line[72:].split(' ',2)
        print line2[0]
        
        outfile.write(line2[0])
        outfile.write('\n')
        #if len(line2) > 0:
            #line2[1] = line2[1].replace('\n','')
            #outfile.write(line2[0])
            #outfile.write('\n')
            #outfile.write(line2[1])


