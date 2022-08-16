rem copy assets and .dll files to output mkdir
robocopy "./resource" "./build/bin/Debug/assets" /E
robocopy "./windows/bin/" "./build/bin/Debug" /E