#!/usr/bin/env python

import os, sys, time, re

print time.localtime()[:6]

PS = []

RCNAME = sys.argv[0] + '.rc'
RC = open(RCNAME, 'w')
print >> RC, '''
ghostscript \\
-o psplot.pdf \\
-sDEVICE=pdfwrite \\
-dPDFSettings=/Screen''',

for i in os.listdir('.'):
    if i.split('.')[-1] == 'ps':
        T = i[:-3]
        PS.append(T)
        print >>RC,'\\\n"%s.ps"'%T,

print >>RC
print >>RC
for i in PS:
     print >>RC,'rm "%s.ps"'%i
print >>RC
print >>RC
RC.close()

os.system('sh %s && "rm %s"'%(RCNAME,RCNAME))
