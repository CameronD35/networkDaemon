'''

This file and the python file are not intended to belong here. So, if you wish to pair this program up with
the daemon, you must change the path specified at or around line 197 in 'networkChecker.c', recompile and run
This is just to make dowloading easier and reduce the number of repositories.

To download requirments: pip install -r /path/to/requirements.txt

'''



import socketio
import time
import json
import psutil
from platform import freedesktop_os_release, node

IP = '192.168.1.10'
port = 3000
os = freedesktop_os_release()
name = node()
# print(os)
# print(name)

# Seconds
dataFrequency = 2.5

sio = socketio.Client()

def sendStaticData():
    OS = freedesktop_os_release()['PRETTY_NAME']
    name = node()
    print('I SEND DATA')
    sendDataToServer([OS, name, IP], ['os', 'name', 'ip'])

@sio.event
def connect():
    print('connection established')
    sendStaticData()

@sio.event
def disconnect():
    print('disconnected')


#def commConnection(isConnected, IP, portNumber, timeElapsed):
    #print(isConnected, IP, portNumber, timeElapsed)

def changeDataFrequency(freq):
    global dataFrequency 
    dataFrequency = int(freq)
    print(f'Changed freq to {dataFrequency}')

def executeFile(fileName):
    print(f'Executing {fileName}')

def terminateFile(fileName):
    print(f'Terminating {fileName}')

def sendDataToServer(data, tags, eventName='data'):

    formattedData = {
        'msg': data,
        'tags': tags
    }

    #print(type(formattedData))

    if(isinstance(formattedData, (dict, list, tuple)) == True):
        formattedData = json.dumps(formattedData)

    sio.emit(eventName, formattedData)

    #print(f'sent datum: {data}, with tags: {tags}')

def getUsageData():
    memoryObj = psutil.virtual_memory()

    totalMemory = round(memoryObj.total/1000000000, 2)
    usedMemory = round(memoryObj.used/1000000000, 2)

    #print(usedMemory, totalMemory)

    memoryString = f'{usedMemory}GB / {totalMemory}GB'
    cpuUsage = str(psutil.cpu_percent()) + '%'

    return [memoryString, cpuUsage]

    

# sio.on('commConnection', commConnection)
sio.on('changeFreq', changeDataFrequency)
sio.on('reqStaticData', sendStaticData)
sio.on('executeFile', executeFile)
sio.on('terminateFile', terminateFile)

sio.connect(f'http://{IP}:{port}')

while sio.connected:

    memoryUsage, cpuUsage = getUsageData()

    sendDataToServer([cpuUsage, memoryUsage], ['cpupercent', 'memusage'])

    time.sleep(dataFrequency)

# data = {
#     'msg': 'msg test',
#     'tags': [
#         'hello',
#         'there'
#     ] 
# }




