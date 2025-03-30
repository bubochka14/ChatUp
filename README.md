# ChatUp
 ChatUp is a  messager available for Linux, Windows and Mac.
In ChatUp you can: 
* Сreate groups and add users to them
* Send text messages to group chats
* Make group video/audio calls<br>

Websockets are used to communicate with the server, and media traffic is transmitted using the [libdatachannel](https://github.com/paullouisageneau/libdatachannel) WebRTC library.
Encoding/decoding is performed via [FFmpeg](https://github.com/FFmpeg/FFmpeg).
  
## Dependencies
Qt version >= 6.8.0
- [FFmpeg](https://github.com/FFmpeg/FFmpeg) (vcpkg port)
- [OpenSSL](https://github.com/openssl/openssl) (vcpkg port)
- [nlohmann-json](https://github.com/nlohmann/json) (vcpkg port)
- [libdatachannel](https://github.com/bubochka14/libdatachannel) (submodule)
- [QWindowKit](https://github.com/stdware/qwindowkit) (submodule)
- [QuickFuture](https://github.com/bubochka14/quickfuture) (submodule)
## Building
The build will require vcpkg, I tested on MSVC 19.41.34123 and CMake 3.27.0
```sh
git clone https://github.com/bubochka14/ChatUp.git && cd ChatUp
git submodule update --init --recursive
mkdir build && cd build
cmake build .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg
```
## To do
* Сlient-server encryption
* Personal chats
* Sending files to the chat
* Screen sharing 
