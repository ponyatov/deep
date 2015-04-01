#!/usr/bin/env python
import os,sys,time,re

RC=open('rc.rc','w')

for i in os.listdir('.'):
	if i.split('.')[-1]=='ps':
		T=i[:-3]
		C='ps2pdf "%s.ps" "%s.pdf" && rm "%s.ps"'%(T,T,T)
		print C ; print >>RC,C

RC.close()
os.system('sh rc.rc && rm rc.rc')

