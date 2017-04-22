# ActionFPS Game [![Build Status](https://travis-ci.org/ActionFPS/ActionFPS-Game.svg?branch=master)](https://travis-ci.org/ActionFPS/ActionFPS-Game) [![CircleCI](https://circleci.com/gh/ActionFPS/ActionFPS-Game.svg?style=svg)](https://circleci.com/gh/ActionFPS/ActionFPS-Game) [![Build status](https://ci.appveyor.com/api/projects/status/dx4x857jldgx5d9h/branch/master?svg=true)](https://ci.appveyor.com/project/ScalaWilliam/actionfps-game/branch/master)

> Game client and server for [ActionFPS](https://actionfps.com/)

_See: [Development Guide](https://docs.actionfps.com/game-development-guide.html)
· [Contributor Guide](https://docs.actionfps.com/contributor-guide.html)_

# Quickstart

You have two choices:
1. Download a pre-built binary.
2. Run from sources.

## Download

You have two download choices:
* Directly from [automatically built development release](https://github.com/ActionFPS/ActionFPS-Game/releases)
* Through [ActionFPS Play page](https://actionfps.com/play).

## Run from source

### Windows

1. Install Windows Visual C++ Studio Express 2010.
2. Open `source/vcpp/cube.vcxproj` and build "Release".
3. Launch `actionfps_release.bat`.

### Linux

On Ubuntu or the like:

```
$ cd source/src
$ make install
$ cd ../..
$ ./actionfps.sh
```

### Mac

1. Install XCode
2. Dependencies with [brew](http://brew.sh): `brew install openssl jq`
3. Compile: `cd source/xcode && make && cd .. && cd ..`
4. Run: `open source/xcode/build/Release/actionfps.dmg`.

## Connect to a test server
This server is synchronised with the [`master` branch](https://help.github.com/articles/github-glossary/#branch).

In the game, type: `/connect woop.ac 7654` or open link `actionfps://woop.ac:7654` in your browser.
