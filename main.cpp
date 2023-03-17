#include <iostream>
#include "property_generated.h"
#include <fstream>
#include <boost/asio.hpp>
#include "flatbuffers/reflection.h"
#include "flatbuffers/util.h"
using boost::asio::ip::tcp;



void handle_write(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        std::cout << "Sent Property successfully" << std::endl;
    } else {
        std::cerr << "Error: " << error.message() << std::endl;
    }
}

void ReadProperty(const Property *property)
{
    // task 3
    std::cout << "Name: " << property->name()->str() << ", Type: ";
    switch (property->type()) {
        case PropertyType_StringType:
            std::cout << "String";
            break;
        case PropertyType_IntegerType:
            std::cout << "Integer";
            break;
        case PropertyType_DoubleType:
            std::cout << "Double";
            break;
        case PropertyType_BoolType:
            std::cout << "Bool";
            break;
        default:
            std::cout << "Unknown";
    }
    std::cout << ", Value: " << property->value()->str() << std::endl;
    if (property->children()) {
        for (const Property *child : *property->children()) {
            ReadProperty(child);
        }
    }
}
void Task3Server()
{
    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        tcp::socket socket(io_context);
        acceptor.accept(socket); // blocking  wait for connection

        uint32_t size;
        std::vector<uint8_t> buffer;

        auto handle_read_property = [&buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                const Property* property = flatbuffers::GetRoot<Property>(buffer.data());
                ReadProperty(property);
            } else {
                std::cerr << "Error: " << error.message() << std::endl;
            }
        };

        auto handle_read_size = [&socket, &size, &buffer, &handle_read_property](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                buffer.resize(size);
                boost::asio::async_read(socket, boost::asio::buffer(buffer.data(), size), handle_read_property);
            } else {
                std::cerr << "Error: " << error.message() << std::endl;
            }
        };

        boost::asio::async_read(socket, boost::asio::buffer(&size, sizeof(uint32_t)), handle_read_size);

        io_context.run();

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


