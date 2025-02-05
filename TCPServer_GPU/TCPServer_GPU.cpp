#include <asio.hpp>
#include <iostream>
#include <string>

void get_image_from_cpu(asio::ip::tcp::socket& socket) {
    asio::io_context io_context;
    asio::ip::tcp::socket server_socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "12345");
    asio::connect(server_socket, endpoints);

    // 向 TCPServer_CPU 发送 GetImage 请求
    std::string command = "GetImageEND";
    asio::write(server_socket, asio::buffer(command));

    // 读取图像大小
    uint32_t size;
    asio::read(server_socket, asio::buffer(&size, sizeof(size)));

    // 读取图像数据
    std::vector<char> image_data(size);
    asio::read(server_socket, asio::buffer(image_data));

    // 读取结束标志
    asio::streambuf buf;
    asio::read_until(server_socket, buf, "END");
    std::istream is(&buf);
    std::string end_signal;
    std::getline(is, end_signal);  // 获取 "END" 标志

    std::cout << "Received image from TCPServer_CPU\n";
    // 发送图片大小
    asio::write(socket, asio::buffer(&size, sizeof(size)));
    // 将图像数据返回给客户端
    asio::write(socket, asio::buffer(image_data));

    // 发送结束标志
    asio::write(socket, asio::buffer("END"));
}

void handle_client(asio::ip::tcp::socket& socket) {
    try {
        while (true) {
            // 读取客户端请求命令，直到遇到 "END"
            asio::streambuf buf;
            asio::read_until(socket, buf, "END");
            std::istream is(&buf);
            std::string command;
            std::getline(is, command);
            std::cout << "Received command: " << command << std::endl;
            command = command.substr(0, command.find("END"));
            if (command == "GetParam") {
                std::string response = "Hello";
                asio::write(socket, asio::buffer(response));
                // 发送结束标志
                asio::write(socket, asio::buffer("END"));
            }
            else if (command == "GetPhoto") {
                // 请求图像数据
                get_image_from_cpu(socket);
            }
            else if (command == "Infer") {
                std::string response = "World";
                asio::write(socket, asio::buffer(response));
                // 发送结束标志
                asio::write(socket, asio::buffer("END"));
            }
            else {
                std::string error = "Unknown command";
                asio::write(socket, asio::buffer(error));
                // 发送结束标志
                asio::write(socket, asio::buffer("END"));
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        asio::io_context io_context;
        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 12346));

        std::cout << "TCPServer_GPU listening on port 12346\n";
        while (true) {
            asio::ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            handle_client(socket);  // 处理客户端请求
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Server exception: " << e.what() << std::endl;
    }

    return 0;
}
