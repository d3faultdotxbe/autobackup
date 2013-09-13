#include "easytreeparser.h"

#include "easytreehashitem.h"

QList<EasyTreeHashItem*> *EasyTreeParser::parseEasyTreeAndReturnAsNewList(QIODevice *easyTree)
{
    bool weOpened = false; //backwards compatibility as I realize during a later project utilizing this library that this is not a good place to do opening/closing
    if(!easyTree->isOpen())
    {
        easyTree->open(QIODevice::ReadOnly | QIODevice::Text);
        weOpened = true;
    }
    QTextStream textStream(easyTree);
    QList<EasyTreeHashItem*> *easyTreeHashList = new QList<EasyTreeHashItem*>();
    while(!textStream.atEnd())
    {
        QString oneLine = textStream.readLine();
        easyTreeHashList->append(EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(oneLine));
    }
    if(weOpened)
    {
        easyTree->close();
    }
    return easyTreeHashList;
}
