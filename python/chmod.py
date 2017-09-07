import os
import stat

file_l = os.listdir('.')
target_file=[name for name in file_l if name.endswith(('.sh','.py'))]

for file in target_file:
    os.chmod(file,os.stat(file).st_mode|stat.S_IXUSR)


