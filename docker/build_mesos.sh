#!/bin/sh

FLAGS="-Wno-deprecated-declarations -Wno-error=parentheses"

autoreconf --install -Wall --verbose

./configure \
    CFLAGS="$FLAGS" \
    CXXFLAGS="$FLAGS" \
    --disable-silent-rules \
    --prefix=/usr \
    --bindir=/usr/bin \
    --libdir=/usr/lib64 \
    --includedir=/usr/include \
    --disable-python \
    --disable-python-dependency-install \
    --enable-libevent \
    --enable-ssl \
    --enable-optimize \
    --with-network-isolator \
    --enable-seccomp-isolator \
    --enable-xfs-disk-isolator \
    --enable-perftools \
    "$@"
make -j "$(nproc)"
