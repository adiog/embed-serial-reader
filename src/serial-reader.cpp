#include <chrono>
#include <iostream>
#include <memory>
#include <serial/serial.h>

#define INPUT_BUFFER_SIZE (256U)
uint8_t inputBuffer[INPUT_BUFFER_SIZE];
uint8_t inputBufferIndex = 0U;

FILE * accXStream;
FILE * accYStream;
FILE * accZStream;
FILE * gyroXStream;
FILE * gyroYStream;
FILE * gyroZStream;
FILE * magXStream;
FILE * magYStream;
FILE * magZStream;

std::chrono::milliseconds systemClockMillis;

std::unique_ptr<serial::Serial> Serial;

void setup(std::string port, uint32_t baudRate)
{
    Serial = std::make_unique<serial::Serial>(port, baudRate);
    accXStream = fopen("/dev/shm/PRO/accX", "a");
    accYStream = fopen("/dev/shm/PRO/accY", "w");
    accZStream = fopen("/dev/shm/PRO/accZ", "w");
    gyroXStream = fopen("/dev/shm/PRO/gyroX", "w");
    gyroYStream = fopen("/dev/shm/PRO/gyroY", "w");
    gyroZStream = fopen("/dev/shm/PRO/gyroZ", "w");
    magXStream = fopen("/dev/shm/PRO/magX", "w");
    magYStream = fopen("/dev/shm/PRO/magY", "w");
    magZStream = fopen("/dev/shm/PRO/magZ", "w");
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
    float accX;
    float accY;
    float accZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float magX;
    float magY;
    float magZ;
    (void)sscanf(line.c_str(), "%f %f %f %f %f %f %f %f %f",
             &accX, &accY, &accZ,
             &gyroX, &gyroY, &gyroZ,
             &magX, &magY, &magZ
    );

    fprintf(accXStream, "%f\n", accX);
    fflush(accXStream);
    fprintf(accYStream, "%f\n", accY);
    fflush(accYStream);
    fprintf(accZStream, "%f\n", accZ);
    fflush(accZStream);

    fprintf(gyroXStream, "%f\n", gyroX);
    fflush(gyroXStream);
    fprintf(gyroYStream, "%f\n", gyroY);
    fflush(gyroYStream);
    fprintf(gyroZStream, "%f\n", gyroZ);
    fflush(gyroZStream);

    fprintf(magXStream, "%f\n", magX);
    fflush(magXStream);
    fprintf(magYStream, "%f\n", magY);
    fflush(magYStream);
    fprintf(magZStream, "%f\n", magZ);
    fflush(magZStream);
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
            inputBufferIndex = 0;
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
