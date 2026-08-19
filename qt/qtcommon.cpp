/*****************************************************************************
 * Eliot
 * Copyright (C) 2010-2012 Olivier Teulière
 * Authors: Olivier Teulière <ipkiss @@ gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *****************************************************************************/

#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include <QIcon>
#include <QPixmap>
#include <QLabel>
#include <QStyle>
#include <QLayout>
#include <QSettings>

#include <iostream>

#include "qtcommon.h"
#include "debug.h"

using namespace std;


QString _q(const char* s)
{
    if (s == nullptr)
    {
        return {};
    }

    string str(s);

    // Check if the string is wrapped in gettext format: _("your_string_here")
    // Length must be > 5 to safely hold at least _("")
    if (str.starts_with("_(\"") && str.size() > 5 && str.back() == ')')
    {
        // Chop off the leading '_("' (3 chars) and trailing '")' (2 chars)
        str = str.substr(3, str.size() - 5);
    }

#if defined(__APPLE__) || defined(WIN32)
    // On MacOSX, we force the encoding to UTF-8, because we have trouble
    // detecting the current encoding properly (see call to
    // bind_textdomain_codeset() in main.cpp)
    return qfu(_(str.c_str()));
#else
    return QString::fromLocal8Bit(_(str.c_str()));
#endif
}


wstring wfq(const QString &q)
{
#ifdef QT_NO_STL
    wchar_t *array = new wchar_t[q.size()];
    int size = q.toWCharArray(array);
    wstring ws(array, size);
    delete[] array;
    return ws;
#else
    return q.toStdWString();
#endif
}


QString qfw(const wstring &wstr)
{
#ifdef QT_NO_STL
    return QString::fromWCharArray(wstr.c_str());
#else
    return QString::fromStdWString(wstr);
#endif
}

static void logFailedTest(const string &testName, bool & oFailure)
{
    cerr << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    cerr << "@@@@@@@ Test " + testName + " failed! @@@@@@@" << endl;
    cerr << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl << endl;

    oFailure = true;
}


void QtCommon::CheckConversions()
{
    string s = "abcdéöùßĿ";

    bool failure = false;

    // Check identities
    if (s != lfw(wfl(s)))
        logFailedTest("1", failure);
    if (s != lfu(ufl(s)))
        logFailedTest("2", failure);
    if (s != lfq(qfl(s)))
        logFailedTest("3", failure);

    wstring w = wfl(s);
    if (w != wfl(lfw(w)))
        logFailedTest("4", failure);
    if (w != wfu(ufw(w)))
        logFailedTest("5", failure);
    if (w != wfq(qfw(w)))
        logFailedTest("6", failure);

    QString q = qfl(s);
    if (q != qfl(lfq(q)))
        logFailedTest("7", failure);
    if (q != qfu(ufq(q)))
        logFailedTest("8", failure);
    if (q != qfw(wfq(q)))
        logFailedTest("9", failure);

    // Check some cycles
    if (s != lfu(ufw(wfl(s))))
        logFailedTest("10", failure);
    if (s != lfw(wfu(ufl(s))))
        logFailedTest("11", failure);
    if (s != lfq(qfw(wfl(s))))
        logFailedTest("12", failure);
    if (s != lfw(wfq(qfl(s))))
        logFailedTest("13", failure);
    if (s != lfu(ufw(wfq(qfl(s)))))
        logFailedTest("14", failure);
    if (s != lfq(qfw(wfu(ufl(s)))))
        logFailedTest("15", failure);
    if (s != lfu(ufq(qfw(wfl(s)))))
        logFailedTest("16", failure);
    if (s != lfw(wfq(qfu(ufl(s)))))
        logFailedTest("17", failure);

    ASSERT(!failure, "Some string conversions failed");
}


bool QtCommon::requestConfirmation(QString confoKey, QString iMsg,
                                   QString iQuestion, QWidget *iParent)
{
    const bool withCheckbox = confoKey != "";
    if (withCheckbox)
    {
        // Do nothing if the user doesn't want to see this confirmation anymore
        QSettings qs;
        bool confirm = qs.value(confoKey, true).toBool();
        if (!confirm)
            return true;
    }

    // Build the dialog
    QDialog dialog(iParent);
    dialog.setWindowTitle(_q("Eliot"));
    auto *layout = new QGridLayout;
    dialog.setLayout(layout);

    auto *iconLabel = new QLabel;
    layout->addWidget(iconLabel, 0, 0, 3, 1, Qt::AlignTop);
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QIcon icon = QApplication::style()->standardIcon(
            QStyle::SP_MessageBoxQuestion, nullptr, &dialog);
    int iconSize = QApplication::style()->pixelMetric(
            QStyle::PM_MessageBoxIconSize, nullptr, &dialog);
    iconLabel->setPixmap(icon.pixmap(iconSize, iconSize));

    auto *label = new QLabel;
    layout->addWidget(label, 0, 1, 1, 1);
    label->setText(iMsg);
    label->setWordWrap(true);

    auto *questionLabel = new QLabel;
    layout->addWidget(questionLabel, 1, 1, 1, 1);
    questionLabel->setText(iQuestion != "" ? iQuestion :
                    _q("Do you want to continue?"));
    questionLabel->setWordWrap(true);

    QCheckBox *checkBox = nullptr;
    if (withCheckbox)
    {
        // Add a checkbox to the dialog box
        checkBox = new QCheckBox(_q("Do not show this confirmation anymore"));
        layout->addWidget(checkBox, 2, 1, 1, 1);
        checkBox->setToolTip(_q("You can still display the confirmation in the future,\n"
                                "by changing the appropriate option in the preferences."));
    }

    auto *buttons =
        new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No);
    layout->addWidget(buttons, 3, 0, 1, 2);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    buttons->button(QDialogButtonBox::Yes)->setDefault(true);

    if (!withCheckbox)
        buttons->button(QDialogButtonBox::Yes)->setFocus();

    // Try to define a correct size
    dialog.setMinimumWidth(300);
    dialog.resize(layout->sizeHint());

    int res = dialog.exec();

    // Save the "no more confirmation" setting
    if (withCheckbox && checkBox->isChecked() && res == QDialog::Accepted)
    {
        QSettings qs;
        qs.setValue(confoKey, false);
    }

    return res == QDialog::Accepted;
}

