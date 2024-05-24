import os
import sys
import subprocess
from serial.tools import list_ports

def check_docker():
    try:
        subprocess.run(
            ['docker'], 
            capture_output=True, text=True,
            check=True
        )
        return True
    except subprocess.CalledProcessError as e:
        print("Docker is not installed or not accessible.")
        print(e)
        return False
    
def build_docker_image():
    current_dir = os.path.abspath(os.getcwd())
    parent_dir = os.path.dirname(current_dir)

    print("Building Docker image...")

    os.chdir(parent_dir)

    # docker build -t fstick-build .
    subprocess.run(['docker', 'build', '-t', 'fstick', '.'], check=True)
    print("Docker image built. Exporting firmware...")

    # docker create --name temp-fstick-build fstick-build
    subprocess.run(['docker', 'create', '--name', 'temp-fstick-build', 'fstick-build'], check=True)
    print("Docker container created.")

    # docker cp temp-fstick-build:/root/fstick/fstick.ino .
    subprocess.run(['docker', 'cp', 'temp-fstick-build:/root/fstick/fstick.ino', '.'], check=True)
    print("Firmware exported.")

    # docker rm temp-fstick-build
    subprocess.run(['docker', 'rm', 'temp-fstick-build'], check=True)
    print("Docker container removed.")


def install_esptool():
    print("Installing esptool...")
    subprocess.run(['pip', 'install', 'esptool'], check=True)
    print("Dependencies installed.")

def select_serial_port():
    ports = list(list_ports.comports())
    if not ports:
        print("No serial ports detected.")
        return False

    print("Available serial ports:")
    for idx, port in enumerate(ports):
        print(f"{idx + 1}: {port}")

    selection = input(f"Select the number of the serial port to use: ")

    try:
        index = int(selection) - 1
        if 0 <= index < len(ports):
            return ports[index].device
        else:
            print("Invalid selection.")
            return None
    except ValueError:
        print("Invalid input. Please enter a number.")
        return None

def flash_firmware(bootloader_path, partitions_path, firmware_path, serial_port):
    esptool_path = 'esptool'  

    flash_command = [
        esptool_path,
        '--chip', 'auto', '--port', serial_port, 
        'write_flash', '-z', '--flash_mode', 'dio', '--flash_freq', '40m', '--flash_size', 'detect',
        '0x1000', bootloader_path,
        '0x8000', partitions_path,
        '0x10000', firmware_path 
    ]

    print(f"Flashing firmware '{firmware_path}' to '{serial_port}'...")

    try:
        subprocess.run(flash_command, check=True)
        print("Flash complete.")
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error flashing firmware: {e}")
        return False


def main():
    if sys.platform == 'linux' or sys.platform == 'linux2':
        print("Running on Linux")
    elif sys.platform == 'win32':
        print("Running on Windows")
    else:
        raise Exception(f'Unsupported OS ({sys.platform})')
    
    if not check_docker():
        raise Exception("Docker is not installed or not accessible. Please follow https://github.com/IMXNOOBX/fstick#-automatic-build")

    build_docker_image()

    bootloader_path = '../build/fstick.ino.bootloader.bin'
    partitions_path = '../build/fstick.ino.partitions.bin'
    firmware_path = '../build/fstick.ino.bin'

    if not os.path.exists(firmware_path) or not os.path.exists(partitions_path) or not os.path.exists(bootloader_path):
        print(f"Could not find firmware file '{firmware_path}'. Please build the firmware first. See README.md for instructions.")
        exit(0)


    serial_port = select_serial_port()
    while serial_port is None:
        serial_port = select_serial_port()

    if serial_port is False:
        exit(0)

    install_esptool()

    success = flash_firmware(bootloader_path, partitions_path, firmware_path, serial_port)

    if success:
        print("Firmware flash successful.")
    else:
        print("Firmware flash failed.")


if __name__ == '__main__':
    main()