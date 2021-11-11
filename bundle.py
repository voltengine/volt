#!/bin/python

# Merges ./assets/ and ./config.json
# from over all packages

import sys, os, json, shutil
from pathlib import Path

def merge_dict(dst, src):
	for key in src:
		if key == 'development':
			continue

		if isinstance(src[key], dict):
			if not key is dst:
				dst[key] = {}
			merge_dict(dst[key], src[key])
		elif not key in dst:
			dst[key] = src[key]

bundle_assets_path = os.path.normpath(os.path.join(sys.argv[1], 'assets'))
bundle_config_path = os.path.normpath(os.path.join(sys.argv[1], 'config.json'))

bundle_config = {}

for i in range(len(sys.argv) - 1, 1, -1):
	assets_path = os.path.normpath(os.path.join(sys.argv[i], 'assets'))
	config_path = os.path.normpath(os.path.join(sys.argv[i], 'config.json'))
	
	if os.path.isdir(assets_path):
		print('Copying assets from "' + assets_path + '"...')
		for root, dirs, files in os.walk(assets_path):
			rel_root = os.path.relpath(root, assets_path)
			if '.development' in Path(rel_root).parents:
				continue

			for file in files:
				src = os.path.join(root, file)
				rel_src = os.path.relpath(src, assets_path)
				dst = os.path.join(bundle_assets_path, rel_src)

				if not os.path.exists(dst):
					os.makedirs(os.path.dirname(dst), exist_ok=True)
					shutil.copyfile(src, dst)

	if os.path.isfile(config_path):
		print('Applying configuration from "' + config_path + '"...')
		config = json.load(open(config_path))
		merge_dict(bundle_config, config)

print('Writing merged configuration...') 
json.dump(bundle_config, open(bundle_config_path, 'w'), indent='\t', sort_keys=True)
