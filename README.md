[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/76mHqLr5)
# Description
_C Shell_
## Prompt
- Format:   ```<Username@SystemName:CurrDir>```
- The directory from which shell is invoked becomes the home directory for the shell and represented with “~”.
- Extra spaces and tabs are handled.
## Commands
- ### warp
    Works similar to 'cd' in bash by changing directories. Supports “.”, “..”, “~”, and “-” flags. Takes in multiple arguments in a single command.
- ### peek
    Format:<br>```peek <flags> <path/name>``` 
    <br>
    Lists all the files and directories in the specified directories in lexicographic order. Supports only -a and -l flags.
    -   -l : displays extra information, all and shown in bash
    -   -a : displays all files, including hidden files
    <!-- -->
    Displays only year for the files last modified before almost an year(365 days - 1min) ago. 
        
    Specific color coding to differentiate between file names, directories and executables in the output [green for executables, white for files and blue for directories].
- ### pastevents
    Similar to the actual history command in bash. Stores the 15 most recent command statements given as input.
    -   #### pastevents purge
        Clears all the pastevents currently stored.
    -   #### pastevents execute <index>

        Execute the command at position in pastevents (ordered from most recent to oldest). 
- ### proclore
    Format:<br>
    ```proclore <pid>```<br>
    Obtain information regarding a process. If an argument is missing, prints the information of the shell.

    Following information is printed:
    - PID
    - Process Status (R/R+/S/S+/Z)
    - Process group
    - Virtual Memory
    - Executable path of process
    
    Process states :
    - R/R+ : Running
    - S/S+ : Sleeping in an interruptible wait
    - Z : Zombie

    The “+” signifies whether it is a foreground or background process, i.e., add “+” only if it is a foreground process.
- ### seek
    Format: <br>
    ```seek <flags> <search> <target_directory>```<br>
    Looks for a file/directory in the specified target directory (or current if no directory is specified). It returns a list of relative paths (from target directory) of all matching files/directories (files in green and directories in blue) separated with a newline character.
    
    Flags :

    - -d : Only look for directories (ignore files even if name matches)
    - -f : Only look for files (ignore directories even if name matches)
    - -e : This flag is effective only when a single file or a single directory with the name is found. If only one file (and no directories) is found, then print it’s output. If only one directory (and no files) is found, then change current working directory to it. Otherwise, the flag has no effect. This flag should work with -d and -f flags. If -e flag is enabled but the directory does not have access permission (execute) or file does not have read permission, then output “Missing permissions for task!”
- ### activities
    Format: <br>
    ```activities```<br>
    Prints a list of all the active processes that were spawned by your shell, in the current run, in lexicographic order.
- ### ping
    Format: <br>
    ```ping <pid> <signal_number>```<br>
    Sends signals to processes. Takes signal number’s modulo with 32 before checking which signal it belongs to (assuming x86/ARM machine).
- ### fg
    Format: <br>
    ```fg <pid>```<br>
    Brings the running or stopped background process with corresponding pid to foreground, handing it the control of terminal.
- ### bg
    Format: <br>
    ```bg <pid>```<br>
    Changes the state of a stopped background process to running
- ### neonate
    Format: <br>
    ```neonate -n [time_arg]```<br>
    Prints the PID of the most recently created process on the system, this PID will be printed every [time_arg] seconds until the key ‘x’ is pressed

- ### iMan
    Format: <br>
    ```iMan <command_name>```<br>
    iMan fetches man pages from the internet using sockets and outputs it to the terminal (stdout). In this case, you are required to use the website : http://man.he.net/ to get the man pages.
## Signals
- Ctrl - C
    Interrupts any currently running foreground process by sending it the SIGINT signal. It has no effect if no foreground process is currently running.
- Ctrl - D
    Logs out of the shell (after killing all processes) while having no effect on the actual terminal.
## System commands
    Bash commands handled by execvp() is also handled.
    -   #### Foreground processes
        Control of terminal is handed over to this process for the time being while it is running. Time taken by the foreground process and the name of the process is printed in the next prompt if process takes > 2 seconds to run.

    -   #### Background processes
        Any command invoked with “&” is treated as a background command, but is not actually a background process. The shell will spawn that process but doesn’t hand the control of terminal to it. Whenever a new background process is started, the PID of the newly created background process is printed. Whenever background process finishes, a message displays (after command is entered), all background processes ended between the last command run.
 
## Errors
-   Whenever an error occurs, 'ERROR' is displayed in red, while the description is displayed in pruple.
# Assumptions
- 'SystemName' in the prompt is the hostname of the system.
- Not handling background processes for custom commands(commands implemented by me), including printing process ID.
- Not handling background processes for invalid commands, but printing their PID.
- Priting "time taken to execute" only for system foreground processes. Each command is considered, even when multiple commands are executed.
- System commands:
    - all space seperated strings are considered as an argument for the command
    - Not handling ```"``` explicitly.
-   'warp'
    - terminates when a single directory change fails
    - '-' flag has to be used alone (space seperated for multiple arguments), just like in bash.
- 'peek'
    -   multiple or wrong arguments give error.
    -   flags are not necessary
    -   ```peek -``` is an accepted command.
- 'pastevents'
    -   for ```execute <idx>``` , idx=1 displays the latest stored command.
    -   ignoring to store when shell exits without a command
    -   Doesn't store commands that have the term 'pastevents'.
- 'proclore'
    - only first argument after "proclore" is condsidered.
- 'seek'
    - 'search' can not be empty
- 'ping'
    - assuming only positive integer signals are valid
- 'iMan'
    - not handling the html tags in between the content
- I/O Redirection
    - throws error when the first I/O symbols don't have string next to it.
    - considers first file only when multiple files are entred for either input or output.
    - if the output is being redirected to a file and being piped, it is not piped and the pipe is empty
    - if the input is piped to and the next commands can take input from a file, it takes input only from the file and the pipe becomes empty