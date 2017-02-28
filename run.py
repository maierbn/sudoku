#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import numpy as np
import matplotlib.pyplot as plt
import subprocess

if len(sys.argv)<2:
  sys.exit(0)

nruns = 200
print "number of runs: ",nruns

average = 0
n_summands = 0
avg_fixed = 0
n_summands_fixed = 0
for i in range(nruns):

  output = subprocess.check_output(sys.argv[1], shell=True) 
  
  #print "output: ",output
  
  teststring = "total average: "
  if teststring in output:
    pos = output.index(teststring)
    posend = output.index("\n", pos)
    value = float(output[pos+len(teststring):posend])
    
    average += value
    n_summands += 1
    
  teststring = "nfixed: "
  if teststring in output:
    pos = output.index(teststring)
    posend = output.index("\n", pos)
    nFixed = int(output[pos+len(teststring):posend])
    
    avg_fixed += nFixed
    n_summands_fixed += 1
  
  if i%4 == 0:
    print "value:",value, ", nFixed:",nFixed
    

average /= n_summands
avg_fixed /= n_summands_fixed
print "average:",average
print "fixed:",avg_fixed
