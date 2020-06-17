#! /bin/bash

set -e -x

if [ -z "${BUILD_TYPE}" ]; then
  echo BUILD_TYPE must be defined
  exit 2
fi

if [ -z "${DOCKER_REPO}" ]; then
  echo DOCKER_REPO must be defined
  exit 2
fi

if [ -z "${DOCKER_USERNAME}" ]; then
  echo DOCKER_USERNAME must be defined
  exit 2
fi

if [ -z "${DOCKER_PASSWORD}" ]; then
  echo DOCKER_PASSWORD must be defined
  exit 2
fi

if [ -z "${GITHUB_REPO}" ]; then
  echo GITHUB_REPO must be defined
  exit 2
fi

if [ -z "${GITHUB_USERNAME}" ]; then
  echo GITHUB_USERNAME must be defined
  exit 2
fi

if [ -z "${GITHUB_OAUTH_TOKEN}" ]; then
  echo GITHUB_OAUTH_TOKEN must be defined
  exit 2
fi

TAGS=$(curl -u "${GITHUB_USERNAME}:${GITHUB_OAUTH_TOKEN}" "https://api.github.com/repos/${GITHUB_REPO}/tags")
export MESOS_GIT=https://github.com/${GITHUB_REPO}.git
MESOS_VERSIONS="$(echo "$TAGS" | jq '.[].name' -r | sort -h | grep -E "^([0-9]{1,}\.)+[0-9]{1,}")"
DOCKER_TAGS_URL="https://hub.docker.com/v2/repositories/${DOCKER_REPO}/tags?page_size=1000"
DOCKER_TAGS="$(curl "${DOCKER_TAGS_URL}" | jq -r '.results|.[]|.name')"

set +e

for MESOS_VERSION in $MESOS_VERSIONS; do
  echo "Checking mesos version: ${MESOS_VERSION}"
  IMAGE=$DOCKER_REPO:$MESOS_VERSION-$BUILD_TYPE
  if grep -q "^$MESOS_VERSION-$BUILD_TYPE\$" <<< "$DOCKER_TAGS"; then
    echo "Image $IMAGE already up to date on docker hub"
  else
    echo "Rebuilding image $IMAGE"
    echo "${DOCKER_PASSWORD}" | docker login -u "${DOCKER_USERNAME}" --password-stdin
    export MESOS_VERSION
    ./build_and_push.sh
  fi
done
