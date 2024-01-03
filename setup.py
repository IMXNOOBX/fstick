import os

def add_flags_to_platform(file_path):
    try:
        with open(file_path, 'r') as file:
            content = file.read()

        if 'build.extra_flags.esp32=-w' not in content:
            # Add -w to build.extra_flags
            content = content.replace('build.extra_flags.esp32=', 'build.extra_flags.esp32=-w ')
            content = content.replace('build.extra_flags.esp32s2=', 'build.extra_flags.esp32s2=-w ')
            content = content.replace('build.extra_flags.esp32s3=', 'build.extra_flags.esp32s3=-w ')
            content = content.replace('build.extra_flags.esp32c3=', 'build.extra_flags.esp32c3=-w ')
        else:
            print("(-w) Flag seems to be already be added in build.extra_flags.esp32=-w")

        if 'compiler.c.elf.libs.esp32=-zmuldefs' not in content: # just simple check to check if has already been added
            # Add -zmuldefs to compiler.c.elf.libs
            content = content.replace('compiler.c.elf.libs.esp32=', 'compiler.c.elf.libs.esp32=-zmuldefs ')
            content = content.replace('compiler.c.elf.libs.esp32s2=', 'compiler.c.elf.libs.esp32s2=-zmuldefs ')
            content = content.replace('compiler.c.elf.libs.esp32s3=', 'compiler.c.elf.libs.esp32s3=-zmuldefs ')
            content = content.replace('compiler.c.elf.libs.esp32c3=', 'compiler.c.elf.libs.esp32c3=-zmuldefs ')
        else:
            print("(-zmuldefs) Flag seems to be already be added in compiler.c.elf.libs.esp32=-zmuldefs")

        with open(file_path, 'w') as file:
            file.write(content)

        print(f'Successfully added flags to {file_path}')
    except Exception as e:
        print(f'Error: {e}')

def main():
    esp32_folder = os.path.join(os.getenv('LOCALAPPDATA'), 'Arduino15', 'packages', 'esp32', 'hardware', 'esp32')
    m5stack_folder = os.path.join(os.getenv('LOCALAPPDATA'), 'Arduino15', 'packages', 'm5stack', 'hardware', 'esp32')

    platform_files = ['platform.txt']

    for folder in [esp32_folder, m5stack_folder]:
        if os.path.exists(folder):
            print(folder)
            for root, dirs, files in os.walk(folder):
                for current_folder in dirs:
                    for platform_file in platform_files:
                        file_path = os.path.join(root, current_folder, platform_file)
                        if os.path.exists(file_path):
                            add_flags_to_platform(file_path)
                        # else:
                            # print(f'{platform_file} not found in {current_folder}')
        else:
            print(f'{folder} not found')

if __name__ == "__main__":
    main()
