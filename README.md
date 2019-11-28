# exercitii_C

## 1. Practice C pointers and synchronisation

Implement a single-linked API(add,delete,print_list,sort_list):
 
- add_node - adds a new node to the end of list
- delete_node - deletes the specified element
- print_list - prints the nodes of the list(each element of the list has a member callback_print which stores the function executed to show the node informations)
- sort_list - re-arrange the list from the lower to the higher value of the nodes
- flush_list  - reset the list (delete all the nodes)
 
3 threads will be created and will do some work on the list :
 
Thread1: add(2)
         add(4)
                        add(10)
                        delete(2)
                        sort_list()
                        delete(10)
                        delete(5)
                       
Thread2: add(11)
         add(1)
                        delete(11)
                        add(8)
                        print_list()
                       
Thread3: add(30)
         add(25)
                        add(100)
                        sort_list()
                        print_list()
                        delete(100)
 
NODE informations:
            The node should have the following members beside next pointer:
            - callback for the print function to be used when displaying the element
            - val -> the value of the element added
 
Observations:
 
Use a synchronisation method to wait until all threads are created before starting executing threads instructions.
When executing the program print the action executed(add,delete,print,etc) by each thread and the thread id which executes it.
Before flushing the list at the end of the program display the list in the final state after threads actions.

## 2. Message queue exercise

The example below demonstrates interprocess communication between a server and clients using POSIX message queues in Linux. The server manages token numbers, which could be seat numbers for a flight, or something similar. It is server's job to give a token number to a client on request. In a typical scenario, there might be multiple clients requesting the server for token numbers. The server's message queue name is known to clients. Each client has its own message queue, in which server posts responses. When a client sends a request, it sends its message queue name. The server opens client's message queue and sends its response. The client picks up the response from its message queue and reads the token number in it. The process architecture looks like this. 

## 3. Sockets exercise

Sa se scrie o aplicatie client si una server cu urmatorele specificatii:
 
Programul server:
- Asculta pe un port(primit ca parametru) si primeste requesturi pentru diverse fisiere
- Trimite clientului fisierul care a fost solicitat sau mesajul "File not found" daca fisierul nu este disponibil
- Serverul poate primi requesturi de la mai multi clienti
 
Programul client:
- Realizeaza o conexiune cu serverul si trimite requesturi catre el pe acea conexiune pentru diverse fisiere
- Numele fisierului solicitat este primit ca parametru de catre programul client
- Requestul consta in trimitrea catre server dupa realizarea conexiunii a numelui fisierului solicitat
- Fisierul trimis de catre server este salvat de catre client 
