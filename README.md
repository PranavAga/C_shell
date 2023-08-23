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

 
## Errors
-   Whenever an error occurs, 'ERROR' is displayed in red, while the description is displayed in pruple.
# Assumptions
- 'SystemName' in the prompt is the hostname of the system.
- Not handling background processes for custom commands(commands implemented by me), including printing process ID.
- Not handling background processes for invalid commands, but printing their PID.
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