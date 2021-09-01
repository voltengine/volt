#!/bin/python

# Merges ./assets/ and ./config.json
# from over all packages

import sys, os, distutils, json

def merge_dict(dst, src):
	for key in src:
		if isinstance(src[key], dict):
			dst[key] = {}
			merge_dict(dst[key], src[key])
		else:
			dst[key] = src[key]

bundle_assets_path = os.path.join(sys.argv[1], 'assets')
bundle_config_path = os.path.join(sys.argv[1], 'config.json')

bundle_config = {}

for i in range (2, len(sys.argv)):
	assets_path = os.path.join(sys.argv[i], 'assets')
	config_path = os.path.join(sys.argv[i], 'config.json')
	
	if os.path.isdir(assets_path):
		distutils.dir_util.copy_tree(assets_path, bundle_assets_path)

	if os.path.isfile(config_path):
		config = json.load(open(config_path))
		merge_dict(bundle_config, config)

json.dump(bundle_config, open(bundle_config_path, 'w'), indent='\t', sort_keys=True)
