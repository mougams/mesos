#! /bin/sh

set -e

if [ -z "${MESOS_VERSION}" ]; then
  echo MESOS_VERSION must be defined
  exit 2
fi

if [ -z "${MESOS_GIT}" ]; then
  echo MESOS_GIT must be defined
  exit 2
fi

if [ -z "${DOCKER_REPO}" ]; then
  echo DOCKER_REPO must be defined
  exit 2
fi

if [ -z "${BUILD_TYPE}" ]; then
  echo BUILD_TYPE must be defined
  exit 2
fi

clone_dir=$(mktemp -d)

git clone "${MESOS_GIT}" "${clone_dir}"

git -C "${clone_dir}" checkout "${MESOS_VERSION}"

docker build "${clone_dir}" \
  -f "Dockerfile.${BUILD_TYPE}" \
  -t "${DOCKER_REPO}:${MESOS_VERSION}-${BUILD_TYPE}"

docker push "${DOCKER_REPO}:${MESOS_VERSION}-${BUILD_TYPE}"
