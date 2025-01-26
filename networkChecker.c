#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>


#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <errno.h>


// #define IP "192.168.1.10"
// #define PORT 3000

#define ERR_TOO_FEW_ARGS 1
#define ERR_SOCKET_INIT 2
#define ERR_TO_NETWORK_BYTE 3
#define ERR_INVALID_CONNECTION 4
#define ERR_CONN_TO_SRVR 5
#define ERR_PYTHON_FORK 6
#define ERR_IOCTL_CALL 7

bool isConnected = false;

// Simplifies error checking
void checkErr(int varToCheck, char* errorDesc, int errCode, bool ignore) {

    if (varToCheck == -1) {
        fprintf(stderr, "Error: %s\n", errorDesc);

        //perror("Error");

        if (!ignore) {
            exit(errCode);
        }
    }

}

// Allows millisecond precision with sleep
int sleepMS(int ms) {
    // usleep is in microseconds and micro -> ms is micro * 1000
    // https://www.geeksforgeeks.org/sleep-function-in-c/
    usleep(ms * 1000);

    return 0;
}

// Checks id ethernet cable is plugged in and if so, will check if 
int checkConnection(int _fd, struct sockaddr_in addr){

    // structure that contains ifr_name and a union
    // https://man7.org/linux/man-pages/man7/netdevice.7.html
    struct ifreq ifr;



    // 'eth0' is the driver name for ethernet output when typing 'ipconfig' in terminal
    char ethName[] = "eth0";

    // Coies value of ethName ('eth0') into ifr.ifr_name - stands for interface name
    // 5 the interface name length since there a 4 characters and null terminator
    strncpy(ifr.ifr_name, ethName, 5);

    // Creates ioctl call on socket
    // SIOCGIFFLAGS are flags for the device (eth0) and contains flags such as IFF_UP and IFF_RUNNING
    // https://man7.org/linux/man-pages/man7/netdevice.7.html
    int ethernetRunning = ioctl(_fd, SIOCGIFFLAGS, &ifr);

    checkErr(ethernetRunning, "Error creating ioctl call.", ERR_IOCTL_CALL, false);

    // ifr.ifr_flags is a short set to 0 or 1; I assume it indicates if a flag exists
    // binary AND operator since the first is a short and IFF_RUNNING is an int
    // IFF_RUNNING tells us if there is a physical connection of this computer 
    // and another via the device (eth0; our ethernet port)
    if (ifr.ifr_flags & IFF_RUNNING) {
        // nothing to see here
    } else {
        // We close the socket and kill this function is IFF_RUNNING is not set since it indicates no physical, and therefore no digital, connection
        printf("Ethernet cable is not plugged in!\n");

        close(_fd);
        shutdown(_fd, SHUT_RDWR);

        return 0;
    }

    // Since we've established there is a physical connection, the specified ip DOES exist
    // Therefore, we can check if the specified port is open
    if (connect(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {

        //perror("error");

        // errno 111: Connection refused. Indicates that the port is not open
        if (errno == 111) {
            printf("Ethernet cable connected, but PORT:%i is not set open.\n", ntohs(addr.sin_port));
        }

        close(_fd);
        shutdown(_fd, SHUT_RDWR);

        isConnected = false;

        return 0;

    } else {
        // Server is live and it can be connected to.
        close(_fd);
        shutdown(_fd, SHUT_RDWR);

        //isConnected = true;

        return 1;
    }

}

int main(int argc, char** argv) {

    
    if (argc < 2) {
        fprintf(stderr, "Pass both an IP and Port. (In that order) \n");
        exit(ERR_TOO_FEW_ARGS);
    }

    char *IP = argv[1];
    int PORT = atoi(argv[2]);
    // const ip = argv[0];
    const int sleepTime = 2000;


    while (true) {

        struct sockaddr_in address;
        int socketFD;

        // The '|' operator is a bitwise or operator.
        // Since SOCK_STREAM and SOCK_NONBLOCK host different bits, it combines them so that both are performed simultaneously
        socketFD = socket(AF_INET, SOCK_STREAM, 0);
        checkErr(socketFD, "Failed to initialize socket.", ERR_SOCKET_INIT, false);

        time_t sec = 0;
        suseconds_t usec = 500000;

        struct timeval timeout = {sec, usec};
        
        setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, &timeout, 1);

        //printf("test\n");

        address.sin_family = AF_INET;
        address.sin_port = htons(PORT);

        int conversion = inet_pton(AF_INET, IP, &address.sin_addr.s_addr);
        checkErr(conversion, "Error converting from standard IP to Network Byte format.", ERR_TO_NETWORK_BYTE, false);
        

        //printf("IP: %s, Port: %" PRIu16 "\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));


        int connection = checkConnection(socketFD, address);

        checkErr(connection, "Error connecting to server.", ERR_CONN_TO_SRVR, true);

        
        // Runs python script
        if (connection && !isConnected) {

            printf("\n=========================================\n\n\n");
            printf("Connected to IP: %s, PORT: %i\n", IP, PORT);
            printf("\n\n=========================================\n\n");
            // Creates child process that will wait on the python program to end top continue the parent program 
            // aka: momentarily stops monitoring connection (the daemon) until it ceases to exist again (handled by python script)
            pid_t pid = fork();

            checkErr(pid, "Error initializing python fork.", ERR_PYTHON_FORK, true);

            // error in creating fork, not too big of a deal here; will try again in the next cycle
            if (pid == -1) {
                continue;
            }

            // executes python script in current process, waits on it to exit/terminate and then exits the child process (itself) 
            if(pid == 0) {
                execl("/usr/bin/python3", "python3", "/home/rsx25/Desktop/Software/ethernetTest/client.py", NULL);
                wait(NULL);
                exit(0);
                
            }

            isConnected = true;
        }
        
        //fprintf(stderr, "Waiting %i milliseconds.\n", sleepTime);
        //fprintf(stderr, "connected: %d\n", isConnected);
        sleepMS(sleepTime);
        
        fflush(stdout);
    }
    
    return 0;
}