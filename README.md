# BasicC2
This is repository to backup basic C2 malware.

It contains *solution* for Visual Studio Community 2022 (with C++ development module).

## Environment install and setup
1. Go to https://visualstudio.microsoft.com/cs/thank-you-downloading-visual-studio and download "Community" version of VisualStudio
2. Install it
3. After installation, select "Desktop development with C++" package and install it
4. Open solution file `BasicC2.sln` from this repository
5. Click on project with desired level: `BasicC2-STEPX` and click on "Select as Startup project"
6. Click on "Local Windows Debugger" and it lives! :)

## Folder structure
`BasicC2-STEP1` - intial state - basic functionality works, you need to fill in some functionality to get to `BasicC2-STEP2`
`BasicC2-STEP2` - final state to get to from `BasicC2-STEP1`
`BasicC2-STEP3` - second initial state - you need to fill in some functionality to get to `BasicC2-STEP4`
`BasicC2-STEP4` - final state to get to from `BasicC2-STEP3`

## Tasks
### To get from `BasicC2-STEP1` to `BasicC2-STEP2`
1. Implement `exec()` function to be able to execute passed function argument in CMD.EXE and returns output.
2. In function `persistance()` fill in command to create persistance of actual file using variables `exeFileLocation` and `serviceName`.
3. Add support for command `!exit`, which should kill the program once command recieved from server.
4. Hide console window of BasicC2.

### To get from `BasicC2-STEP3` to `BasicC2-STEP4`
1. In function `utilShowCredentialsPrompt()` update powershell script to display GUI capture prompt to legitimate user.
2. In function `utilScan()` update functionality so that it can scan multiple ports. Syntax: `!scan IP PORT1,PORT2,PORT3,...`.
3. In function `utilKeyloggerInit()` udpate function, so that attacker can specify time for which the keylogget will run.

## Server side
- Since this implant only uses basic socket communication, basic ncat listener is enough.
- Ncat can be installed from (together with nmap): https://nmap.org/download
- Or your can use zip file in repository 
- Command to start listener: `ncat.exe -nvlp 8080`
