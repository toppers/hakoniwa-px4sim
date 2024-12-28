#!/bin/bash

IMAGE_NAME=`cat docker/image_name.txt`
IMAGE_TAG=`cat docker/latest_version.txt`
DOCKER_IMAGE=toppersjp/${IMAGE_NAME}:${IMAGE_TAG}
DOCKER_FILE=docker/Dockerfile
docker build --platform linux/amd64 -t ${DOCKER_IMAGE} -f ${DOCKER_FILE} .

