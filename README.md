# pipe_in_c
Description: This program is working as a pipeline in linux.
An pipeline is when you do several commands but the first command
send's the out data to the next command and that command takes the data and use it 
with it's own command, and so on.
An exempel of pipeline in linus is "cat -n mexec.c | grep -B4 -A2 return | less".
This program will take a text-file as a parameter with each command on a new line.
You can also run the program like "./mexec < file" instead of a file input. 
This will take the commands from text-file and store it in stdin buffer.
You can also run the program with "./mexec" and type in the command by your self.
After entering the commands you press ctrl-d to tell the program to run.

Exampel of a text-file or in data by user:
cat -n mexec.c
grep -B4 -A2 return
less