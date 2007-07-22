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
#include <q3textedit.h>
#include <q3frame.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qpixmap.h>
#include <qhbuttongroup.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qevent.h>
//Added by qt3to4:
#include <QShowEvent>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

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
	: QWidget(a_parent, a_name), KommanderWidget((QObject *)this)
{

	QStringList states;
	states << "default";
	setStates(states);
	setDisplayStates(states);

	// setup toolbar
	m_toolbar = new Q3Frame(this, "buttonBar");
	m_toolbar->setMinimumSize(0, 32);
	m_toolbar->setFrameShape(Q3Frame::NoFrame);
	m_toolbar->setFrameShadow(Q3Frame::Plain);

	//setup textedit
	m_textedit = new Q3TextEdit(this, "editor");
	m_textedit->setTextFormat(Qt::RichText);

	// layout the widgets
	Q3VBoxLayout *layout = new Q3VBoxLayout(this);
	layout->addWidget(m_toolbar);
	layout->addWidget(m_textedit);
	layout->setSpacing(1);

	// setup buttons
	Q3HBoxLayout *tbLayout = new Q3HBoxLayout(m_toolbar);

	//bold italic underline left right center link
	m_formatGroup = new Q3HButtonGroup(m_toolbar, "formatGroup");
	//m_formatGroup->setFlat(true);
	m_alignGroup = new Q3HButtonGroup(m_toolbar, "alignGroup");
	//m_alignGroup->setFlat(true);
	m_alignGroup->setExclusive(true);
	tbLayout->insertStretch(0);
	tbLayout->addWidget(m_formatGroup);
	tbLayout->addWidget(m_alignGroup);
	tbLayout->insertStretch(3);

	m_buttonTextBold = new QToolButton(m_formatGroup, "textBold");
	m_buttonTextBold->setPixmap(QPixmap((const char **)bold_xpm));
	m_buttonTextBold->setToggleButton(true);
	connect(m_buttonTextBold, SIGNAL(toggled(bool)), this, SLOT(textBold(bool)));
	m_buttonTextItalic = new QToolButton(m_formatGroup, "textItalic");
	m_buttonTextItalic->setPixmap(QPixmap((const char **)italic_xpm));
	m_buttonTextItalic->setToggleButton(true);
	connect(m_buttonTextItalic, SIGNAL(toggled(bool)), this, SLOT(textItalic(bool)));
	m_buttonTextUnder = new QToolButton(m_formatGroup, "textUnder");
	m_buttonTextUnder->setPixmap(QPixmap((const char **)under_xpm));
	m_buttonTextUnder->setToggleButton(true);
	connect(m_buttonTextUnder, SIGNAL(toggled(bool)), this, SLOT(textUnder(bool)));

	m_buttonTextLeft = new QToolButton(m_alignGroup, "textLeft");
	m_buttonTextLeft->setPixmap(QPixmap((const char **)left_xpm));
	m_buttonTextLeft->setToggleButton(true);
	m_buttonTextCenter = new QToolButton(m_alignGroup, "textCenter");
	m_buttonTextCenter->setPixmap(QPixmap((const char **)center_xpm));
	m_buttonTextCenter->setToggleButton(true);
	m_buttonTextRight = new QToolButton(m_alignGroup, "textRight");
	m_buttonTextRight->setPixmap(QPixmap((const char **)right_xpm));
	m_buttonTextRight->setToggleButton(true);

	connect(m_alignGroup, SIGNAL(clicked(int)), this, SLOT(textAlign(int)));
	connect(m_textedit, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(fontChanged(const QFont &)));
	connect(m_textedit, SIGNAL(currentAlignmentChanged(int)), this, SLOT(alignmentChanged(int)));
	
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
  emit widgetTextChanged(m_textedit->text());
}

void RichTextEditor::textBold(bool a_isOn)
{
  m_textedit->setBold(a_isOn);
}

void RichTextEditor::textUnder(bool a_isOn)
{
  m_textedit->setUnderline(a_isOn);
}

void RichTextEditor::textItalic(bool a_isOn)
{
  m_textedit->setItalic(a_isOn);
}

void RichTextEditor::textAlign(int a_id)
{
  QToolButton *b = (QToolButton *)m_alignGroup->find(a_id);
  if(b == m_buttonTextLeft)
    m_textedit->setAlignment(Qt::AlignLeft);
  else if(b == m_buttonTextCenter)
    m_textedit->setAlignment(Qt::AlignCenter);
  else if(b == m_buttonTextRight)
    m_textedit->setAlignment(Qt::AlignRight);
}

void RichTextEditor::fontChanged(const QFont &a_font)
{
  m_buttonTextBold->setOn(a_font.bold());
  m_buttonTextItalic->setOn(a_font.italic());
  m_buttonTextUnder->setOn(a_font.underline());
}

void RichTextEditor::alignmentChanged(int a_alignment)
{
  if((a_alignment == Qt::AlignLeft) || (a_alignment & Qt::AlignLeft)) 
    m_buttonTextLeft->setOn(true);
  else if(a_alignment & Qt::AlignHCenter) 
    m_buttonTextCenter->setOn(true);
  else if(a_alignment & Qt::AlignRight) 
    m_buttonTextRight->setOn(true);
}

void RichTextEditor::showEvent( QShowEvent *e )
{
    QWidget::showEvent(e);
    emit widgetOpened();
}

bool RichTextEditor::isFunctionSupported(int f)
{
  return f == DBUS::text || f == DBUS::setText || f == DBUS::clear || f == DBUS::selection;
}

QString RichTextEditor::handleDCOP(int function, const QStringList& args)
{
  switch (function) {
    case DBUS::text:
      return m_textedit->text();
    case DBUS::setText:
      setWidgetText(args[0]);
      break;
    case DBUS::selection:
      return m_textedit->selectedText();
    case DBUS::clear:
      setWidgetText("");
      break;
    default:
      return KommanderWidget::handleDCOP(function, args);
  }      
  return QString::null;
}

#include "richtexteditor.moc"
