//
// Created by Zhang Zhuyan on 14/3/23.
//

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include "property_generated.h"

using boost::asio::ip::tcp;

void handle_write(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        std::cout << "Sent Property successfully" <<bytes_transferred<<std::endl;
    } else {
        std::cerr << "Error: " << error.message() << std::endl;
    }
}


flatbuffers::FlatBufferBuilder buildnestedProperty()
{
    flatbuffers::FlatBufferBuilder builder;

    std::vector<flatbuffers::Offset<Property>> childPropertiesVector;

    auto name = builder.CreateString("TestName");
    auto value = builder.CreateString("TestValue");

    auto name_children = builder.CreateString("TestName_children");
    auto value_children = builder.CreateString("TestValue_children");





    PropertyBuilder propertyBuilderChild(builder);
    propertyBuilderChild.add_name(name_children);
    propertyBuilderChild.add_value(value_children);
    propertyBuilderChild.add_type(PropertyType_StringType);
    auto child_property = propertyBuilderChild.Finish();
    childPropertiesVector.push_back(child_property);
    auto propertiesChild=builder.CreateVector(childPropertiesVector);



//propertyBuilder.add_children(propertiesChild);

    PropertyBuilder propertyBuilder(builder);
    propertyBuilder.add_name(name);
    propertyBuilder.add_value(value);
    propertyBuilder.add_type(PropertyType_StringType);
    propertyBuilder.add_children(propertiesChild);

    auto property = propertyBuilder.Finish();

    builder.Finish(property);
    return builder;
}


void Task4()
{
        boost::asio::io_context io_context;

tcp::resolver resolver(io_context);
auto endpoints = resolver.resolve("127.0.0.1", "8080");
tcp::socket socket(io_context);
boost::asio::connect(socket, endpoints);

    flatbuffers::FlatBufferBuilder builder=buildnestedProperty();

uint32_t size = static_cast<uint32_t>(builder.GetSize());
std::vector<boost::asio::const_buffer> buffers;
buffers.push_back(boost::asio::buffer(&size, sizeof(uint32_t)));
buffers.push_back(boost::asio::buffer(builder.GetBufferPointer(), builder.GetSize()));

boost::asio::async_write(socket, buffers, handle_write);

io_context.run();

}

void send_property_tree(tcp::socket &socket, const flatbuffers::FlatBufferBuilder &fbb, boost::asio::steady_timer &timer);

void handle_write_frequent(const boost::system::error_code &error, size_t bytes_transferred, tcp::socket &socket, boost::asio::steady_timer &timer) {
    if (!error) {
        timer.expires_after(std::chrono::milliseconds(500)); // Set timer
        timer.async_wait([&](const boost::system::error_code &error) {
            if (!error) {
                flatbuffers::FlatBufferBuilder fbb=buildnestedProperty(); // construct property tree
                std::cout << "Sent Property successfully" << std::endl;

                send_property_tree(socket, fbb, timer);
            } else {
                std::cerr << "Timer error: " << error.message() << std::endl;
            }
        });
    } else {
        std::cerr << "Error: " << error.message() << std::endl;
    }
}

void send_property_tree(tcp::socket &socket, const flatbuffers::FlatBufferBuilder &fbb, boost::asio::steady_timer &timer) {
    std::vector<boost::asio::const_buffer> buffers;
    uint32_t size = static_cast<uint32_t>(fbb.GetSize());
    buffers.push_back(boost::asio::buffer(&size, sizeof(uint32_t)));
    buffers.push_back(boost::asio::buffer(fbb.GetBufferPointer(), fbb.GetSize()));
    boost::asio::async_write(socket, buffers, [&](const boost::system::error_code &error, std::size_t bytes_transferred) {
        handle_write_frequent(error, bytes_transferred, socket, timer);
    });
}
void Task5()
{
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "8080");
    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);
    boost::asio::steady_timer timer(io_context);
    flatbuffers::FlatBufferBuilder fbb=buildnestedProperty();// construct property tree
    send_property_tree(socket, fbb, timer);

    io_context.run();
}




int main() {
    Task5();
    return 0;
}
