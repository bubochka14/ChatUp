
add_executable (QtChatClient "main.cpp" "res.qrc" "chatclient.cpp" "chatclient.h")
target_link_libraries(QtChatClient PRIVATE Qt6::Core Qt6::Widgets Qt6::Qml Qt6::Quick Qt6::WebSockets Qt6::Network)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET QtChatClient PROPERTY CXX_STANDARD 20)
endif()
add_subdirectory(qml)
# TODO: Добавьте тесты и целевые объекты, если это необходимо.
