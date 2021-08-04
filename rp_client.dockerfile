# Using the latest long-term-support Ubuntu OS
FROM ubuntu:16.04

RUN apt -y update
RUN apt -y upgrade
RUN apt install -y libzmq3-dev
RUN apt install -y g++ sdb libjsoncpp-dev gdb
RUN apt install -y vim man
RUN apt install -y tree curl
RUN apt install -y git wget cmake

RUN apt install -y iputils-ping
RUN apt install -y curl zip unzip tar

RUN apt update && apt install -y software-properties-common && add-apt-repository -y ppa:deadsnakes/ppa
RUN apt update && apt install -y python3.5

ENV HOME=/home/oe

WORKDIR /home/oe/cpp
#WORKDIR /home/oe/cpp/rp_server


COPY ./ $HOME

RUN python3.5 $HOME/get-pip.py

RUN ln -s /usr/bin/python3.5 /usr/bin/python

#RUN ln -s /usr/bin/python3.7 /usr/bin/python && \
#	ln -s /usr/bin/pip3.7 /usr/bin/pip

RUN pip install flask
RUN pip install pyzmq

RUN mv $HOME/vimrc $HOME/.vimrc

ENV FLASK_APP=rp_gui.py
ENV FLASK_DEBUG=1

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

EXPOSE 5005
