# ChatUp
 ChatUp is a desktop messager available for Linux, Windows and Mac.
In ChatUp you can: 
* Сreate groups and add users to them
* Send text messages to group chats
* Make group video/audio calls<br>

## Screenshots
![Alt text](/screenshots/call.png?raw=true "Optional Title")
![Alt text](/screenshots/chat.png?raw=true "Optional Title")
![Alt text](/screenshots/users.png?raw=true "Optional Title")

## Dependencies
Qt version >= 6.8.0
- [FFmpeg](https://github.com/FFmpeg/FFmpeg) for opus/h264 encoding/decoding (vcpkg port)
- [nlohmann-json](https://github.com/nlohmann/json) for commnunication with server (vcpkg port)
- [libdatachannel](https://github.com/bubochka14/libdatachannel) for P2P media transfering (submodule)
- [OpenSSL](https://github.com/openssl/openssl) libdatachannel dependency (vcpkg port)
- [QWindowKit](https://github.com/stdware/qwindowkit) for frameless widnow (submodule)
- [QuickFuture](https://github.com/bubochka14/quickfuture) (submodule)
## Server setup
Communication with the [server](https://github.com/bubochka14/ChatUp_server) is performed using the webscoket protocol,address and port of the server can be set via the command line:
```sh
$ ChatUp -h "host" -p "port"
```
Default address is wss://5.35.124.43:8000
## Building
The build requires vcpkg, I tested on MSVC 19.41.34123 and CMake 3.27.0
```sh
git clone https://github.com/bubochka14/ChatUp.git && cd ChatUp
git submodule update --init --recursive
mkdir build && cd build
cmake build .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg
```
## To do
* Group and user icons
* Personal chats
* Sending files to the chat
* Screen sharing 
