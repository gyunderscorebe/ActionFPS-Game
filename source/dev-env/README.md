# Docker image to build and run ActionFPS

    export ACTIONFPS_GAME=/path/to/ActionFPS-Game
    export AF_CONTAINER_ID=scalawilliam/actionfps-game-build:latest

## Running local ActionFPS server

    $ docker run \
      -p 30000:30000/udp \
      -v $ACTIONFPS_GAME:/game/ \
      -it \
      --entrypoint '' \
      $AF_CONTAINER_ID \
      bash -c 'cd /game/ && ./bin_unix/native_server'

## Building ActionFPS server

    $ docker run \
    -v $ACTIONFPS_GAME:/game/ \
    -it \
    --entrypoint '' \
    $AF_CONTAINER_ID \
    bash -c 'cd /game/source/src/ && make install'

## Building this image

    $ docker build -t actionfps-linux-build .

You can also run it:
    $ export AF_CONTAINER_ID=<container id>
    $ # use commands from above

## Pushing it

    $ docker tag <container id> scalawilliam/actionfps-game-build:latest
    $ docker push
