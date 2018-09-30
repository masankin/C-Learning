#ifndef JARGUMENTANALYSE_H
#define JARGUMENTANALYSE_H

#include <QStringList>
#include <QHostAddress>

class JArgumentAnalyse
{
public:
    JArgumentAnalyse(int argc, char *argv[]);
    JArgumentAnalyse(const QStringList&);



    const QHostAddress& getAddress()const;
    quint16 getPort()const;
private:
    void init();
    void Analyse();

    QStringList m_argv;
    QHostAddress m_address;
    quint16 m_port;
    QString m_file;
};

#endif // JARGUMENTANALYSE_H
