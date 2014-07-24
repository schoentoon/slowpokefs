Slowpokefs
==========

Simulates slow disk IO using FUSE. Generally useful for testing although the main purpose was to show that blocking IO (as provided by most libaries) really, really sucks.

As a simple comparision I mounted /tmp using slowpokefs. When using ls on /tmp I got the following results using time:
> real    0m0.058s<br />
> user    0m0.001s<br />
> sys     0m0.002s

Then I did ls on the same directory but this time mounted through slowpokefs, this gave me the following results:
> real    0m34.515s<br />
> user    0m0.001s<br />
> sys     0m0.005s

Now this may not be a big deal in a lot of cases, but it can be a big deal in your own programs. For example we have a single threaded web server, when the IO hangs like the server will just not accept new connections. And yes IO can be this slow without a program like this think damaged hard drives.

Notes
=====

In case you're writing to it with buffers larger than 4096 bytes fuse will by default split your write operation up in chunks of 4096 bytes. If you don't want this mount with -o direct_io
