import os

def add_flags_to_platform(file_path):
    try:
        with open(file_path, 'r') as file:
            content = file.read()

        # Check if flags are already present
        if '-w' not in content:
            # Add -w to build.extra_flags
            content = content.replace('build.extra_flags.esp32=', 'build.extra_flags.esp32=-w ')
            content = content.replace('build.extra_flags.esp32s2=', 'build.extra_flags.esp32s2=-w ')
            content = content.replace('build.extra_flags.esp32s3=', 'build.extra_flags.esp32s3=-w ')
            content = content.replace('build.extra_flags.esp32c3=', 'build.extra_flags.esp32c3=-w ')

        if '-zmuldefs' not in content:
            # Add -zmuldefs to compiler.c.elf.libs
            content = content.replace('compiler.c.elf.libs.esp32=', 'compiler.c.elf.libs.esp32=-zmuldefs ')
            content = content.replace('compiler.c.elf.libs.esp32s2=', 'compiler.c.elf.libs.esp32s2=-zmuldefs ')
            content = content.replace('compiler.c.elf.libs.esp32s3=', 'compiler.c.elf.libs.esp32s3=-zmuldefs ')
            content = content.replace('compiler.c.elf.libs.esp32c3=', 'compiler.c.elf.libs.esp32c3=-zmuldefs ')

        with open(file_path, 'w') as file:
            file.write(content)

        print(f'Successfully added flags to {file_path}')
    except Exception as e:
        print(f'Error: {e}')

def main():
    arduino_folder = os.path.join(os.getenv('LOCALAPPDATA'), 'Arduino15', 'packages', 'esp32', 'hardware', 'esp32', '2.0.3')
    m5stack_folder = os.path.join(os.getenv('LOCALAPPDATA'), 'Arduino15', 'packages', 'm5stack', 'hardware', 'esp32', '2.0.3')

    platform_files = ['platform.txt']

    for folder in [arduino_folder, m5stack_folder]:
        if os.path.exists(folder):
            for platform_file in platform_files:
                file_path = os.path.join(folder, platform_file)
                if os.path.exists(file_path):
                    add_flags_to_platform(file_path)
                else:
                    print(f'{platform_file} not found in {folder}')
        else:
            print(f'{folder} not found')

if __name__ == "__main__":
    main()
