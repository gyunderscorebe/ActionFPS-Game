# ActionFPS Game

Game client and server for http://actionfps.com/ | http://github.com/ScalaWilliam/ActionFPS/

[![Circle CI](https://circleci.com/gh/lucasgautheron/ActionFPS-Game.svg?style=svg&circle-token=036a7cf2fac640f9b8ba51635cc5ae56390343cb)](https://circleci.com/gh/lucasgautheron/ActionFPS-Game)

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
make 
cd ../..
./assaultcube.sh
```

## Test server

We're running an always up-to-date server (synchronized with the branch that is being test) at woop.ac:2500 :

```
/connect woop.ac 2500
```