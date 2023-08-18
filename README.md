# Description
_C Shell_
## Prompt
- Format: <Username@SystemName:CurrDir>
- The directory from which shell is invoked becomes the home directory for the shell and represented with “~”.
- Extra spaces and tabs are handled.
## Commands
- # warp
    Works similar to 'cd' in bash by changing directories. Supports “.”, “..”, “~”, and “-” flags. Takes in multiple arguments in a single command.

## Errors
-   Whenever an error occurs, 'ERROR' is displayed in red, while the description is displayed in pruple.
# Assumptions
- 'SystemName' in the prompt is the hostname of the system.
-   'warp'
    - terminates when a single directory change fails
    - '-' flag has to be used alone (space seperated for multiple arguments), just like in bash.