void Task2()
{

    flatbuffers::FlatBufferBuilder builder(1024);
    auto name = builder.CreateString("name");
    auto crypto = builder.CreateString("crypto");

    auto p=CreateProperty(builder,name,crypto,PropertyType_StringType);
    builder.Finish(p);

    auto propertyTree = GetProperty(builder.GetBufferPointer());
    assert(propertyTree->name()->str() == "name");
    assert(propertyTree->value()->str() == "crypto");
    std::ofstream outfile("propertyTree.dat", std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
    outfile.close();

    std::ifstream infile("propertyTree.dat", std::ios::binary);

    infile.seekg(0, std::ios::end);
    size_t size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::unique_ptr<char[]> buffer(new char[size]);
    infile.read(buffer.get(), size);

    auto propertyTree2 = GetMutableProperty(buffer.get());
    std::cout << " property tree name: " << propertyTree2->name()->str() << std::endl;
    std::cout << "property tree value: " << propertyTree2->value()->str() << std::endl;

    // update the property value
    propertyTree2->mutable_value()->Mutate(0,'C');
    std::cout << " property tree name: " << propertyTree2->name()->str() << std::endl;
    std::cout << "property tree value: " << propertyTree2->value()->str() << std::endl;

}
void print_property_recursively(const flatbuffers::Table *property_table,
                                const reflection::Schema &schema
                                ) {

    auto root_table = schema.root_table();
    if (root_table==NULL) return ;
    auto fields = root_table->fields();

    if (fields) {
        auto name_field_ptr = fields->LookupByKey("name");
        auto &name_field = *name_field_ptr;
        auto name_string = flatbuffers::GetAnyFieldS(*property_table, name_field, &schema);

        auto type_field_ptr = fields->LookupByKey("type");
        auto &type_field = *type_field_ptr;
        auto type_enum= flatbuffers::GetAnyFieldS(*property_table, type_field, &schema);

        auto value_field_ptr = fields->LookupByKey("value");
        auto &value_field = *value_field_ptr;
        auto value= flatbuffers::GetAnyFieldS(*property_table, value_field, &schema);
        //std::string nameString = name->name()->c_str();
        std::cout<<"name is "<<name_string<<"type is"<<type_enum<<"value is "<<value<<std::endl;
        auto children_field_ptr = fields->LookupByKey("children");
        auto &children_field = *children_field_ptr;
        auto children = flatbuffers::GetFieldV<flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::Table>>>>(*property_table,children_field);
        if (children) {
            //auto children_vector = root_table->GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::Table>>>(*children_field_ptr);
            for (flatbuffers::uoffset_t i = 0; i < children->size(); ++i) {
                auto child_table = children->GetAs<flatbuffers::Table>(i);


                std::cout << "child" << "    " << std::endl;
                print_property_recursively(child_table, schema);
            }
        }


    }

}



void Task4()
{
    //https://jorenjoestar.github.io/post/flatbuffers_reflection_data_driven_rendering/
    // read the schema must be bianry form
    std::string schemafile;
    if (!flatbuffers::LoadFile("../property.bfbs", false, &schemafile)) {
        std::cerr << "Error: Unable to load schema file." << std::endl;
    }

    flatbuffers::Verifier verifier(reinterpret_cast<const uint8_t *>(schemafile.data()), schemafile.size());
    if (!reflection::VerifySchemaBuffer(verifier)) {
        std::cerr << "Error: Unable to verify binary schema file." << std::endl;
    }
    auto &schema = *reflection::GetSchema(schemafile.c_str());


    boost::asio::io_context io_context;

    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
    tcp::socket socket(io_context);
    acceptor.accept(socket); // blocking  wait for connection
    uint32_t size;
    std::vector<uint8_t> buffer;

    auto handle_read_property = [&buffer, &schema,&size](const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (!error) {
            //auto property_type = schema.objects()->LookupByKey("Property");
            const flatbuffers::Table * root = flatbuffers::GetAnyRoot(buffer.data());
            // flatbuffers::Verifier verifier(buffer.data(),buffer.size());
            if (!flatbuffers::Verify(schema,*schema.root_table(), buffer.data(), size)) {
                std::cerr << "Failed to verify FlatBuffer" << std::endl;
            }
            print_property_recursively(root,schema);

        } else {
            std::cerr << "Error: " << error.message() << std::endl;
        }
    };

    auto handle_read_size = [&socket, &size, &buffer, &handle_read_property](const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (!error) {
            buffer.resize(size);
            boost::asio::async_read(socket, boost::asio::buffer(buffer.data(), size), handle_read_property);
        } else {
            std::cerr << "Error: " << error.message() << std::endl;
        }
    };

    boost::asio::async_read(socket, boost::asio::buffer(&size, sizeof(uint32_t)), handle_read_size);

    io_context.run();

}


void print_hello_world(const boost::system::error_code &error, boost::asio::steady_timer &timer) {
    if (!error) {
        std::cout << "Hello world" << std::endl;

        timer.expires_after(std::chrono::milliseconds(500)); // Set timer to expire after 0.5 seconds
        timer.async_wait([&](const boost::system::error_code &error) {
            print_hello_world(error, timer);
        });
    } else {
        std::cerr << "Timer error: " << error.message() << std::endl;
    }
}



void handle_read_buffer(const boost::system::error_code& error, std::size_t bytes_transferred,tcp::socket &socket,std::vector<uint8_t>& buffer,const reflection::Schema &schema);
void handle_read_size(const boost::system::error_code& error, std::size_t bytes_transferred,tcp::socket &socket,uint32_t size_,const reflection::Schema &schema,std::vector<uint8_t>& buffer)
{


    if (!error) {

        buffer.resize(size_);
        //std::cout<<"size of the buffer is1 "<<size_;
        //std::cout<<"byte transfered is  "<<bytes_transferred<<std::flush;
        boost::asio::async_read(socket, boost::asio::buffer(buffer), [&](const boost::system::error_code &error, std::size_t bytes_transferred) {
            //std::cout<<"byte transfered is  "<<bytes_transferred<<std::flush;
            handle_read_buffer(error, bytes_transferred, socket, buffer, schema);});
    } else {
        std::cerr << "Error:  handle_read_size" << error.message() << std::endl;
    }
}


void handle_read_buffer(const boost::system::error_code& error, std::size_t bytes_transferred,tcp::socket &socket,std::vector<uint8_t>& buffer,const reflection::Schema &schema)
{
    if (!error) {
        //auto property_type = schema.objects()->LookupByKey("Property");
        // std::cout<<"byte transfered is  "<<bytes_transferred<<std::flush;
        //std::cout<<"size of the buffer is 3 "<<buffer.size()<<std::flush;
        if (!flatbuffers::Verify(schema,*schema.root_table(), buffer.data(), buffer.size())) {
            // std::cerr << "Failed to verify FlatBuffer" << std::endl;
            return ;
        }

        const flatbuffers::Table * root = flatbuffers::GetAnyRoot(buffer.data());
        print_property_recursively(root,schema);
        uint32_t size;
        //std::vector<uint8_t> buffernew;

        boost::asio::async_read(socket, boost::asio::buffer(&size, sizeof(size)), [&](const boost::system::error_code &error, std::size_t bytes_transferred) {
            handle_read_size(error, bytes_transferred, socket, size, schema,buffer);});


    } else {
        std::cerr << "Error: from handle_read_buffer " << error.message() << std::endl;
    }
}
void task5()
{
    std::string schemafile;
    if (!flatbuffers::LoadFile("../property.bfbs", false, &schemafile)) {
        std::cerr << "Error: Unable to load schema file." << std::endl;
    }

    flatbuffers::Verifier verifier(reinterpret_cast<const uint8_t *>(schemafile.data()), schemafile.size());
    if (!reflection::VerifySchemaBuffer(verifier)) {
        std::cerr << "Error: Unable to verify binary schema file." << std::endl;
    }
    auto &schema = *reflection::GetSchema(schemafile.c_str());
    boost::asio::io_context io_context;

    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
    tcp::socket socket(io_context);
    acceptor.accept(socket); // blocking  wait for connection
    uint32_t size;
    std::vector<uint8_t> buffer;


    boost::asio::async_read(socket, boost::asio::buffer(&size, sizeof(uint32_t)), [&](const boost::system::error_code &error, std::size_t bytes_transferred) {handle_read_size(error, bytes_transferred, socket, size, schema,buffer);});

    boost::asio::steady_timer timer(io_context, std::chrono::milliseconds(5000));
    timer.async_wait([&](const boost::system::error_code &error) {
        print_hello_world(error, timer);
    });

    io_context.run();
}


int main() {

    //Task3Server();
    Task4();
    //task5();
    // Task2();
    return 0;
}


