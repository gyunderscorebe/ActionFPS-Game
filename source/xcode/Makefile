.PHONY: \
	default  \
	no-clean \
	clean \


default:
	xcodebuild -project actionfps.xcodeproj -target actionfps -configuration Release clean build
	rm -rf build/Release/launcher.app
	echo your artifact:
	ls -lsa $(PWD)/build/Release/actionfps.dmg
#	xcodebuild -project actionfps.xcodeproj -target launcher -configuration Release clean build
clean:
	rm -rf build
no-clean:
	xcodebuild -project actionfps.xcodeproj -target actionfps -configuration Release build
	xcodebuild -project actionfps.xcodeproj -target launcher -configuration Release build
