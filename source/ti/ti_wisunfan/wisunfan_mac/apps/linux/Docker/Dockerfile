# Docker context has to bee the root of the repo
FROM ubuntu:16.04

ENV CONAN_USER="" CONAN_PASS=""

# copy proxy files
COPY --chown=root:root apt_proxy.trusty /etc/apt/apt.conf.d/proxy
COPY --chown=root:root environment /etc/environment
COPY --chown=root:root sudoers_proxy /etc/sudoers.d/proxy

SHELL ["/bin/bash", "-c"]

#RUN sudo rm /etc/apt/sources.list.d/ubuntu-esm-infra-trusty.list && \
RUN apt-get update && \
  apt-get install -y \
    build-essential \
    curl \
    dh-autoreconf \
    dos2unix \
    git \
    make \
    pkg-config \
    python3-pip \
    unzip \
    tcsh \
    zip

RUN useradd -ms /bin/bash developer
USER developer
WORKDIR /home/developer

RUN python3 -m pip install conan==1.22 --proxy=http://webproxy.ext.ti.com:80
RUN mkdir ~/.ssh && mkdir -p ~/project/154stack_env


