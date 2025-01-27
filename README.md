# Network Checker Daemon

This daemon:
- Checks if the specified IP and Port are open
- Connects to the IP and Port and if successful, creates a fork program
- The fork program runs a Python script until the connection is disrupted. The python script IS provided, but the path will be different and the script is specific to our Jetson Orin Nano setup. To point to a new path you must change the path string at or around line 197 and direct it to your desired location.

---

## Building the daemon:

To build the daemon, you can use the makefile OR you can use your own C compiler to combine the code--whichever you prefer.

* If you find time to review the code, please free to make suggestions

### PLEASE NOTE:
This daemon is intended to run on LINUX only. It has only been tested on Ubuntu 22.04 and I cannot guarantee it will work on other distributions.
