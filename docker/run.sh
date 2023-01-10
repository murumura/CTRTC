#!/bin/bash
container_id=$(docker ps -aq --filter name=raytracer_container)
if [ ! -z "$container_id" ]
then
    docker rm ${container_id}
fi
if [[ $(uname -m) == 'arm64' ]]; then
  docker run \
    --platform linux/amd64\
    --rm \
    -it \
    -v $(pwd):/home/RayTracer \
    -p 1235:8888 \
    -m 64g \
    --memory-swap 128g \
    --name raytracer_container raytracer-dev
else
  docker run  --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it \
    -v $(pwd):/home/RayTracer \
    -p 1235:8888 \
    -m 64g \
    --memory-swap 128g \
    --name raytracer_container raytracer-dev
fi


