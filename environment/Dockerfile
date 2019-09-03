FROM registry.codeocean.com/codeocean/ubuntu:16.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
      "cmake=3.5.1-1ubuntu3" \
      "g++=4:5.3.1-1ubuntu1" \
      "gdb=7.11.1-0ubuntu1~16.5" \
      "ghostscript=9.26~dfsg+0-0ubuntu0.16.04.9" \
      "gnuplot=4.6.6-3" \
      "graphviz=2.38.0-12ubuntu2.1" \
      "imagemagick=8:6.8.9.9-7ubuntu5.14" \
      "libalglib-dev=3.10.0-1" \
      "make=4.1-6" \
      "pkg-config=0.29.1-0ubuntu1" \
      "poppler-utils=0.41.0-0ubuntu1.14" \
      "pypy=5.1.2+dfsg-1~16.04" \
      "python-dev=2.7.12-1~16.04" \
      "python-numpy=1:1.11.0-1ubuntu1" \
      "python-opencv=2.4.9.1+dfsg-1.5ubuntu1.1" \
      "python-pandas=0.17.1-3ubuntu2" \
      "python-scipy=0.17.0-1" \
      "python2.7=2.7.12-1ubuntu0~16.04.4" \
      "strace=4.11-1ubuntu3" \
      "transfig=1:3.2.5.e-5ubuntu0.1" \
      "valgrind=1:3.11.0-1ubuntu4.2" \
    && rm -rf /var/lib/apt/lists/*


# Needed for OpenCV (used in Hyt-EM method)
COPY postInstall /
RUN /postInstall
