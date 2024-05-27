FROM python:3.9-slim AS builder

RUN apt-get update && apt-get install -y \
    curl \
    unzip \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

RUN pip install pyserial

RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

ENV PATH=$PATH:/root/bin

# Install Arduino core for ESP32
RUN arduino-cli core update-index \
    && arduino-cli core install esp32:esp32

# Install additional libraries
COPY setup/libraries.txt /libraries.txt
COPY setup/package_m5stack_index.json /package_m5stack_index.json
RUN arduino-cli core install  m5stack:esp32 --additional-urls "file:///package_m5stack_index.json"

RUN arduino-cli core search m5stack
RUN arduino-cli board install

RUN arduino-cli lib install $(cat /libraries.txt)

COPY setup/setup.py /setup_flags.py

COPY . /fstick
WORKDIR /fstick

# Run to the script to modify compiler flags
RUN python /setup_flags.py

# Compile the sketch
RUN arduino-cli compile --fqbn m5stack:esp32:m5stack_stickc_plus /fstick --build-path "./build" --verbose

RUN ls -la /fstick

FROM python:3.9-slim

COPY --from=builder /fstick/build /fstick/build 

WORKDIR /fstick

# Flash the compiled binary to the device
# Linux
# CMD ["bash", "-c", "esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x1000 /fstick/build/fstick.ino.bin"]
# Windows
# CMD ["bash", "-c", "esptool.py --chip esp32 --port COM3 --baud 115200 write_flash -z 0x1000 /fstick/build/fstick.ino.bin"]