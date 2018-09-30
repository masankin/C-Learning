#ifndef JSNAKESERVERDISCONNECTEDPROCESSOR_H
#define JSNAKESERVERDISCONNECTEDPROCESSOR_H

#include <Processor/JProcessor>

class JSnakeServerDisconnectedProcessor : public JProcessor
{
public:
	static JSnakeServerDisconnectedProcessor* instance();
	void process(JSocket* socket , const QByteArray& data);
	JType getProcessorType()const;
private:
	explicit JSnakeServerDisconnectedProcessor(QObject* parent=0);
};

#endif /* JSNAKESERVERDISCONNECTEDPROCESSOR_H */

