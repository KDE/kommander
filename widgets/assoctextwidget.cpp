/* KDE INCLUDES */
#include <kapplication.h>
#include <dcopclient.h>
#include <kprocess.h>

/* QT INCLUDES */
#include <qvariant.h>
#include <qstringlist.h>
#include <qcstring.h>
#include <qdatastream.h>
#include <qvaluelist.h>
#include <qstring.h>
#include <qapplication.h>
#include <qobject.h>
#include <qobjectlist.h>

/* OTHER INCLUDES */
#include "myprocess.h"
#include "assoctextwidget.h"

AssocTextWidget::AssocTextWidget(QObject *a_thisObject)
{
	m_thisObject = a_thisObject;
}

AssocTextWidget::~AssocTextWidget()
{
}

void AssocTextWidget::setAssociatedText(QStringList a_associations)
{
	m_associatedText = a_associations;
	while(m_associatedText.count() < (states().count()))
		m_associatedText += QString::null; // sync states and associations
}

QStringList AssocTextWidget::associatedText() const
{
	return m_associatedText;
}

QStringList AssocTextWidget::states() const
{
	return m_states;
}

void AssocTextWidget::setStates(QStringList a_states)
{
	m_states = a_states;
}

QString AssocTextWidget::evalAssociatedText() const // expands and returns associated text as a string
{
	QString curState = currentState();
	int index = (states().findIndex(curState));
	if(index == -1)
	{
		fprintf(stderr, "AssocTextWidget::evalAssociatedText() - Invalid state");
		return QString::null;
	}

	QStringList at = m_associatedText;
	QString baseText = at[index];

	return evalAssociatedText(baseText);
}

QString AssocTextWidget::evalAssociatedText(const QString &a_text) const
{
	QString evalText;
	QString baseText = a_text;
	int pos = 0, baseTextLength = baseText.length();
	while(pos < baseTextLength) //expand identifiers
	{
		while(baseText[pos] != ESCCHAR && pos < baseTextLength)
			evalText += baseText[pos++];

		if(pos < baseTextLength)
		{
			QString identifier;

			for(++pos;pos < baseTextLength && baseText[pos].isLetterOrNumber();++pos)
				identifier += baseText[pos];

			if(identifier.isEmpty())
			{
				evalText += ESCCHAR;
				++pos;
			}
			else
			{
				if(identifier == SPECIAL_NAME)
					evalText += widgetText();
				else // see if this is an identifier
				{
					QObject *superParent = m_thisObject;
					while(superParent->parent() != 0)
					{
						superParent = superParent->parent();
						if(superParent->inherits("QDialog"))
							break;
					}

					QObject *childObj = superParent->child(identifier.latin1());
					AssocTextWidget *childTextObj = dynamic_cast<AssocTextWidget *>(childObj);
					if(childTextObj)
					{
						evalText += childTextObj->evalAssociatedText();
					}
					else if(identifier == "dcop" || identifier == "exec")
					{
						while(baseText[pos].isSpace() && pos < baseTextLength)
								++pos;

						if(baseText[pos++] != '(')
						{
							printError(QString(m_thisObject->name()), QString("Expected \'(\' after \'%1\'").arg(identifier));
							return QString::null;
						}

						int braceCount = 1;
						int startpos = pos;
						bool inQuotes = false;
						while(pos < baseTextLength && braceCount)
						{
							if((baseText[pos] == ')' || baseText[pos] == '('))
							{
								if(!inQuotes) // this brace counts
								{
									if(baseText[pos] == '(')
										++braceCount;
									else if(baseText[pos] == ')')
										--braceCount;
								}
							}
							else if(baseText[pos] == '\"')
							{
								if(pos <= 0 || baseText[pos-1] != '\\')
									inQuotes = !inQuotes;
							}
							++pos;
						}
						if(braceCount)
						{
							printError(QString(m_thisObject->name()), QString("Unmatched parenthesis after \'%1\'").arg(identifier));
							return QString::null;
						}
						if(inQuotes)
						{
							printError(QString(m_thisObject->name()), QString("Unmatched quotes in argument of \'%1\'").arg(identifier));
							return QString::null;
						}

						QString arg = evalAssociatedText(baseText.mid(startpos, pos-startpos-1));

						if(identifier == "dcop")
							evalText += dcopQuery(arg);
						else if(identifier == "exec")
							evalText += execCommand(arg);
					}
					else
					{
						int termIndex = baseText.find(identifier, pos);
						if(termIndex == -1)
						{
							printError(QString(m_thisObject->name()), QString("Unterminated text block \'%1\'").arg(identifier));
							return QString::null;
						}

						evalText += baseText.mid(pos, termIndex-pos-1);
						pos = termIndex + identifier.length();
					}

				}
			}
		}
	}
	return evalText;
}

QString AssocTextWidget::dcopQuery(QString a_query) const
{
	int pos = 0, argc = 0, queryLength = a_query.length();

	QCString appId, object, function, data;
	while(argc < 4 && pos < queryLength)
	{
		do
		{
			pos = a_query.find('\"', pos);
			if(pos == -1)
				break;
		} while(pos > 0 && a_query[pos-1] == '\\');
		if(pos == -1)
			break;

		int startpos = ++pos;
		do
		{
			pos = a_query.find('\"', pos);
			if(pos == -1)
				break;
		} while(pos > 0 && a_query[pos-1] == '\\');
		if(pos == -1)
			break;

		if(argc == 0)
		{
		    appId = a_query.mid(startpos, pos-startpos);
		}
		else if(argc == 1)
		{
		    object = a_query.mid(startpos, pos-startpos);
		}
		else if(argc == 2)
		{
		    function = a_query.mid(startpos, pos-startpos);
		}
		else if(argc == 3)
		{
	  	    data = a_query.mid(startpos, pos-startpos);
		}

		++pos;
		++argc;
	}
	if(argc != 4)
	{
		printError(QString(m_thisObject->name()), QString("Error in arguments to DCOP query; Unmatched quotes or too few arguments"));
		return QString::null;
	}

	QString evalText;
	QCString replyType;
	QByteArray byteData, byteReply;
	QDataStream byteDataStream(byteData, IO_ReadWrite);
	byteDataStream << data;

	DCOPClient *cl = KApplication::dcopClient();
	if(cl)
	{
	    if(!cl->call(appId, object, function, byteData, replyType, byteReply))
            {
		printError(m_thisObject->name(), QString("Tried to perform DCOP query, but failed!"));
		return QString::null;
    	    }

	    QDataStream byteReplyStream(byteReply, IO_ReadOnly);
	    if(replyType == "QString")
	    {
		QString text;
		byteReplyStream >> text;
		evalText += text;
	    }
	    else if(replyType == "void")
	    {
	    }
	    else if(replyType == "int")
	    {
		int i;
		byteReplyStream >> i;
		QVariant var(i);
		evalText += var.toString();
	    }
	    else
		fprintf(stderr, "Return type %s not yet implemented in handling..\n", replyType.data());
	}
	return evalText;
}

QString AssocTextWidget::execCommand(QString a_command) const
{
	MyProcess proc(this);

	a_command.stripWhiteSpace();
	if(a_command[0] != '\"' || a_command[a_command.length()-1] != '\"')
	{
	    printError(QString(m_thisObject->name()), "Missing quotation in exec argument");
	    return QString::null;
	}
	a_command = a_command.mid(1, a_command.length()-2);
	    
	QString text = proc.run(a_command);
	return text;
}

void AssocTextWidget::printError(QString a_className, QString a_error) const
{
	qWarning(QString("In widget %1:\n\t").arg(a_className) + a_error);
}
