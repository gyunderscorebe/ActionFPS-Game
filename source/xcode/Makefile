.PHONY: default no-clean
default:
	xcodebuild -project actionfps.xcodeproj -target actionfps -configuration Release clean build
	xcodebuild -project actionfps.xcodeproj -target launcher -configuration Release clean build
no-clean:
	xcodebuild -project actionfps.xcodeproj -target actionfps -configuration Release build
	xcodebuild -project actionfps.xcodeproj -target launcher -configuration Release build
