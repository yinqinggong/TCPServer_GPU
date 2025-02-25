#include <asio.hpp>
#include <iostream>
#include <fstream>
#include <vector>

void send_request(asio::ip::tcp::socket& socket, const std::string& command) {
    // 在请求命令后添加结束标志
    std::string request = command + "\n";
    asio::write(socket, asio::buffer(request));
}

void receive_response(asio::ip::tcp::socket& socket) {
    try {
        // 读取响应，直到遇到 "\n"
        asio::streambuf buf;
        asio::read_until(socket, buf, "\n");

        std::istream is(&buf);
        std::string response;
        std::getline(is, response);
        std::cout << "Received response: " << response << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error receiving response: " << e.what() << std::endl;
    }
}

void receive_image(asio::ip::tcp::socket& socket) {
    try {
        uint32_t size;
        asio::read(socket, asio::buffer(&size, sizeof(size)));

        std::vector<char> image_data(size);
        asio::read(socket, asio::buffer(image_data));

        // 读取结束标志
        asio::streambuf buf;
        asio::read_until(socket, buf, "\n");
        std::istream is(&buf);
        std::string end_signal;
        std::getline(is, end_signal);  // 获取 "\n" 标志

        std::cout << "Image received, saving as received_image.jpg\n";

        // 保存图片到本地
        std::ofstream file("received_image.png", std::ios::binary);
        file.write(image_data.data(), image_data.size());
    }
    catch (const std::exception& e) {
        std::cerr << "Error receiving image: " << e.what() << std::endl;
    }
}

int main() {
    asio::io_context io_context;
    asio::ip::tcp::socket socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    //auto endpoints = resolver.resolve("127.0.0.1", "12345");
    auto endpoints = resolver.resolve("192.168.1.111", "12345");

    try {
        // 连接到 TCPServer_GPU
        //asio::connect(socket, endpoints);

        //// 第一次命令：intrinsics
        //send_request(socket, "intrinsics");
        //receive_response(socket);

        for (size_t i = 0; i < 100; i++)
        {
            //第二次命令：capture_2d
            asio::connect(socket, endpoints);
            send_request(socket, "capture_2d");
            receive_image(socket);
            socket.close();
            std::cout << "第 " << i + 1 << " 次请求" << std::endl;
            Sleep(1000);
        }
         

        //// 第三次命令：chessboard_pose
        //send_request(socket, "chessboard_pose");
        //receive_response(socket);

        //// 第四次命令：solar_panel_pose
        //send_request(socket, "solar_panel_pose");
        //receive_response(socket);

        // 保持连接打开，直到所有请求都完成
        std::cout << "All requests processed, closing connection." << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
