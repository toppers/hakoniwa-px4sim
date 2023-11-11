#!/bin/bash

IMAGE_NAME=`cat docker/image_name.txt`
IMAGE_TAG=`cat docker/version.txt`
DOCKER_IMAGE=${IMAGE_NAME}:${IMAGE_TAG}
DOCKER_FILE=docker/Dockerfile
docker build --platform linux/arm64 -t ${DOCKER_IMAGE} -f ${DOCKER_FILE} .

