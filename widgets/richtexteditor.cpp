/***************************************************************************
                         richtexteditor.cpp - Widget providing simple rich text editing 
                             -------------------
    copyright            : (C) 2002 by Marc Britton
    email                : consume@optusnet.com.au
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* KDE INCLUDES */
#include <klocale.h>

/* QT INCLUDES */
#include <qlayout.h>
#include <qevent.h>
#include <qtextedit.h>
#include <qframe.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qgroupbox.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qevent.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>

/* OTHER INCLUDES */
#include "specials.h"
#include "richtexteditor.h"

/* Pixmaps */
#include "pixmaps/textbold.xpm"
#include "pixmaps/textunder.xpm"
#include "pixmaps/textitalic.xpm"
#include "pixmaps/textcenter.xpm"
#include "pixmaps/textleft.xpm"
#include "pixmaps/textright.xpm"

RichTextEditor::RichTextEditor(QWidget *a_parent, const char *a_name)
	: QWidget(a_parent), KommanderWidget((QObject *)this)
{
        setObjectName(a_name);
	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	// setup toolbar
	m_toolbar = new QFrame(this);
	m_toolbar->setMinimumSize(0, 32);
	m_toolbar->setFrameShape(QFrame::NoFrame);
	m_toolbar->setFrameShadow(QFrame::Plain);

	//setup textedit
	m_textedit = new QTextEdit(this);
	m_textedit->setAcceptRichText(true);

	// layout the widgets
	QVBoxLayout *layout = new QVBoxLayout();
  setLayout(layout);
  layout->addWidget(m_toolbar);
	layout->addWidget(m_textedit);
 	layout->setSpacing(1);

	// setup buttons
	QHBoxLayout *tbLayout = new QHBoxLayout();
  m_toolbar->setLayout(tbLayout);
    
	//bold italic underline left right center link
	m_formatGroup = new QGroupBox(m_toolbar);
  m_formatGroup->setObjectName("formatGroup");
  tbLayout->addWidget(m_formatGroup);
	m_formatGroup->setFlat(true);
  m_alignGroup = new QGroupBox(m_toolbar);
  m_alignGroup->setObjectName("alignGroup");
  tbLayout->addWidget(m_alignGroup);
	m_alignGroup->setFlat(true);
	tbLayout->insertStretch(0);
	tbLayout->insertStretch(3);

  QHBoxLayout *hLayout = new QHBoxLayout;  
  m_formatGroup->setLayout(hLayout);
	m_buttonTextBold = new QToolButton(m_formatGroup);
  m_buttonTextBold->setObjectName("textBold");
	m_buttonTextBold->setIcon(QPixmap((const char **)bold_xpm));
	m_buttonTextBold->setCheckable(true);
	connect(m_buttonTextBold, SIGNAL(toggled(bool)), this, SLOT(textBold(bool)));
  hLayout->addWidget(m_buttonTextBold);
  
  
	m_buttonTextItalic = new QToolButton(m_formatGroup);
  m_buttonTextItalic->setObjectName("textItalic");
  m_buttonTextItalic->setIcon(QPixmap((const char **)italic_xpm));
  m_buttonTextItalic->setCheckable(true);
	connect(m_buttonTextItalic, SIGNAL(toggled(bool)), this, SLOT(textItalic(bool)));
  hLayout->addWidget(m_buttonTextItalic);
  
  m_buttonTextUnder = new QToolButton(m_formatGroup);
  m_buttonTextUnder->setObjectName("textUnder");
  m_buttonTextUnder->setIcon(QPixmap((const char **)under_xpm));
  m_buttonTextUnder->setCheckable(true);
	connect(m_buttonTextUnder, SIGNAL(toggled(bool)), this, SLOT(textUnder(bool)));
  hLayout->addWidget(m_buttonTextUnder);

  QButtonGroup *alignGroup = new QButtonGroup(this);
  hLayout = new QHBoxLayout;  
  m_alignGroup->setLayout(hLayout);
  m_buttonTextLeft = new QToolButton(m_alignGroup);  
  m_buttonTextLeft->setObjectName("textLeft");
  m_buttonTextLeft->setIcon(QPixmap((const char **)left_xpm));
  m_buttonTextLeft->setCheckable(true);
  connect(m_buttonTextLeft, SIGNAL(toggled(bool)), this, SLOT(textAlignLeft(bool)));
  hLayout->addWidget(m_buttonTextLeft);
  alignGroup->addButton(m_buttonTextLeft);
  
  m_buttonTextCenter = new QToolButton(m_alignGroup);
  m_buttonTextCenter->setObjectName("textCenter");
  m_buttonTextCenter->setIcon(QPixmap((const char **)center_xpm));
  m_buttonTextCenter->setCheckable(true);
  connect(m_buttonTextCenter, SIGNAL(toggled(bool)), this, SLOT(textAlignCenter(bool)));
  hLayout->addWidget(m_buttonTextCenter);
  alignGroup->addButton(m_buttonTextCenter);
  
  m_buttonTextRight = new QToolButton(m_alignGroup);
  m_buttonTextRight->setObjectName("textRight");
  m_buttonTextRight->setIcon(QPixmap((const char **)right_xpm));
  m_buttonTextRight->setCheckable(true);
  connect(m_buttonTextRight, SIGNAL(toggled(bool)), this, SLOT(textAlignRight(bool)));
  hLayout->addWidget(m_buttonTextRight);
  alignGroup->addButton(m_buttonTextRight);

	connect(m_textedit, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(fontChanged(const QFont &)));
  connect(m_textedit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
	
	connect(m_textedit, SIGNAL(textChanged()), this, SLOT(setTextChanged()));

}

QString RichTextEditor::currentState() const
{
	return QString("default");
}

RichTextEditor::~RichTextEditor()
{
}

bool RichTextEditor::isKommanderWidget() const
{
	return true;
}

QStringList RichTextEditor::associatedText() const
{
	return KommanderWidget::associatedText();
}

void RichTextEditor::setAssociatedText(const QStringList& a_at)
{
	KommanderWidget::setAssociatedText(a_at);
}

void RichTextEditor::setPopulationText(const QString& a_text)
{
    KommanderWidget::setPopulationText( a_text );
}

QString RichTextEditor::populationText() const
{
    return KommanderWidget::populationText();
}

void RichTextEditor::populate()
{
    QString txt = KommanderWidget::evalAssociatedText( populationText() );
    setWidgetText( txt );
}

void RichTextEditor::setWidgetText(const QString &a_text)
{
  m_textedit->setText(a_text);
  emit widgetTextChanged(a_text);
}

void RichTextEditor::setTextChanged()
{
  emit widgetTextChanged(m_textedit->toPlainText());
}

void RichTextEditor::textBold(bool a_isOn)
{
  m_textedit->setFontWeight(a_isOn ? QFont::Bold : QFont::Normal);
}

void RichTextEditor::textUnder(bool a_isOn)
{
  m_textedit->setFontUnderline(a_isOn);
}

void RichTextEditor::textItalic(bool a_isOn)
{
  m_textedit->setFontItalic(a_isOn);
}

void RichTextEditor::textAlignLeft(bool a_isOn)
{
  if (a_isOn)
  {
    m_textedit->setAlignment(Qt::AlignLeft);
    
  }
}

void RichTextEditor::textAlignCenter(bool)
{
  m_textedit->setAlignment(Qt::AlignCenter);
}

void RichTextEditor::textAlignRight(bool)
{
  m_textedit->setAlignment(Qt::AlignRight);
}

void RichTextEditor::fontChanged(const QFont &a_font)
{
  m_buttonTextBold->setChecked(a_font.bold());
  m_buttonTextItalic->setChecked(a_font.italic());
  m_buttonTextUnder->setChecked(a_font.underline());
}

void RichTextEditor::cursorPositionChanged()
{
  Qt::Alignment alignment = m_textedit->alignment(); 
  if (((alignment == Qt::AlignLeft) || (alignment & Qt::AlignLeft)) && !m_buttonTextLeft->isChecked()) 
  {
    m_buttonTextLeft->setChecked(true);
  }
  else 
  if ((alignment & Qt::AlignHCenter) && !m_buttonTextCenter->isChecked()) 
  {
    m_buttonTextCenter->setChecked(true);
  }
  else 
  if ((alignment & Qt::AlignRight) && !m_buttonTextCenter->isChecked()) 
  {
    m_buttonTextRight->setChecked(true);
  }
}

void RichTextEditor::showEvent( QShowEvent *e )
{
    QWidget::showEvent(e);
    emit widgetOpened();
}

void RichTextEditor::contextMenuEvent( QContextMenuEvent * e )
{
  e->accept();
  QPoint p = e->globalPos();
  emit contextMenuRequested(p.x(), p.y());
}

bool RichTextEditor::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText || f == DBUS::clear || f == DBUS::selection || f == DBUS::setEditable;
}

QString RichTextEditor::handleDBUS(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return m_textedit->toPlainText();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::selection:
      return m_textedit->textCursor().selectedText();
    case DBUS::clear:
      setWidgetText("");
      break;
    case DBUS::setEditable:
      m_textedit->setReadOnly(args[0] == "false" || args[0] == "0");
      break;
    default:
      return KommanderWidget::handleDBUS(function, args);
  }      
  return QString();
}

#include "richtexteditor.moc"
