# First stage: Builder (version independent)
FROM centos:7.6.1810 AS builder

RUN yum -y install epel-release
RUN yum -y install \
  apr-devel \
  automake \
  cmake3 \
  curl-devel \
  cyrus-sasl-devel \
  cyrus-sasl-md5 \
  gcc-c++ \
  git \
  libblkid-devel \
  libevent-devel \
  libnl3-devel \
  libseccomp-devel \
  libtool \
  make \
  maven \
  mlocate \
  openssl-devel \
  patch \
  python3 \
  subversion-devel \
  vim \
  xfsprogs-devel \
  zlib-devel

RUN alternatives \
  --install /usr/local/bin/cmake cmake /usr/bin/cmake3 20 \
  --slave /usr/local/bin/ctest ctest /usr/bin/ctest3 \
  --slave /usr/local/bin/cpack cpack /usr/bin/cpack3 \
  --slave /usr/local/bin/ccmake ccmake /usr/bin/ccmake3 \
  --family cmake

WORKDIR /src/mesos

ARG CONFIGURE_EXTRA_FLAGS

CMD autoreconf --install -Wall --verbose && \
  ./configure \
     CFLAGS="-Wno-deprecated-declarations" \
     CXXFLAGS="-Wno-deprecated-declarations" \
     --disable-silent-rules \
     --disable-python \
     --disable-python-dependency-install \
     --enable-libevent \
     --enable-ssl \
     --enable-optimize \
     --enable-seccomp-isolator \
     --with-network-isolator \
     --enable-xfs-disk-isolator \
     ${CONFIGURE_EXTRA_FLAGS} && \
  make -j "$(nproc)"

FROM builder AS runner

COPY docker/resources/mesos-agent /usr/bin/mesos-agent
COPY docker/resources/mesos-master /usr/bin/mesos-master

RUN chmod 0755 /usr/bin/mesos-master
RUN chmod 0755 /usr/bin/mesos-agent

RUN yum -y install ruby

CMD /usr/bin/mesos-agent

# Second stage: Building (version specific)
FROM builder AS builded

COPY . /src/mesos
WORKDIR /src/mesos
RUN autoreconf --install -Wall --verbose

RUN ./configure \
     CFLAGS="-Wno-deprecated-declarations" \
     CXXFLAGS="-Wno-deprecated-declarations" \
     --disable-silent-rules \
     --disable-python \
     --disable-python-dependency-install \
     --enable-libevent \
     --enable-ssl \
     --enable-optimize \
     --enable-seccomp-isolator \
     --with-network-isolator \
     --enable-xfs-disk-isolator \
     ${CONFIGURE_EXTRA_FLAGS}

RUN make -j "$(nproc)"

ENV MESOS_ROOT_DIR=/src/mesos
ENV MESOS_BUILD_DIR=/src/mesos

