module;

#include <chrono>
#include <iostream>

export module ducktyping;

// Adapted from https://www.modernescpp.com/index.php/dynamic-and-static-polymorphism
export namespace DuckTyping
{
    template <typename T>   // (1)
    concept MessageServer = requires(T t) 
    {
        t.writeMessage();
    };

    auto start = std::chrono::steady_clock::now();

    void writeElapsedTime() 
    {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = now - start;

        std::cerr << diff.count() << " sec. elapsed: ";
    }

    struct MessageSeverity 
    {
        void writeMessage() const 
        {
            std::cerr << "unexpected" << '\n';
        }
    };

    struct MessageInformation 
    {
        void writeMessage() const 
        {
            std::cerr << "information" << '\n';
        }
    };

    struct MessageWarning 
    {
        void writeMessage() const 
        {
            std::cerr << "warning" << '\n';
        }
    };

    struct MessageFatal : MessageSeverity {};

    template <MessageServer T>   // (2)
    void writeMessage(T& messServer) 
    {
        writeElapsedTime();
        messServer.writeMessage();
    }

    int Run()
    {
        std::cout << '\n';

        MessageInformation messInfo;
        writeMessage(messInfo);

        MessageWarning messWarn;
        writeMessage(messWarn);

        MessageFatal messFatal;
        writeMessage(messFatal);

        std::cout << '\n';
        return 0;
    }
}
