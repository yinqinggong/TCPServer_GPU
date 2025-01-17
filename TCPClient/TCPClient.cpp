// TCPClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <asio.hpp>
#include <iostream>
#include <string>

void send_command(asio::io_context& io_context, const std::string& command) {
    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::socket socket(io_context);

    // Resolve the server address
    auto endpoints = resolver.resolve("127.0.0.1", "12345");
    asio::connect(socket, endpoints);

    // Send the command
    asio::write(socket, asio::buffer(command + '\n'));

    // Read the response
    std::string response;
    asio::read_until(socket, asio::dynamic_buffer(response), '\n');
    std::cout << "Server response: " << response << std::endl;
}

int main() {
    try {
        asio::io_context io_context;
        send_command(io_context, "GetParam");
        send_command(io_context, "GetPhoto");
        send_command(io_context, "Infer");
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
