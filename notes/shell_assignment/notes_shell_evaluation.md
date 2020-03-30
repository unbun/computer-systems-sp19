
### Shell Evaluation Plan

You're writing a function "eval" which takes an AST and executes it

eval: 
* Base case: "command arg1 arg2"
	- first, check for builtins : "cd", "exit"
		- if so, don't fork
		- do the builtins
	- fork
	- in child: exec(command, [args...])
	- in parent: wait on child

* Semicolon: "command1; command2"
	- eval(command1)
	- eval(command2)

* And / OR: "command1 && command2", "command1 || command2"
	- AND:
		- status = eval(command1)
		- if(status == 0) : eval(command2)
	- OR:
		- status = eval(command1)
		- if(status != 0) : eval(command2) 

* Background "command1 &" , "command1 & command2" 
	- fork
	- in child: exec(command, [args...])
	- in parent: don't wait
	- NOTE: "command1 & command2" == "command1 &; command2" 

* Redirect: "command1 > file1", "command1 < file1"
	- fork()
	- in child : 
		- close(1) // 0 for case 2
		- fd = open(file1, write) // read for case 2
		- dup(fd)
		- close(fd)
		- status = eval(command1)
		- exit(status)
	- in parent: wait for child	

* Pipe: "command 1 | command2"
	- pipe(pipes[])
	- fork (1), in child:
		- close(stdout)
		- dup(pipes[1])
		- close(pipes[1], pipse[0])
		- status = eval(command1)
		- exit(status)
	- fork (2),
		- close(stdin)
		- dup(pipes[0])
		- close(pipes[1], pipse[0])
		- status = eval(command2)
		- exit(status)
	- close both ends of pipe
		-alternatively, do the whole pipe thing in a child
	- problem: we still have the pipe
	- *** PIPE AND REDIRECT ARE SIMILAR AND COULD BE ABSTRACTED OUT ***
		- Redirect uses open() to create an FD and Pipe uses pipe() to create two FDs


