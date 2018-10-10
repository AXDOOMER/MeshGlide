FROM debian:9

RUN apt update && \
    apt-get install -y g++ make libglfw3-dev libsdl2-image-dev libsdl2-dev libczmq-dev

ENV uid=1000 \
    gid=1000

# Replace 1000 with your user / group id
RUN export uid=$uid gid=$gid && \
    mkdir -p /home/developer && \
    echo "developer:x:${uid}:${gid}:Developer,,,:/home/developer:/bin/bash" >> /etc/passwd && \
    echo "developer:x:${uid}:" >> /etc/group
    # echo "developer ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/developer && \
    # chmod 0440 /etc/sudoers.d/developer && \



ADD run.sh  /home/developer/MeshGlide/
ADD ./Project/*.cpp ./Project/*.h  ./Project/*.png  ./Project/*.jpg ./Project/makefile  /home/developer/MeshGlide/Project/

RUN chown ${uid}:${gid} -R /home/developer

USER developer
ENV HOME /home/developer
WORKDIR /home/developer

CMD cd MeshGlide/Project && make
