savedcmd_/home/kagu/code/BJTU_OS/OS-Lab/Lab5/myalarm.mod := printf '%s\n'   myalarm.o | awk '!x[$$0]++ { print("/home/kagu/code/BJTU_OS/OS-Lab/Lab5/"$$0) }' > /home/kagu/code/BJTU_OS/OS-Lab/Lab5/myalarm.mod