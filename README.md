# ActionFPS Game [![Build Status](https://travis-ci.org/ActionFPS/ActionFPS-Game.svg?branch=master)](https://travis-ci.org/ActionFPS/ActionFPS-Game) [![CircleCI](https://circleci.com/gh/ActionFPS/ActionFPS-Game.svg?style=svg)](https://circleci.com/gh/ActionFPS/ActionFPS-Game) [![Build status](https://ci.appveyor.com/api/projects/status/dx4x857jldgx5d9h/branch/master?svg=true)](https://ci.appveyor.com/project/ScalaWilliam/actionfps-game/branch/master)

> Game client and server for [ActionFPS](https://actionfps.com/)

_[ActionFPS Portal](https://actionfps.com/)
· [ActionFPS Documentation](https://docs.actionfps.com)
· [Development Guide](https://docs.actionfps.com/portal-development-guide.html)
· [Contributor Guide](https://docs.actionfps.com/contributor-guide.html)_

# Quickstart

You can either Download an [automatically built development release](https://github.com/ActionFPS/ActionFPS-Game/releases),
or run from source.

## Run from source

### Windows

1. Install Windows Visual C++ Studio Express 2010 
2. Open `source/vcpp/cube.vcxproj` and build "Release"
3. Launch `actionfps_release.bat`.

### Linux
```
$ cd source/src
$ make install
$ cd ../..
$ ./actionfps.sh
```
## Connect to a test server
This server is synchronised with the [`master` branch](https://help.github.com/articles/github-glossary/#branch).

In the game, type: `/connect woop.ac 7654`
