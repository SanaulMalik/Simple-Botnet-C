# Simple-Botnet-C
server.c listens on a port. Every new connection adds a new thread, while the main thread continues listening.
A mutex locks the command segment, so that one client requests a commands at a time.

### Commands:
send TCP-ACK <ip> <packet-count> <br>
send TCP-SYN <ip> <packet-count> <br>
send TCP-XMAS <ip> <packet-count> <br>
send HTTP <ip> <packet-count>
