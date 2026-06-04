# Cuncurrent-server-client
Cuncurrent server-client is like server is able to servermultiple client at same time without any wait


**What is a Concurrent Server?**
A concurrent server is a server that can handle multiple clients at the same time. Instead of finishing one client and then moving to the next, it serves all clients simultaneously.

Iterative Server vs Concurrent Server
FeatureIterative ServerConcurrent ServerClients handledOne at a timeMultiple at same timeNext client waitsYes — blockedNo — served immediatelyComplexitySimpleModerateReal world useRareStandard
Simple example:

Iterative = One doctor, one patient at a time. Next patient waits outside.
Concurrent = One doctor + multiple assistants. Each assistant handles one patient.


**The Problem with Iterative Server**
cwhile(1) {
    accept();       // accepts client1
    recv();         // talking to client1... client2 is WAITING
    send();
    close();        // only now client2 gets accepted
}
If client1 takes 10 minutes → client2 waits 10 minutes. Not acceptable in real world.

3 Ways to Build a Concurrent Server in C
1. Using fork()

For every new client → create a child process
Child handles the client, parent goes back to accept()
Your implementation ✅

cwhile(1) {
    connfd = accept();
    pid = fork();
    if(pid == 0) {
        // child process
        close(listenfd);
        handle_client(connfd);
        exit(0);
    }
    // parent
    close(connfd);
}
Pros: Simple, memory isolated per client
Cons: Each process is heavy — too many clients = high RAM usage

2. Using Threads (pthread)

For every new client → create a thread
Thread handles the client, main thread goes back to accept()

cwhile(1) {
    connfd = accept();
    pthread_create(&tid, NULL, handle_client, &connfd);
}
Pros: Lightweight, shared memory
Cons: Need mutex/locks to avoid race conditions

3. Using select() — I/O Multiplexing

No new process, no new thread
Single process monitors all clients using select()
Acts only when any client sends data

cwhile(1) {
    select(maxfd+1, &readfds, NULL, NULL, NULL);
    // check which fd is ready and handle it
}
Pros: Most efficient, no extra process/thread overhead
Cons: Logic is more complex

Comparison Table
Featurefork()threadsselect()New process/threadYes — processYes — threadNoMemory usageHighMediumLowComplexityLowMediumHighCommunication between clientsHard (separate memory)Easy (shared memory)EasyCrash isolationGood (one crash = one process)Bad (one crash = whole server)MediumBest forSimple serversMulti-core systemsHigh performance servers

How fork() Based Concurrent Server Works
Client1 connects
    → Server_Parent accepts → fork() → Server_Child1 handles Client1
    → Server_Parent loops back to accept()

Client2 connects
    → Server_Parent accepts → fork() → Server_Child2 handles Client2
    → Server_Parent loops back to accept()

Client N connects
    → Server_Parent accepts → fork() → Server_ChildN handles ClientN
Golden rules:

Child must close listenfd (it doesn't need it)
Parent must close connfd (it doesn't use it)
Parent must call wait() to avoid zombie processes


What is a Zombie Process?
When a child process finishes but parent hasn't called wait() yet — the child becomes a zombie. It is dead but still occupies a slot in the process table.
Fix:
csignal(SIGCHLD, SIG_IGN);  // auto reap zombies
// OR
waitpid(-1, NULL, WNOHANG); // non-blocking wait

Key System Calls Used
CallPurposesocket()Create socket endpointbind()Attach socket to portlisten()Mark socket as passiveaccept()Accept incoming clientfork()Create child processsend() / recv()Exchange dataclose()Close file descriptorwait() / waitpid()Reap zombie childrenexit()Child exits after serving client

Real World Examples

Apache Web Server — uses fork/threads to handle multiple HTTP requests
SSH Server (sshd) — forks a child for every new SSH session
FTP Server — one child per client connection
Database servers — PostgreSQL uses one process per connection (fork based)


Connection to Your Resume
You have built 3 versions of concurrent server:
VersionStatusfork() based concurrent server✅ Built & documentedselect() based chat server (TCP/UDP)✅ Built & in resumeThread basedCan add next
This shows you understand all 3 approaches to concurrency in Linux — which is exactly what systems programming interviews test. Very strong portfolio. 💪
