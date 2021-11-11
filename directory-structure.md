## Prerequisities:
```
Microsoft Visual Studio Build Tools / GCC / Clang
CMake 3.20
Python 3.7
Vulkan SDK
Microsoft DirectX SDK
PhysX SDK
```

## Volt Install
```
.volt
	cli
		bin
			volt.exe
		Volt.cmake
	hub
		assets
			volt-hub
		bin
			volt.dll
			volt-hub.dll
			volt-hub.exe
	packages
		voltengine
			volt
				1.2.0
				1.2.1
					app
						main.cpp
					assets
						volt
							editor
					include
						volt
					src
						volt
					CMakeLists.txt
```

## Voltcraft Install
```
voltcraft
	assets
		volt
		voltcraft
	bin
		volt.dll
		voltcraft.dll
		voltcraft.exe
		voltpy.dll
	python
		volt
			__main__.py
		voltcraft
			__main__.py
```

## Voltcraft Package
```
voltcraft
	assets
		voltcraft
	build
		android
		ios
		linux-amd64
		macos
		ps4
		ps5
		webgl
		windows-x86
		windows-amd64
		xbox-360
		xbox-one
		xbox-series-x
	cache
		cmake
			bin
				volt.dll
				app.exe
				voltcraft.dll
			generated
				volt.cpp
				voltcraft.cpp
	python
		voltcraft
			__main__.py
	include
		voltcraft
	src
		voltcraft
	CMakeLists.txt
	package.json
```
