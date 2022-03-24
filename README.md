# BasicC2
This is repository to backup basic C2 malware.

It contains *solution* for Visual Studio Community 2022.

## Folder structure
`BasicC2` - complete C2 implant

`BasicC2TBD` - skeleton of C2 implant with missing:
  - `exec()` function to get cmd.exe execution
  - `persistence()` function to achieve persistance via Schkeduled Tasks

## Server side
- Since this implant only uses basic socket communication, basic ncat listener is enough.
- Ncat can be installed from (together with nmap): https://nmap.org/download
- Command to start listener: `ncat -nvlp 8080`
