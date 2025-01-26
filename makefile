CFLAGS= -Wall -pedantic -std=gnu99

all: networkChecker

networkChecker:
	gcc networkChecker.c -o build/networkCheckerd