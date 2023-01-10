#!/bin/bash
if [[ $(uname -m) == 'arm64' ]]; then
    docker build --platform linux/amd64 -t raytracer-dev .
else
    docker build -t raytracer-dev .
fi