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
RUN apt update && apt install -y python3.7

ENV HOME=/home/oe

WORKDIR /home/oe/cpp/rp_server


# RUN mkdir -p /home/oe/vcpkg && \
# 	git clone https://github.com/Microsoft/vcpkg /home/oe/vcpkg &&\
# 	/home/oe/vcpkg/bootstrap-vcpkg.sh

COPY ./ $HOME

RUN python3.7 $HOME/get-pip.py

RUN ln -s /usr/bin/python3.7 /usr/bin/python && \
	ln -s /usr/bin/pip3.7 /usr/bin/pip

RUN pip install flask
RUN pip install pyzmq

RUN mv $HOME/vimrc $HOME/.vimrc

# RUN make rp_server
# CMD ["/home/oe/cpp/rp_server/rp_server"]

ENV FLASK_APP=rp_gui.py
ENV FLASK_DEBUG=1

EXPOSE 5005
