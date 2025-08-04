# Thrask

This technique involves stealing access tokens from privileged processes to impersonate users or services. The tool (Thrask) enables SeDebugPrivilege, grabs the token from a target PID, duplicates it, and spawns a new process (like cmd.exe) running under that token — simulating how real APTs escalate privileges and move laterally.

# Usage

```sh
g++ -o thrask.exe thrask.cpp -ladvapi32
```

```sh
thrask.exe [PID TARGET]
```
