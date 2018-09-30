#include "jargumentanalyse.h"

static const QString preOption="-";

JArgumentAnalyse::JArgumentAnalyse(int argc, char *argv[])
{
    int i;
    for(i=0;i<argc;++i)
    {
        m_argv.push_back(argv[i]);
    }
    init();
    Analyse();
}

JArgumentAnalyse::JArgumentAnalyse(const QStringList& list)
{
    m_argv=list;
    init();
    Analyse();
}

void JArgumentAnalyse::init()
{
    m_address=QHostAddress::LocalHost;
    m_port=37373;
}

/*
-a :address
-p :port
-f :setting file
默认127.0.0.1:37373
*/

void JArgumentAnalyse::Analyse()
{
    while(!m_argv.isEmpty())
    {
        QString arg=m_argv.front();
        m_argv.pop_front();
        if(arg.startsWith(preOption))
        {
            arg.remove(0,preOption.length());
            switch(arg.at(0).toAscii())
            {
            case 'a':
                m_address=m_argv.front();
                m_argv.pop_front();
                break;
            case 'p':
                m_port=m_argv.front().toInt();
                m_argv.pop_front();
                break;
            case 'f':
                m_file=m_argv.front();
                m_argv.pop_front();
            }
        }
    }
}

const QHostAddress& JArgumentAnalyse::getAddress()const
{
    return m_address;
}

quint16 JArgumentAnalyse::getPort()const
{
    return m_port;
}
