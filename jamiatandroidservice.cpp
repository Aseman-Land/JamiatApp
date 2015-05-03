#include "jamiatandroidservice.h"

class JamiatAndroidServicePrivate
{
public:
};

JamiatAndroidService::JamiatAndroidService(QObject *parent) :
    QObject(parent)
{
    p = new JamiatAndroidServicePrivate;
}

void JamiatAndroidService::start()
{

}

JamiatAndroidService::~JamiatAndroidService()
{
    delete p;
}
