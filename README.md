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