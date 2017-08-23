/*
 * Copyright 2013  Lukas Appelhans <l.appelhans@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Qt includes
#include <QBoxLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QtWidgets>
#include <QPushButton>
#include <QDebug>

// Own includes
#include "AkabeiQuestion.h"

AkabeiQuestion::AkabeiQuestion(AkabeiClient::TransactionQuestion* question, QWidget* parent)
  : QDialog(parent),
    m_question(question),
    m_buttonGroup(0)
{
    //setButtons(QDialog::None);
    
    QWidget * mainWidget = new QWidget(this);
    
    QVBoxLayout * layout = new QVBoxLayout(mainWidget);
    
    QHBoxLayout * buttonLayout = new QHBoxLayout();
    
    QLabel * quest = new QLabel(question->question(), mainWidget);
    layout->addWidget(quest);
    
    m_buttonGroup = new QButtonGroup(this);
    
    buttonLayout->addSpacerItem(new QSpacerItem(50, 30, QSizePolicy::Expanding, QSizePolicy::Minimum));
    
    foreach (const AkabeiClient::TransactionAnswer &answer, question->possibleAnswers()) {
        QPushButton * button = new QPushButton(mainWidget);
        button->setObjectName(answer.letter);
        button->setText(answer.message);
        if (question->suggestedAnswer() == answer) {
            button->setFocus();
        }
        buttonLayout->addWidget(button);
        m_buttonGroup->addButton(button);
    }
    connect(m_buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &AkabeiQuestion::buttonClicked);
    layout->addItem(buttonLayout);
    
    //setMainWidget(mainWidget);
}

void AkabeiQuestion::buttonClicked(QAbstractButton* button)
{
    m_answer = button->objectName();
    accept();
}

QString AkabeiQuestion::ask()
{
    QDialog::exec();
    return m_answer;
}
