#include <asio.hpp>
#include <iostream>
#include <fstream>
#include <vector>

void handle_client(asio::ip::tcp::socket& socket) {
    try {
        // 读取客户端请求命令，直到遇到 "END"
        asio::streambuf buf;
        asio::read_until(socket, buf, "END");
        std::istream is(&buf);
        std::string command;
        std::getline(is, command);
        std::cout << "Received command: " << command << std::endl;
        command = command.substr(0, command.find("END"));
        if (command == "GetImage") {
            // 读取图片 l.jpg
            std::ifstream file("D:\\Workspace\\TCPServer_GPU\\x64\\Debug\\l.jpg", std::ios::binary);
            if (!file) {
                std::cerr << "Failed to open image file." << std::endl;
                return;
            }

            std::vector<char> image_data((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
            uint32_t size = image_data.size();

            // 发送图片大小
            asio::write(socket, asio::buffer(&size, sizeof(size)));

            // 发送图片数据
            asio::write(socket, asio::buffer(image_data));

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
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        asio::io_context io_context;
        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 12345));

        std::cout << "TCPServer_CPU listening on port 12345\n";
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
