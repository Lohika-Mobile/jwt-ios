# jwt-ios

**JWT** is a framework project to demo **code coverage data automation** on iOS.

Project comes with a set of xcconfig files and shell scripts.

Dependencies
-
1. Install the following Ruby gems:
	* *sudo gem install xcpretty*
1. Install the lcov tool:
	* *brew install lcov*
	
Developing
-
To build the project please use **JWT.xcodeproj** file and one of the available schemes - *JWT-Debug* or *JWT-Release*.

Code Coverage
-
To collect code coverage please use the shell script:

	./Scripts/run-unit-tests.sh debug arm64
	or
	./Scripts/run-unit-tests.sh debug i386

Output will be available at *./Autobuild/unit-tests/coverage*.
