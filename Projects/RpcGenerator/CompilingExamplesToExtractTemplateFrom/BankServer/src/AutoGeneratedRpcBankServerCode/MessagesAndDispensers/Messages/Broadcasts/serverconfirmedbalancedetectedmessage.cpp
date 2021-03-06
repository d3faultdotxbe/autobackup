#include "serverconfirmedbalancedetectedmessage.h"

ServerConfirmedBalanceDetectedMessage::ServerConfirmedBalanceDetectedMessage(QObject *owner)
    : IMessage(owner)
{ }
void ServerConfirmedBalanceDetectedMessage::streamIn(QDataStream &in)
{
    CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(in,>>);
}
void ServerConfirmedBalanceDetectedMessage::streamOut(QDataStream &out)
{
    CONFIRMED_BALANCE_DETECTED_MESSAGE_PARSE_BROADCAST(out,<<);
}
void ServerConfirmedBalanceDetectedMessage::resetMessageParameters()
{
    resetMessageParametersBase(); //TODOreq: client needs to also call this in it's base-inherited class
}
