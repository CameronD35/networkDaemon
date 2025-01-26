# Network Checker Daemon

This daemon:
- Checks if the specified IP and Port are open
- Connects to the IP and Port and if successful, creates a fork program
- The fork program runs a python script until the connection is disrupted (The python script is not provided, but it will be added in a later push!)

---

## Building the daemon:

To build the daemon, you can use the makefile OR you can use your own C compiler to combine the code--whichever you prefer.

* If you find time to review the code, please free to make suggestions

### PLEASE NOTE:
This daemon is intended to run on LINUX only. It has only been testedon Ubuntu 22.04 and I cannot guarantee it will work on other distributions.
