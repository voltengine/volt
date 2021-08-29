#!/bin/python

import sys, os

for i in range (2, len(sys.argv)):
	assets_path = os.path.join(sys.argv[i], 'assets')
