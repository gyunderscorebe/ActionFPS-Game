# ActionFPS Game [![Build Status](https://travis-ci.org/ActionFPS/ActionFPS-Game.svg?branch=master)](https://travis-ci.org/ActionFPS/ActionFPS-Game) [![Join the chat at https://gitter.im/ScalaWilliam/ActionFPS](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/ScalaWilliam/actionfps?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Build status](https://ci.appveyor.com/api/projects/status/dx4x857jldgx5d9h/branch/master?svg=true)](https://ci.appveyor.com/project/ScalaWilliam/actionfps-game/branch/master)

Game client and server for https://actionfps.com/ | http://github.com/ScalaWilliam/ActionFPS/

## Test

### Windows

 * Clone
```
git clone https://github.com/lucasgautheron/ActionFPS-Game.git
```
 * Install Windows Visual C++ Studio Express 2010 
 * Open source/vcpp/cube.vcxproj and build "Release"
 * Launch actionfps_release.bat

### Linux

 * Clone
```
git clone https://github.com/lucasgautheron/ActionFPS-Game.git
```
 * Compile and run
```
cd source/src
make install
cd ../..
./actionfps.sh
```

## Test server

We're running an always up-to-date server (synchronized with the branch that is being test) at woop.ac:7654 :

```
/connect woop.ac 7654
```
## Contribute
 * Have a look at the current [projects](https://github.com/lucasgautheron/ActionFPS-Game/projects)
 * Check out open [issues](https://github.com/lucasgautheron/ActionFPS-Game/issues?q=is%3Aissue+is%3Aopen), especially [high-priority issues](https://github.com/lucasgautheron/ActionFPS-Game/issues?q=is%3Aissue+is%3Aopen+label%3Ahigh-priority)
 * Come and talk to us at our gitter [![Join the chat at https://gitter.im/ScalaWilliam/ActionFPS](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/ScalaWilliam/actionfps?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
 * Fork the repository and submit patches
