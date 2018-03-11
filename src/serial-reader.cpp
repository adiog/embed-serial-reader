#include <chrono>
#include <iostream>
#include <memory>
#include <serial/serial.h>

#define INPUT_BUFFER_SIZE (256U)
uint8_t inputBuffer[INPUT_BUFFER_SIZE];
uint8_t inputBufferIndex = 0U;

std::chrono::milliseconds systemClockMillis;

std::unique_ptr<serial::Serial> Serial;

void setup(std::string port, uint32_t baudRate)
{
    Serial = std::make_unique<serial::Serial>(port, baudRate);
}

void dumpByte(uint8_t byte)
{
    static uint32_t lineIndex = 0U;

    if (lineIndex % 4U == 0U)
    {
        std::cout << "0x";
    }

    std::cout << std::hex << (int)byte << std::dec;

    if (lineIndex % 4U == 3U)
    {
        std::cout << ' ';
    }

    if (byte == '\n')
    {
        std::cout << std::endl;
        lineIndex = 0U;
    }
    else
    {
        lineIndex++;
    }
}

bool processByte(uint8_t incomingByte)
{
    inputBuffer[inputBufferIndex++] = incomingByte;

    if (inputBufferIndex == INPUT_BUFFER_SIZE)
    {
        inputBufferIndex = 0U;
    }

    return (incomingByte == '\n');
}

void processLine(std::string line)
{
    std::cout << line << std::endl;
}

bool loop()
{
    systemClockMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    uint8_t incomingByte;

    if (Serial->available() > 0)
    {
        Serial->read(&incomingByte, 1U);

        dumpByte(incomingByte);

        if (processByte(incomingByte))
        {
            std::cout << systemClockMillis.count() << '\n';
            processLine(std::string((char*)inputBuffer, inputBufferIndex));
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " baudRate port" << std::endl;
        return 0;
    }

    auto baudRate = static_cast<uint32_t>(atoi(argv[1]));
    std::string port = argv[2];

    setup(port, baudRate);

    while (loop())
    {
    }

    return 0;
}
