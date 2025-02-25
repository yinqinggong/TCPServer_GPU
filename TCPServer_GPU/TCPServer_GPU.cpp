#include <asio.hpp>
#include <iostream>
#include <string>
#include "intrinsic.h"
#include "chessboard_pose.h"
#include "solar_panel_pose.h"
#include "stereo3d.h"
#include "yolov8.h"

void get_image_from_cpu(asio::ip::tcp::socket& socket) {
    asio::io_context io_context;
    asio::ip::tcp::socket server_socket(io_context);
    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "12345");
    asio::connect(server_socket, endpoints);

    // 向 TCPServer_CPU 发送 capture_2d 请求
    std::string command = "capture_2d\n";
    asio::write(server_socket, asio::buffer(command));

    // 读取图像大小
    uint32_t size;
    asio::read(server_socket, asio::buffer(&size, sizeof(size)));

    // 读取图像数据
    std::vector<char> image_data(size);
    asio::read(server_socket, asio::buffer(image_data));

    // 读取结束标志
    asio::streambuf buf;
    asio::read_until(server_socket, buf, "\n");
    std::istream is(&buf);
    std::string end_signal;
    std::getline(is, end_signal);  // 获取 "\n" 标志

    std::cout << "Received image from TCPServer_CPU\n";
    // 发送图片大小
    asio::write(socket, asio::buffer(&size, sizeof(size)));
    // 将图像数据返回给客户端
    asio::write(socket, asio::buffer(image_data));

    // 发送结束标志
    asio::write(socket, asio::buffer("\n"));
}

void handle_client(asio::ip::tcp::socket& socket) {
    try {
        while (true) {
            // 读取客户端请求命令，直到遇到 "\n"
            asio::streambuf buf;
            asio::read_until(socket, buf, "\n");
            std::istream is(&buf);
            std::string command;
            std::getline(is, command);
            std::cout << "Received command: " << command << std::endl;
            command = command.substr(0, command.find("\n"));
            if (command == " intrinsics") {
                std::string response = "response-intrinsics";
                asio::write(socket, asio::buffer(response));
                // 发送结束标志
                asio::write(socket, asio::buffer("\n"));
            }
            else if (command == "capture_2d") {
                // 请求图像数据
                get_image_from_cpu(socket);
            }
            else if (command == "chessboard_pose") {
                std::string response = "reponse-solar_panel_pose";
                asio::write(socket, asio::buffer(response));
                // 发送结束标志
                asio::write(socket, asio::buffer("\n"));
            }
            else if (command == "solar_panel_pose") {
                std::string response = "reponse-solar_panel_pose";
                asio::write(socket, asio::buffer(response));
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
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    //1. 调用intrinsic.h中的load_intrinsic函数，加载相机的内参数，这些内参数在后面的接口中会被使用，应定义成全局变量。下面是示例代码：
    cv::Mat lcam_mat, lcam_dist, rcam_mat, rcam_dist, r_mat, t_vect;
    int ret_flag = load_intrinsic("intrinsics.xml", lcam_mat, lcam_dist, rcam_mat, rcam_dist, r_mat, t_vect);
    if (ret_flag == 0)
    {
        // 加载失败
    }

    //2.初始化实例分割模型，该模型会在后面的接口中使用，应定义成全局变量。下面是示例代码：
    //头文件：yolov8.h
    YOLO seg_model;
    int load_result = seg_model.Init("solar_panel_seg.engine");
    if (load_result == 0)
    {
        // 加载失败
    }

    //3.初始化双目3D重建模型，该模型会在后面的接口中被使用，应定义成全局变量。下面是示例代码：
    //头文件：stereo3d.h
    Stereo stereo_model;
    load_result = stereo_model.Initialize("stereo_3d.engine");
    if (load_result == 0)
    {
        // 加载失败
    }
    
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
