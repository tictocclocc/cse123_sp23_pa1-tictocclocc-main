FROM ubuntu:22.04
RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y build-essential \
                           python3 \
                           python3-pip \
                           python3-dev \
                           libconfig-dev \
                           gdb \
                           git
