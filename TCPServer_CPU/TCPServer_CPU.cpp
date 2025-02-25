// g++ TCPServer_CPU.cpp -Iasio/include -o tcp_cpu -std=c++11 -pthread
#include <asio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>  // for std::system
#include <cstdio>   // for popen and pclose
//#include <thread>

// 检查文件是否存在
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good(); // 如果文件存在且可打开，返回 true
}

// 删除文件
bool deleteFile(const std::string& filename) {
    if (std::remove(filename.c_str()) != 0) {
        std::cerr << "Failed to delete file: " << filename << std::endl;
        return false;
    }
    else {
        std::cout << "Deleted old file: " << filename << std::endl;
        return true;
    }
}

// 调用系统命令拍照
bool takePhoto(const std::string& outputFile) {
    // 构造命令
    std::string command = "libcamera-still -t 1000 -e png -o " + outputFile;

    // 调用系统命令
    int result = std::system(command.c_str());

    // 检查命令是否成功执行
    if (result == 0) {
        std::cout << "Photo taken successfully! Saved as " << outputFile << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to take photo. Command returned: " << result << std::endl;
        return false;
    }
}

void handle_client(asio::ip::tcp::socket& socket) {
    try {
        // 读取客户端请求命令，直到遇到 "\n"
        asio::streambuf buf;
        asio::read_until(socket, buf, "\n");
        std::istream is(&buf);
        std::string command;
        std::getline(is, command);
        std::cout << "Received command: " << command << std::endl;
        command = command.substr(0, command.find("\n"));
        if (command == "capture_2d") {
            
            //"libcamera-still -t 1000 -e png -o out.png"
            //std::string outputFile = "out.png";

            //// 检查并删除旧文件
            //if (fileExists(outputFile)) {
            //    if (!deleteFile(outputFile)) {
            //        std::cerr << "Failed to delete old photo. Exiting..." << std::endl;
            //        return ;
            //    }
            //}

            ////std::thread photo_thread(takePhoto, outputFile);
            ////photo_thread.join();
            //// 拍照
            //if (takePhoto(outputFile)) {
            //    std::cout << "Photo saved to: " << outputFile << std::endl;
            //}
            //else {
            //    std::cerr << "Failed to save photo." << std::endl;
            //}

            
            // 读取图片 l.jpg
            std::ifstream file("D:\\Workspace\\TCPServer_GPU\\x64\\Debug\\l.jpg", std::ios::binary);
            //std::ifstream file("out.png", std::ios::binary);
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
            asio::write(socket, asio::buffer("\n"));
        }
        else {
            std::string error = "Unknown command";
            asio::write(socket, asio::buffer(error));
            // 发送结束标志
            asio::write(socket, asio::buffer("\n"));
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
