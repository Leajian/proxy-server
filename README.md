# proxy-server
A simple HTTP only proxy server based on Comer's CNAI socket API.

## Linux

### Compilation
In order to produce the executable file, in the **/make-linux** folder, run the command from terminal:
```
make
```
To clean the build, in the same folder, run:
```
make clean
```

### Usage
```
./proxyserver <port>
```
Where './' is the execution command and <port> the port number the proxy server is listening to.

To see the result, change your proxy settings only for http,
so that it listens to the **localhost**, at yhe port you chose.
Then visit a site that supports http and see the log.txt file for details.
To quit the server, press _Ctrl + C_ or _Ctrl + Z_. Do not forget to reset your the proxy settings!
