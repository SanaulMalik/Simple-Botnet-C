# Simple-Botnet-C
botmaster.c listens on a port. Every new connection adds a new thread, while the main thread continues listening.
A mutex locks the command segment, so that one bot requests a command at a time.
The bot then accepts the commands and executes them.

## Assumptions
The bot has already been delivered to the target machine. The bot's priviliges has been elevated to system.

## Setup
<ol>
  <li>Compile the botmaster as:  gcc botmaster.c -o botmaster -lpthread</li>
  <li>Compile the bot as: gcc bot.c -o bot -lpthread</li>
 </ol>
 
 ## Running
 ### Botmaster
 Execution on the botmaster is as follows.
 #### Execution:
 <ul>
 <li>Execute the botmaster as "./botmaster". The botmaster runs and waits for incoming connections.</li>
 <li>When a client connects,the botmaster accepts the connection and continues listening for more connections.</li>
 <li>The botmaster displayes the hostname, IP and socket descriptor for the connection. It then prompts for input.</li>
 <li>Any valid command can be executed. To exit, simply enter "exit". This terminates the connection with the corresponding bot.</li>
 <li>After execution, the execution status is displayed as either "Command executed" or "Command failed".</li>
 <li>The botmaster then switches to the next connection and prompts for input</li>
  </ul>
  
  Commands of the following format can be executed.
  #### Commands
  <ol>
    <li>SEND TCP-SYN ip_address number_of_packets</li>
    <li>SEND TCP-ACK ip_address number_of_packets</li>
    <li>SEND TCP-XMAS ip_address number_of_packets</li>
    <li>SEND UDP ip_address number_of_packets</li>
    <li>SEND HTTP ip_address number_of_packets</li>
  </ol>
 
 ### Bot
 The bot only has to be run as "sudo ./bot". We require the sudo command as we assume that the bot as already acquired the required priviliges to execute all the commands
  
