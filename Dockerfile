FROM ubuntu:22.04

# 기본 설정 및 정리
RUN apt-get clean && \
    rm -rf /var/lib/apt/lists/*


ARG USERNAME=dev
ARG USER_UID=1000
ARG USER_GID=$USER_UID


# Create a non-root user
RUN groupadd --gid $USER_GID ${USERNAME} \
 && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m ${USERNAME} \
 && mkdir /home/${USERNAME}/.config \
 && chown $USER_UID:$USER_GID /home/${USERNAME}/.config
 
# Set up sudo for the non-root user
RUN apt-get update -yq \
&& apt-get install -yq sudo \
&& echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} \
&& chmod 0440 /etc/sudoers.d/${USERNAME}
#  && rm -rf /var/lib/apt/lists/*

# Add user to dialout group to allow access to serial ports
RUN usermod -aG dialout ${USERNAME}

USER dev

RUN sudo apt-get update -yq
RUN sudo apt-get install -yq build-essential 
SHELL ["/bin/bash", "-c"]

ENTRYPOINT ["/bin/bash"]

# If no arguments are passed to the container, CMD is passed as arguments for ENTRYPOINT.
CMD ["bash"]