import socket
import threading
from itertools import cycle, izip
import time

bind_ip = '0.0.0.0'
bind_port = 10000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((bind_ip, bind_port))
server.listen(5)  # max backlog of connections
def encryptDecrypt(input):
    key = [ 'K', 'C', 'Q' ]  # Can be any chars, and any size array
    output = [ ]

    for i in range(len(input)):
        xor_num = ord(input[ i ]) ^ ord(key[ i % len(key) ])
        output.append(chr(xor_num))

    return ''.join(output)

print 'Listening on {}:{}'.format(bind_ip, bind_port)
key = "B0HR"
def handle_client_connection(client_socket):
    request = encryptDecrypt(client_socket.recv(1024))
    f = open("logs/" + address[0],'a')
    print >>f, '{}'.format("Report Date: " + time.ctime())
    print >>f,'{}'.format(request)
    f.close()
    # client_socket.send('confirmed!')
    print("{}".format(request))
    client_socket.close()
while True:
    client_sock, address = server.accept()
    print 'Incoming logs from {}:{}'.format(address[0], address[1])
    client_handler = threading.Thread(
        target=handle_client_connection,
        args=(client_sock,)  # without comma you'd get a... TypeError: handle_client_connection() argument after * must be a sequence, not _socketobject
    )
    client_handler.start()