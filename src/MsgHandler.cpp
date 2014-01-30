#include <MsgHandler.h>
#include <IpmiMessage.h>
#include <IpmiCommandDefines.h>

using namespace IpmiCommandDefines;

bool MsgHandler::isPing(const IpmiMessage& message)
{
    if(message.length() != PING_LENGTH)
    {
        return false;
    }
    int n = message.length();
    while (--n >= 0 && (n==9 || message[n] == PING_REQUEST[n]));
    return n != 0;
}

void MsgHandler::pong(const IpmiMessage& message, IpmiMessage& response)
{
    response.setMessage(PONG_RESPONSE, PONG_LENGTH);
    response[9] = message[9];
}

void MsgHandler::processRequest(const IpmiMessage& message,
                                IpmiMessage& response)
{
    unsigned char temp[9] =  {0x00, 0x01, 0x01, 0x1b, 0x01, 0x00, 0x00, 0x00, 0x00};
    message.serialize(temp, 9, response);
}
