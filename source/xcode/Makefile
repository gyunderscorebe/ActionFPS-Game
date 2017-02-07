.PHONY: default no-clean
default:
	xcodebuild -project actionfps.xcodeproj -target actionfps -configuration Debug clean build
	xcodebuild -project actionfps.xcodeproj -target launcher -configuration Debug clean build
no-clean:
	xcodebuild -project actionfps.xcodeproj -target actionfps -configuration Debug build
	xcodebuild -project actionfps.xcodeproj -target launcher -configuration Debug build
