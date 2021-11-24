#!/usr/bin/env python3
"""Dumps heap of a running process to a *.dump file. Adapted from
   https://unix.stackexchange.com/questions/6301/how-do-i-read-from-proc-pid-mem-under-linux
"""

if __name__ == "__main__":

    import re
    from sys import argv, exit

    if len(argv) != 2:
        exit(1)

    pid = int(argv[1])
    if (pid < 0):
        raise ValueError("Invalid PID")

    maps_file = open("/proc/{}/maps".format(pid), 'r')
    mem_file = open("/proc/{}/mem".format(pid), 'rb', 0)
    output_file = open("{}.dump".format(pid), 'wb')

    for line in maps_file.readlines():
        if "[heap]" in line:
            print(line, end='')
            match = re.match(r'([0-9A-Fa-f]+)-([0-9A-Fa-f]+)', line)
            start = int(match.group(1), 16)
            end = int(match.group(2), 16)
            mem_file.seek(start)
            heap_data = mem_file.read(end - start)
            output_file.write(heap_data)
            break

    maps_file.close()
    mem_file.close()
    output_file.close()
