FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install system tools and languages
RUN apt-get update && apt-get install -y \
    build-essential cmake git curl wget vim nano \
    openjdk-17-jdk \
    python3 python3-pip python3-venv \
    r-base \
    software-properties-common unzip zip ca-certificates \
    gnupg2 lsb-release \
    nodejs npm \
    openssh-server \
    sudo && \
    rm -rf /var/lib/apt/lists/* && \
    mkdir -p /var/run/sshd
    # ssh-keygen -A

# SSH config
# RUN echo "PasswordAuthentication yes" >> /etc/ssh/sshd_config && \
#     echo "PermitRootLogin no" >> /etc/ssh/sshd_config && \
#     echo "UsePAM no" >> /etc/ssh/sshd_config



# Set JAVA environment variables
ENV JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
ENV PATH=$JAVA_HOME/bin:$PATH
ENV TZ=${TZ}

ARG SSH_USER
ARG SSH_PASSWORD
# ENV SSH_USER=${SSH_USER}
# ENV SSH_PASSWORD=${SSH_PASSWORD}



# Create a user 'devuser' with password 'devpass' and grant sudo access
# RUN apt-get update && apt-get install -y passwd && \
#     useradd --create-home --shell /bin/bash --no-log-init ${SSH_USER} && \
#     echo "${SSH_USER}:${SSH_PASSWORD}" | chpasswd && \
#     usermod -aG sudo ${SSH_USER} && \
#     echo "${SSH_USER} ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

RUN useradd -ms /bin/bash ${SSH_USER} && \
    echo "${SSH_USER}:${SSH_PASSWORD}" | chpasswd && \
    adduser ${SSH_USER} sudo

# Setup SSH server
# RUN mkdir /var/run/sshd && \
#     echo "PasswordAuthentication yes" >> /etc/ssh/sshd_config && \
#     echo "PermitRootLogin no" >> /etc/ssh/sshd_config && \
#     echo "UsePAM no" >> /etc/ssh/sshd_config

# Expose SSH port
EXPOSE 22

# Set default user and working directory
USER ${SSH_USER}
WORKDIR /home/${SSH_USER}

# Start SSH server
CMD ["/usr/sbin/sshd", "-D"]

