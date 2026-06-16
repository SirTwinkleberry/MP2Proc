FROM python:latest

WORKDIR /root

RUN apt-get update && apt-get clean

RUN pwd

ADD . /root/VolProc/

RUN mv /root/VolProc/externals/libboost_timer /usr/local/lib/libboost_timer.so.1.83.0

RUN echo export LD_LIBRARY_PATH=/usr/local/lib >> ~/.bashrc

RUN pip install numpy==1.26.4 matplotlib==3.8.0