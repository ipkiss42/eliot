/*****************************************************************************
 * Eliot
 * Copyright (C) 2008-2012 Olivier Teulière
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

#include "config.h"

#include <QGuiApplication>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QFileDialog>
#include <QDockWidget>
#include <QCloseEvent>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QDesktopServices>
#include <QScreen>
#include <QSettings>
#include <QUrl>

#include "main_window.h"
#include "dic.h"
#include "dic_exception.h"
#include "encoding.h"
#include "header.h"
#include "game_factory.h"
#include "game.h"
#include "public_game.h"
#include "player.h"
#include "history.h"
#include "turn_data.h"
#include "move.h"
#include "debug.h"
#include "round.h"
#include "settings.h"

#include "game_signals.h"
#include "new_game.h"
#include "tables_dialog.h"
#include "prefs_dialog.h"
#include "bag_widget.h"
#include "board_widget.h"
#include "rack_widget.h"
#include "score_widget.h"
#include "player_widget.h"
#include "training_widget.h"
#include "topping_widget.h"
#include "arbitration_widget.h"
#include "history_widget.h"
#include "stats_widget.h"
#include "dic_tools_widget.h"
#include "fav_players.h"
#include "timer_widget.h"
#include "dic_wizard.h"
#include "aux_window.h"
#include "update_checker.h"
#include "qtcommon.h"


INIT_LOGGER(qt, MainWindow);

const char *MainWindow::m_windowName = "MainWindow";

MainWindow::MainWindow(QWidget *iParent)
    : QMainWindow(iParent), m_dic(NULL), m_game(NULL),
    m_gameSignals(new GameSignals),
    m_playersWidget(NULL), m_trainingWidget(NULL),
    m_toppingWidget(NULL),
    m_arbitrationWidget(NULL), m_scoresWidget(NULL),
    m_bagWindow(NULL), m_boardWindow(NULL),
    m_historyWindow(NULL), m_statsWindow(NULL), m_timerWindow(NULL),
    m_dicToolsWindow(NULL), m_dicNameLabel(NULL), m_timerModel(NULL)
{
#ifdef DEBUG
    // Check that the string conversion routines are not buggy
    QtCommon::CheckConversions();
#endif

    // Path to the auto-saved game
    m_autoSaveGame = Settings::GetConfigFileDir() + "autosave.xml";

    LOG_DEBUG("Creating main window");
    m_ui.setupUi(this);
    createMenu();
    readSettings();

    // Initialize the random numbers generator
    // Note: This must be done _after_ creating the QMenuBar object,
    // because on Gnome QMenuBar calls gconftool2, which for some reason
    // calls srand() internally...
    // This could be disabled using QApplication::setDesktopSettingsAware(),
    // but we would lose the desktop integration...
    unsigned int val = time(NULL);
    srand(val);

    // Make it easier to reproduce bugs
    LOG_DEBUG("Rand seed: " << val);

    QSettings qs;
    int timerTotal = qs.value(PrefsDialog::kINTF_TIMER_TOTAL_DURATION, 180).toInt();
    int timerAlert = qs.value(PrefsDialog::kINTF_TIMER_ALERT_DURATION, 30).toInt();
    m_timerModel = new TimerModel(timerTotal, timerAlert);
    QObject::connect(m_timerModel, SIGNAL(alert(int)),
                     this, SLOT(beep()));
    QObject::connect(m_timerModel, SIGNAL(expired()),
                     this, SLOT(beep()));
    linkRackChangesAndTimer();

    QObject::connect(m_gameSignals, SIGNAL(gameChangedNonConst(PublicGame*)),
                     this, SLOT(updateForGame(PublicGame*)));
    QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                     this, SLOT(refresh()));

    // Status bar
    statusBar()->addWidget(new QLabel, 1);
    // First widget, not added yet
    m_lettersLabel = new QLabel(statusBar());
    m_lettersLabel->setVisible(false);
    // Second widget, not added yet
    m_turnLabel = new QLabel(statusBar());
    m_turnLabel->setVisible(false);
    // Third widget
    m_dicNameLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_dicNameLabel);
    QObject::connect(this, SIGNAL(dicChanged(const Dictionary*)),
                     this, SLOT(updateStatusBar(const Dictionary*)));

    // Board
    BoardWidget *boardWidget = new BoardWidget(m_playModel);
    QObject::connect(m_gameSignals, SIGNAL(gameChanged(const PublicGame*)),
                     boardWidget, SLOT(setGame(const PublicGame*)));
    QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                     boardWidget, SLOT(refresh()));

    QHBoxLayout *hlayout = new QHBoxLayout;
    QSplitter *vSplitter = new QSplitter(Qt::Vertical);
#if 0
    QDockWidget *dock = new QDockWidget;
    dock->setWidget(boardWidget);
    boardWidget->setWindowTitle(_q("Board"));

    vSplitter->addWidget(dock);
#else
    vSplitter->addWidget(boardWidget);
#endif

    // Rack widget below the board (hidden until there is a game)
    m_rackWidget = new RackWidget;
    m_rackWidget->setPlayModel(&m_playModel);
    QObject::connect(m_rackWidget, SIGNAL(gameUpdated()),
                     m_gameSignals, SLOT(notifyGameUpdated()));
    QObject::connect(m_gameSignals, SIGNAL(gameChanged(const PublicGame*)),
                     m_rackWidget, SLOT(setGame(const PublicGame*)));
    QObject::connect(m_gameSignals, SIGNAL(currPlayerRackChanged(const PlayedRack&)),
                     m_rackWidget, SLOT(setRack(const PlayedRack&)));
    m_rackWidget->hide();
    vSplitter->addWidget(m_rackWidget);

    hlayout->addWidget(vSplitter);
    m_ui.groupBoxTest->setLayout(hlayout);

#if 1
    // History
    m_historyTabWidget = new HistoryTabWidget;
    QObject::connect(m_gameSignals, SIGNAL(gameChanged(const PublicGame*)),
                     m_historyTabWidget, SLOT(setGame(const PublicGame*)));
    QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                     m_historyTabWidget, SLOT(refresh()));
    QObject::connect(m_historyTabWidget, SIGNAL(requestDefinition(QString)),
                     this, SLOT(showDefinition(QString)));
    QHBoxLayout *hlayout2 = new QHBoxLayout;
    hlayout2->addWidget(m_historyTabWidget);
    m_ui.groupBoxHistory->setLayout(hlayout2);
#else
    m_ui.groupBoxHistory->hide();
#endif

    // Hide the players group box
    m_ui.groupBoxPlayers->hide();

    // Notify everyone that we have no game yet
    m_gameSignals->notifyGameChanged(NULL);

    // Load dictionary
    QString dicPath = qs.value(PrefsDialog::kINTF_DIC_PATH, "").toString();
    if (dicPath != "")
    {
        LOG_INFO("Using dictionary " << lfq(dicPath));
        try
        {
            m_dic = new Dictionary(lfq(dicPath));
        }
        catch (const std::exception &e)
        {
            displayErrorMsg(_q("Cannot load dictionary '%1' indicated in the "
                               "preferences.\nReason: %2").arg(dicPath).arg(e.what()));
            return;
        }
    }
    emit dicChanged(m_dic);

    // Check for updates
    UpdateChecker *checker = new UpdateChecker(this);
    QObject::connect(checker, SIGNAL(notifyInfo(QString)),
                     this, SLOT(displayInfoMsg(QString)));
    checker->checkForUpdate();
}


MainWindow::~MainWindow()
{
    delete m_bagWindow;
    delete m_boardWindow;
    delete m_historyWindow;
    delete m_statsWindow;
    delete m_timerWindow;
    delete m_dicToolsWindow;
    delete m_game;
    delete m_dic;
    delete m_timerModel;
    delete m_gameSignals;

    // Destroy singletons
    GameFactory::Destroy();
    Settings::Destroy();
}


void MainWindow::destroyCurrentGame()
{
    if (m_game == NULL)
        return;

    LOG_DEBUG("Destroying current game");

    // Some controls, like the board, can live when there is no game.
    // We only have to give them a NULL handler instead of the current one.
    m_gameSignals->notifyGameChanged(NULL);

    m_ui.groupBoxPlayers->hide();

    delete m_game;
    m_game = NULL;

    LOG_DEBUG("Game destroyed");
}


void MainWindow::refresh()
{
    if (m_game != NULL)
    {
        const Bag &bag = m_game->getBag();
        m_lettersLabel->setText(_q("Consonants: %1 | Vowels: %2 | Jokers: %3")
                                .arg(bag.getNbConsonants())
                                .arg(bag.getNbVowels())
                                .arg(bag.count(Tile::Joker())));
        unsigned currTurn = m_game->getCurrTurn();
        m_turnLabel->setText(_q("Turn %1/%2")
                             .arg(currTurn)
                             .arg(m_game->getNbTurns()));
        bool isFirstTurn = m_game->isFirstTurn();
        bool isLastTurn = m_game->isLastTurn();
        m_actionHistoryFirstTurn->setEnabled(!isFirstTurn);
        m_actionHistoryPrevTurn->setEnabled(!isFirstTurn);
        m_actionHistoryNextTurn->setEnabled(!isLastTurn);
        m_actionHistoryLastTurn->setEnabled(!isLastTurn);
        m_actionHistoryReplayTurn->setEnabled(!isLastTurn);
        if (m_game->isFinished())
            displayInfoMsg(_q("End of the game"));

        // Update the auto-saved game
        try
        {
            m_game->save(m_autoSaveGame);
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("Error during auto-save: " << e.what());
            displayErrorMsg(_q("Error during auto-save of the game: %1").arg(e.what()));
        }

#ifdef DEBUG
        m_game->printTurns();
#endif
    }
}


void MainWindow::beep()
{
    QSettings qs;
    if (!qs.value(PrefsDialog::kINTF_TIMER_BEEPS, true).toBool())
        return;

    LOG_DEBUG("Beep!")
    // FIXME: doesn't seem to be working on Ubuntu
    // (even after 'modprobe pcspkr' and when the 'beep' application works)
    QApplication::beep();
}


void MainWindow::linkArbitrationAnd7P1()
{
    if (m_arbitrationWidget == NULL || m_dicToolsWindow == NULL)
        return;

    // Disconnect the arbitration rack updates from the "Plus 1" tab of the
    // dictionary tools
    m_arbitrationWidget->disconnect(SIGNAL(rackUpdated(const QString&)));
    // Reconnect it only if needed
    QSettings qs;
    if (qs.value(PrefsDialog::kARBIT_LINK_7P1, false).toBool())
    {
        QObject::connect(m_arbitrationWidget,
                         SIGNAL(rackUpdated(const QString&)),
                         &m_dicToolsWindow->getWidget(),
                         SLOT(setPlus1Rack(const QString&)));
    }
}


void MainWindow::linkRackChangesAndTimer()
{
    if (m_timerModel == NULL)
        return;
    // Disable the timer auto-start mechanism
    m_gameSignals->disconnect(SIGNAL(gameRackChanged(const PlayedRack&)), m_timerModel);
    // Reconnect if needed
    QSettings qs;
    if (qs.value(PrefsDialog::kINTF_TIMER_AUTO_START, false).toBool())
    {
        QObject::connect(m_gameSignals, SIGNAL(gameRackChanged(const PlayedRack&)),
                         m_timerModel, SLOT(resetTimer()));
        QObject::connect(m_gameSignals, SIGNAL(gameRackChanged(const PlayedRack&)),
                         m_timerModel, SLOT(startTimer()));
    }
}


void MainWindow::prefsUpdated()
{
    LOG_DEBUG("Preferences updated");
    // Refresh one signal/slot connection
    linkArbitrationAnd7P1();
    // Idem
    linkRackChangesAndTimer();

    // Refresh the timer values
    QSettings qs;
    int timerTotal = qs.value(PrefsDialog::kINTF_TIMER_TOTAL_DURATION, 180).toInt();
    int timerAlert = qs.value(PrefsDialog::kINTF_TIMER_ALERT_DURATION, 30).toInt();
    m_timerModel->setTotalDuration(timerTotal);
    m_timerModel->setAlertDuration(timerAlert);

    // Probably useless in most cases (currently only used for
    // the History alignment)
    m_gameSignals->notifyGameUpdated();
}


void MainWindow::updateForGame(PublicGame *iGame)
{
    // Try to reduce flicker
    setUpdatesEnabled(false);

    if (iGame == NULL)
    {
        m_actionGameSaveAs->setEnabled(false);
        m_actionGamePrintPreview->setEnabled(false);
        m_actionGamePrint->setEnabled(false);
        m_actionHistoryFirstTurn->setEnabled(false);
        m_actionHistoryPrevTurn->setEnabled(false);
        m_actionHistoryNextTurn->setEnabled(false);
        m_actionHistoryLastTurn->setEnabled(false);
        m_actionHistoryReplayTurn->setEnabled(false);
        m_actionSettingsDefineTables->setEnabled(false);
        setWindowTitle(_q("No game") + " - Eliot");
        statusBar()->removeWidget(m_lettersLabel);
        statusBar()->removeWidget(m_turnLabel);

        m_playModel.disconnect(this);
        m_timerModel->resetTimer();

        // Destroy the players widget
        QtCommon::DestroyObject(m_playersWidget, m_gameSignals);
        m_playersWidget = NULL;

        // Destroy the training widget
        QtCommon::DestroyObject(m_trainingWidget, m_gameSignals);
        m_trainingWidget = NULL;

        // Destroy the topping widget
        QtCommon::DestroyObject(m_toppingWidget, m_gameSignals);
        m_toppingWidget = NULL;

        // Destroy the arbitration widget
        QtCommon::DestroyObject(m_arbitrationWidget, m_gameSignals);
        m_arbitrationWidget = NULL;

        // Destroy the scores widget
        QtCommon::DestroyObject(m_scoresWidget, m_gameSignals);
        m_scoresWidget = NULL;
    }
    else
    {
        m_actionGamePrintPreview->setEnabled(true);
        m_actionGamePrint->setEnabled(true);
        m_actionGameSaveAs->setEnabled(true);
        m_actionSettingsDefineTables->setEnabled(iGame->getMode() == PublicGame::kARBITRATION);
        m_rackWidget->show();
        statusBar()->addWidget(m_lettersLabel);
        m_lettersLabel->show();
        statusBar()->addWidget(m_turnLabel);
        m_turnLabel->show();

        // Handle played moves
        QObject::connect(&m_playModel, SIGNAL(movePlayed(const wstring&, const wstring&)),
                         this, SLOT(playWord(const wstring&, const wstring&)));

        if (iGame->getMode() == PublicGame::kTRAINING)
        {
            setWindowTitle(_q("Training mode") + " - Eliot");
            m_ui.groupBoxPlayers->setTitle(_q("Training"));

            // Training widget
            m_trainingWidget = new TrainingWidget(NULL, m_playModel, iGame);
            m_ui.groupBoxPlayers->layout()->addWidget(m_trainingWidget);
            QObject::connect(m_trainingWidget, SIGNAL(gameUpdated()),
                             m_gameSignals, SLOT(notifyGameUpdated()));
            QObject::connect(m_trainingWidget, SIGNAL(notifyInfo(QString)),
                             this, SLOT(displayInfoMsg(QString)));
            QObject::connect(m_trainingWidget, SIGNAL(notifyProblem(QString)),
                             this, SLOT(displayErrorMsg(QString)));
            QObject::connect(m_trainingWidget, SIGNAL(requestDefinition(QString)),
                             this, SLOT(showDefinition(QString)));
            QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                             m_trainingWidget, SLOT(refresh()));

            // Players score
            m_scoresWidget = new ScoreWidget(NULL, iGame);
            m_ui.groupBoxPlayers->layout()->addWidget(m_scoresWidget);
            QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                             m_scoresWidget, SLOT(refresh()));
        }
        else if (iGame->getMode() == PublicGame::kARBITRATION)
        {
            setWindowTitle(_q("Arbitration game") + " - Eliot");
            m_ui.groupBoxPlayers->setTitle(_q("Arbitration"));

            // Note: we specify the parent immediately, otherwise the
            // reparenting creates focus issues
            m_arbitrationWidget = new ArbitrationWidget(m_ui.groupBoxPlayers, iGame, m_playModel);
            m_ui.groupBoxPlayers->layout()->addWidget(m_arbitrationWidget);
            QObject::connect(m_arbitrationWidget, SIGNAL(gameUpdated()),
                             m_gameSignals, SLOT(notifyGameUpdated()));
            QObject::connect(m_arbitrationWidget, SIGNAL(notifyInfo(QString)),
                             this, SLOT(displayInfoMsg(QString)));
            QObject::connect(m_arbitrationWidget, SIGNAL(notifyProblem(QString)),
                             this, SLOT(displayErrorMsg(QString)));
            QObject::connect(m_arbitrationWidget, SIGNAL(requestDefinition(QString)),
                             this, SLOT(showDefinition(QString)));
            QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                             m_arbitrationWidget, SLOT(refresh()));
            // Connect with the dictionary tools only if needed
            linkArbitrationAnd7P1();

            // When a player is selected, show his history
            QObject::connect(m_arbitrationWidget, SIGNAL(playerSelected(unsigned)),
                             this, SLOT(onPlayerSelected(unsigned)));
        }
        else if (iGame->getMode() == PublicGame::kTOPPING)
        {
            setWindowTitle(_q("Topping mode") + " - Eliot");
            m_ui.groupBoxPlayers->setTitle(_q("Topping"));

            // Players widget
            m_toppingWidget = new ToppingWidget(NULL, m_playModel, *m_timerModel, iGame);
            m_ui.groupBoxPlayers->layout()->addWidget(m_toppingWidget);
            QObject::connect(m_toppingWidget, SIGNAL(gameUpdated()),
                             m_gameSignals, SLOT(notifyGameUpdated()));
            QObject::connect(m_toppingWidget, SIGNAL(notifyInfo(QString)),
                             this, SLOT(displayInfoMsg(QString)));
            QObject::connect(m_toppingWidget, SIGNAL(notifyProblem(QString)),
                             this, SLOT(displayErrorMsg(QString)));
            QObject::connect(m_gameSignals, SIGNAL(newTurn(int)),
                             m_toppingWidget, SLOT(onNewTurn()));
            QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                             m_toppingWidget, SLOT(refresh()));
        }
        else
        {
            if (iGame->getMode() == PublicGame::kDUPLICATE)
                setWindowTitle(_q("Duplicate game") + " - Eliot");
            else
                setWindowTitle(_q("Free game") + " - Eliot");
            m_ui.groupBoxPlayers->setTitle(_q("Players"));

            // Players widget
            m_playersWidget = new PlayerTabWidget(m_playModel, NULL);
            m_ui.groupBoxPlayers->layout()->addWidget(m_playersWidget);
            QObject::connect(m_playersWidget, SIGNAL(gameUpdated()),
                             m_gameSignals, SLOT(notifyGameUpdated()));
            QObject::connect(m_playersWidget, SIGNAL(notifyInfo(QString)),
                             this, SLOT(displayInfoMsg(QString)));
            QObject::connect(m_playersWidget, SIGNAL(notifyProblem(QString)),
                             this, SLOT(displayErrorMsg(QString)));
            QObject::connect(m_playersWidget, SIGNAL(requestDefinition(QString)),
                             this, SLOT(showDefinition(QString)));
            QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                             m_playersWidget, SLOT(refresh()));
            m_playersWidget->setGame(iGame);

            // Players score
            m_scoresWidget = new ScoreWidget(NULL, iGame);
            m_ui.groupBoxPlayers->layout()->addWidget(m_scoresWidget);
            QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                             m_scoresWidget, SLOT(refresh()));
        }
    }

    // Restore visual updates
    setUpdatesEnabled(true);
}


void MainWindow::updateStatusBar(const Dictionary *iDic)
{
    if (iDic == NULL)
        m_dicNameLabel->setText(_q("No dictionary"));
    else {
        QString dicName = qfw(m_dic->getHeader().getName());
        m_dicNameLabel->setText(_q("Dictionary: %1").arg(dicName));
        m_dicNameLabel->setToolTip("");
    }
}


void MainWindow::displayErrorMsg(QString iMsg, QString iContext)
{
    LOG_ERROR("Displayed error: " << lfq(iMsg));
    if (iContext == "")
        iContext = _q("Eliot - Error");

    QMessageBox::warning(this, iContext, iMsg);
}


void MainWindow::displayInfoMsg(QString iMsg)
{
    LOG_INFO("Displayed message: " << lfq(iMsg));
    statusBar()->showMessage(iMsg, 2000);
}


void MainWindow::showDefinition(QString iWord)
{
    QSettings qs;
    QString url = qs.value(PrefsDialog::kINTF_DEFINITIONS_SITE_URL,
                           PrefsDialog::kDEFAULT_DEF_SITE).toString();
    if (url == "")
    {
        displayErrorMsg(_q("No definitions site defined.\n"
                           "Please define one in the preferences."));
        return;
    }

    url = url.replace("%w", iWord.toLower());
    url = url.replace("%W", iWord.toUpper());
    bool res = QDesktopServices::openUrl(QUrl(url));
    if (!res)
    {
        LOG_ERROR("Could not open URL: " << lfq(url));
    }
}


void MainWindow::onPlayerSelected(unsigned iPlayerId)
{
    // Select the corresponding tab in the history widget
    // Note: we don't do the same for the external history widget...
    m_historyTabWidget->setCurrentIndex(1 + iPlayerId);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_game && !m_game->isFinished())
    {
        QString msg = _q("A game has been started.");
        QString question = _q("Do you really want to quit?");
        if (!QtCommon::requestConfirmation(PrefsDialog::kCONFO_QUIT_GAME,
                                           msg, question, this))
        {
            event->ignore();
            return;
        }
    }

    LOG_INFO("Exiting");

    // Make sure auxiliary windows don't survive after the main one
    if (m_bagWindow)
        m_bagWindow->close();
    if (m_boardWindow)
        m_boardWindow->close();
    if (m_historyWindow)
        m_historyWindow->close();
    if (m_statsWindow)
        m_statsWindow->close();
    if (m_timerWindow)
        m_timerWindow->close();
    if (m_dicToolsWindow)
        m_dicToolsWindow->close();
    writeSettings();
    event->accept();
}


void MainWindow::writeSettings() const
{
    QSettings settings;
    settings.beginGroup(m_windowName);
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    // Do not save this splitter when no game is present, because the players
    // group is hidden and the splitter is not in its normal place
    if (m_game)
        settings.setValue("splitterHoriz", m_ui.splitterHoriz->saveState());
    settings.setValue("splitterVert", m_ui.splitterVert->saveState());
    settings.endGroup();
}


void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup(m_windowName);
    QSize size = settings.value("size").toSize();
    if (size.isValid())
        resize(size);
    const QRect &desktopRect = qGuiApp->primaryScreen()->geometry();
    QPoint point = settings.value("pos", QPoint(20, 20)).toPoint();
    // If the position was saved when an external monitor was plugged, and
    // is restored when the monitor is not there anymore, the window could
    // be off screen...
    if (point.x() < 0 || point.x() > desktopRect.right())
        point.setX(20);
    if (point.y() < 0 || point.y() > desktopRect.bottom())
        point.setY(20);
    move(point);
    m_ui.splitterHoriz->restoreState(settings.value("splitterHoriz").toByteArray());
    m_ui.splitterVert->restoreState(settings.value("splitterVert").toByteArray());
    settings.endGroup();
}


void MainWindow::changeDictionary(QString iFileName)
{
    if (!iFileName.isEmpty())
    {
        if (m_game && !m_game->isFinished())
        {
            QString msg = _q("Loading a dictionary will stop the current game.");
            if (!QtCommon::requestConfirmation(PrefsDialog::kCONFO_LOAD_DIC,
                                               msg, "", this))
            {
                return;
            }
        }

        LOG_INFO("Loading new dictionary file: " << lfq(iFileName));

        destroyCurrentGame();

        try
        {
            Dictionary *dic = new Dictionary(lfq(iFileName));
            delete m_dic;
            m_dic = dic;
            emit dicChanged(m_dic);
            displayInfoMsg(_q("Loaded dictionary '%1'").arg(iFileName));

            // Save the location of the dictionary in the preferences
            QSettings qs;
            qs.setValue(PrefsDialog::kINTF_DIC_PATH, iFileName);
        }
        catch (const std::exception &e)
        {
            displayErrorMsg(e.what());
        }
    }
}


void MainWindow::playWord(const wstring &iWord, const wstring &iCoord)
{
    ASSERT(m_game != NULL, "No game in progress");

    // Do nothing in Topping mode: this is already handled
    // in the ToppingWidget class
    if (m_game->getMode() == PublicGame::kTOPPING)
        return;

    int res = m_game->play(iWord, iCoord);
    if (res == 0)
    {
        m_gameSignals->notifyGameUpdated();
    }
    else
    {
        // Try to be as explicit as possible concerning the error
        QString msg = _q("Cannot play '%1' at position '%2':\n%3")
            .arg(qfw(iWord)).arg(qfw(iCoord));
        switch (res)
        {
            case 1:
                msg = msg.arg(_q("Some letters are not valid for the current dictionary"));
                break;
            case 2:
                msg = msg.arg(_q("Invalid coordinates"));
                break;
            case 3:
                msg = msg.arg(_q("The word does not exist"));
                break;
            case 4:
                msg = msg.arg(_q("The rack doesn't contain the letters needed to play this word"));
                break;
            case 5:
                msg = msg.arg(_q("The word is part of a longer one"));
                break;
            case 6:
                msg = msg.arg(_q("The word tries to replace an existing letter"));
                break;
            case 7:
                msg = msg.arg(_q("An orthogonal word is not valid"));
                break;
            case 8:
                msg = msg.arg(_q("The word is already present on the board at these coordinates"));
                break;
            case 9:
                msg = msg.arg(_q("A word cannot be isolated (not connected to the placed words)"));
                break;
            case 10:
                msg = msg.arg(_q("The first word of the game must be horizontal"));
                break;
            case 11:
                msg = msg.arg(_q("The first word of the game must cover the H8 square"));
                break;
            case 12:
                msg = msg.arg(_q("The word is going out of the board"));
                break;
            case 13:
                msg = msg.arg(_q("The word contains too many letters from the rack"));
                break;
            default:
                msg = msg.arg(_q("Incorrect or misplaced word"));
        }
        displayErrorMsg(msg);
    }
}


QAction * MainWindow::addMenuAction(QMenu *menu, QString iText,
                                    const QKeySequence &iShortcut,
                                    QString iStatusTip, const char *iMember,
                                    bool iCheckable, QIcon icon)
{
    QAction *action = new QAction(iText, this);
    action->setShortcut(iShortcut);
    action->setStatusTip(iStatusTip);
    action->setCheckable(iCheckable);
    action->setIcon(icon);
    QObject::connect(action, SIGNAL(triggered()), this, iMember);
    menu->addAction(action);
    return action;
}


void MainWindow::createMenu()
{
    // Decide whether to show the toolbar
    QSettings qs;
    bool showToolBar = qs.value(PrefsDialog::kINTF_SHOW_TOOLBAR, true).toBool();
    m_ui.toolBar->setVisible(showToolBar);

    QMenu *menuFile = new QMenu(m_ui.menubar);
    m_ui.menubar->addAction(menuFile->menuAction());
    menuFile->setTitle(_q("&Game"));
    addMenuAction(menuFile, _q("&New..."), _q("Ctrl+N"),
                  _q("Start a new game"), SLOT(onGameNew()));
    menuFile->addSeparator();
    addMenuAction(menuFile, _q("&Load..."), _q("Ctrl+O"),
                  _q("Load an existing game"), SLOT(onGameLoad()));
    addMenuAction(menuFile, _q("Load the auto-saved game"), QString(""),
                  _q("Load the automatically saved game (useful after a crash)"),
                  SLOT(onGameLoadAutoSave()));
    m_actionGameSaveAs = addMenuAction(menuFile, _q("&Save as..."), _q("Ctrl+S"),
                  _q("Save the current game"), SLOT(onGameSaveAs()));
    menuFile->addSeparator();
    m_actionGamePrintPreview = addMenuAction(menuFile, _q("&Print preview..."), QString(""),
                  _q("Print preview"), SLOT(onGamePrintPreview()),
                  false, QIcon(":/images/print-preview.png"));
    m_actionGamePrint = addMenuAction(menuFile, _q("&Print..."), _q("Ctrl+P"),
                  _q("Print the current game"), SLOT(onGamePrint()),
                  false, QIcon(":/images/printer.png"));
    menuFile->addSeparator();
    addMenuAction(menuFile, _q("&Quit"), _q("Ctrl+Q"),
                  _q("Quit Eliot"), SLOT(onGameQuit()),
                  false, QIcon(":/images/quit_16px.png"));

    QMenu *menuHistory = new QMenu(m_ui.menubar);
    m_ui.menubar->addAction(menuHistory->menuAction());
    menuHistory->setTitle(_q("&History"));
    m_actionHistoryFirstTurn = addMenuAction(menuHistory, _q("&First turn"), _q("Ctrl+Home"),
                  _q("Go to the first turn of the game"), SLOT(onHistoryFirstTurn()),
                  false, QIcon(":/images/go-first.png"));
    m_actionHistoryPrevTurn = addMenuAction(menuHistory, _q("&Previous turn"), _q("Ctrl+Left"),
                  _q("Go to the previous turn of the game"), SLOT(onHistoryPrevTurn()),
                  false, QIcon(":/images/go-previous.png"));
    m_actionHistoryNextTurn = addMenuAction(menuHistory, _q("&Next turn"), _q("Ctrl+Right"),
                  _q("Go to the next turn of the game"), SLOT(onHistoryNextTurn()),
                  false, QIcon(":/images/go-next.png"));
    m_actionHistoryLastTurn = addMenuAction(menuHistory, _q("&Last turn"), _q("Ctrl+End"),
                  _q("Go to the last turn of the game"), SLOT(onHistoryLastTurn()),
                  false, QIcon(":/images/go-last.png"));
    m_actionHistoryReplayTurn = addMenuAction(menuHistory, _q("&Replay turn"), _q("Ctrl+R"),
                  _q("Play the game from the current position, "
                     "replacing what was really played"), SLOT(onHistoryReplayTurn()),
                  false, QIcon(":/images/go-jump.png"));
    // Add actions to the toolbar
    m_ui.toolBar->addAction(m_actionHistoryFirstTurn);
    m_ui.toolBar->addAction(m_actionHistoryPrevTurn);
    m_ui.toolBar->addAction(m_actionHistoryNextTurn);
    m_ui.toolBar->addAction(m_actionHistoryLastTurn);
    m_ui.toolBar->addAction(m_actionHistoryReplayTurn);

    QMenu *menuSettings = new QMenu(m_ui.menubar);
    m_ui.menubar->addAction(menuSettings->menuAction());
    menuSettings->setTitle(_q("&Settings"));
    addMenuAction(menuSettings, _q("&Choose dictionary..."), _q("Ctrl+I"),
                  _q("Select a new dictionary"), SLOT(onSettingsChooseDic()));
    addMenuAction(menuSettings, _q("Create &new dictionary..."), QString(""),
                  _q("Start the wizard for creating a new dictionary "
                     "from an existing word list"), SLOT(onSettingsCreateDic()));
    menuSettings->addSeparator();
    addMenuAction(menuSettings, _q("&Favorite players..."), QString(""),
                  _q("Define frequently used players for faster game creation"),
                  SLOT(onSettingsFavPlayers()));
    m_actionSettingsDefineTables =
        addMenuAction(menuSettings, _q("&Define players tables..."), QString(""),
                  _q("Define the tables where the players are sitting, in arbitration mode"),
                  SLOT(onSettingsDefineTables()));
    addMenuAction(menuSettings, _q("&Preferences..."), _q("Ctrl+F"),
                  _q("Edit the preferences"), SLOT(onSettingsPreferences()),
                  false, QIcon(":/images/preferences.png"));

    QMenu *menuWindows = new QMenu(m_ui.menubar);
    m_ui.menubar->addAction(menuWindows->menuAction());
    menuWindows->setTitle(_q("&Windows"));
    m_actionWindowsToolbar = addMenuAction(menuWindows, _q("&Toolbar"), _q("Ctrl+T"),
                  _q("Show/hide the toolbar"), SLOT(onWindowsToolbar()), true);
    m_actionWindowsToolbar->setChecked(showToolBar);
    m_actionWindowsBag = addMenuAction(menuWindows, _q("&Bag"), _q("Ctrl+B"),
                  _q("Show/hide the remaining tiles in the bag"), SLOT(onWindowsBag()), true);
    m_actionWindowsBoard = addMenuAction(menuWindows, _q("&External board"), _q("Ctrl+E"),
                  _q("Show/hide the external board"), SLOT(onWindowsBoard()), true);
    m_actionWindowsHistory = addMenuAction(menuWindows, _q("&History"), _q("Ctrl+H"),
                  _q("Show/hide the game history"), SLOT(onWindowsHistory()),
                  true, QIcon(":/images/playlist_16px.png"));
    m_actionWindowsStats = addMenuAction(menuWindows, _q("&Statistics"), QString(""),
                  _q("Show/hide the statistics"), SLOT(onWindowsStatistics()), true);
    m_actionWindowsTimer = addMenuAction(menuWindows, _q("Ti&mer"), QString(""),
                  _q("Show/hide the timer"), SLOT(onWindowsTimer()), true);
    m_actionWindowsDicTools = addMenuAction(menuWindows, _q("&Dictionary tools"), _q("Ctrl+D"),
                  _q("Show/hide the dictionary tools"), SLOT(onWindowsDicTools()), true);

    QMenu *menuHelp = new QMenu(m_ui.menubar);
    m_ui.menubar->addAction(menuHelp->menuAction());
    menuHelp->setTitle(_q("He&lp"));
    addMenuAction(menuHelp, _q("&About..."), QString(""),
                  _q("About Eliot"), SLOT(onHelpAbout()),
                  false, QIcon(":/images/info_16px.png"));
}


void MainWindow::onGameNew()
{
    LOG_DEBUG("Starting a new game (unconfirmed)");

    if (m_dic == NULL)
    {
        displayErrorMsg(_q("You have to select a dictionary (.dawg file) "
                           "before starting a game. This can be done in the "
                           "\"Settings\" menu."
                           "\n\nYou can download dictionary files on Eliot web site."));
        return;
    }

    if (m_game && !m_game->isFinished())
    {
        QString msg = _q("Starting a new game will stop the current one.");
        if (!QtCommon::requestConfirmation(PrefsDialog::kCONFO_START_GAME,
                                           msg, "", this))
        {
            return;
        }
    }

    NewGame newGameDialog(*m_dic);
    QObject::connect(&newGameDialog, SIGNAL(notifyProblem(QString)),
                     this, SLOT(displayErrorMsg(QString)));
    int res = newGameDialog.exec();
    if (res == QDialog::Rejected)
        return;

    // Destroy the game and the associated controls
    destroyCurrentGame();

    // Create a new game
    m_game = newGameDialog.createGame();
    if (m_game == NULL)
        return;

    m_ui.groupBoxPlayers->show();

    displayInfoMsg(_q("Game started"));
    m_game->start();

    m_gameSignals->notifyGameChanged(m_game);
}


void MainWindow::onGameLoad()
{
    loadGame("");
}


void MainWindow::onGameLoadAutoSave()
{
    loadGame(qfl(m_autoSaveGame));
}


void MainWindow::loadGame(QString fileName)
{
    if (m_dic == NULL)
    {
        displayErrorMsg(_q("You have to select a dictionary first!"));
        return;
    }

    if (m_game && !m_game->isFinished())
    {
        QString msg = _q("Loading a saved game will stop the current game.");
        if (!QtCommon::requestConfirmation(PrefsDialog::kCONFO_LOAD_GAME,
                                           msg, "", this))
        {
            return;
        }
    }

    if (fileName == "")
        fileName = QFileDialog::getOpenFileName(this, _q("Load a game"));
    if (fileName != "")
    {
        try
        {
            PublicGame *tmpGame = PublicGame::load(lfq(fileName), *m_dic);
            destroyCurrentGame();
            m_game = tmpGame;
        }
        catch (const std::exception &e)
        {
            displayErrorMsg(_q("Error while loading the game:\n") + e.what());
            return;
        }
        m_ui.groupBoxPlayers->show();

        m_gameSignals->notifyGameChanged(m_game);

        displayInfoMsg(_q("Game loaded"));
    }
}


void MainWindow::onGameSaveAs()
{
    if (m_game == NULL)
        return;

    QString fileName = QFileDialog::getSaveFileName(this, _q("Save a game"));
    if (fileName != "")
    {
        try
        {
            m_game->save(lfq(fileName));
            displayInfoMsg(_q("Game saved"));
        }
        catch (const std::exception &e)
        {
            displayErrorMsg(_q("Error saving game: %1").arg(e.what()));
        }
    }
}


void MainWindow::onGamePrint()
{
    LOG_DEBUG("Printing game (unconfirmed)");
    if (m_game == NULL)
        return;

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() != QDialog::Accepted)
        return;

    LOG_INFO("Printing game");
    print(&printer);
}


void MainWindow::onGamePrintPreview()
{
    LOG_INFO("Print preview");
    QPrintPreviewDialog previewDialog;
    QObject::connect(&previewDialog, SIGNAL(paintRequested(QPrinter *)),
                     this, SLOT(print(QPrinter*)));
    previewDialog.exec();
}


void MainWindow::print(QPrinter *printer)
{
    ASSERT(m_game != NULL, "No game in progress");

    QPainter painter(printer);
    const History &history = m_game->getHistory();

    // Printing parameters (XXX: these could be configurable by the users)
    // Number of pixels virtually present on the page width. The bigger
    // this number, the smaller the print result
    static const int TOTAL_WIDTH = 700;
    // Distance between 2 horizontal lines
    static const int LINE_HEIGHT = 16;
    // Font size, in pixels
    static const int FONT_SIZE = 10;
    // Width of the pen used to draw the grid lines
    static const int PEN_WIDTH = 1;
    // Offset of the text from the previous vertical line, in pixels
    static const int TEXT_OFFSET = 10;
    // Indicate whether the rack and the solution should be aligned
    static const bool SHOULD_ALIGN = false;
    // Columns widths
    static const int colWidths[] = { 30, 120, 120, 35, 35 };
    // Columns titles
    static const char *colTitles[] = { _("N."), _("RACK"), _("SOLUTION"), _("REF"), _("PTS") };

    static const unsigned int nbCols = sizeof(colWidths) / sizeof(int);
    const unsigned int nbRows = history.getSize() + (SHOULD_ALIGN ? 1 : 2);

    double scale = printer->pageRect().width() / double(TOTAL_WIDTH);
    painter.scale(scale, scale);

    QPen pen(painter.pen());
    pen.setWidth(PEN_WIDTH);
    painter.setPen(pen);

    QFont font;
    font.setPixelSize(FONT_SIZE);
    painter.setFont(font);

    int maxRight = 0;
    for (unsigned int i = 0; i < nbCols; ++i)
        maxRight += colWidths[i];
    int maxBottom = LINE_HEIGHT * (nbRows + 1);

    // Draw the horizontal lines
    for (unsigned int i = 0; i <= nbRows + 1; ++i)
        painter.drawLine(0, LINE_HEIGHT * i, maxRight, LINE_HEIGHT * i);

    // Draw the vertical lines
    painter.drawLine(0, 0, 0, maxBottom);
    int curWidth = 0;
    for (unsigned int i = 0; i < nbCols; ++i)
    {
        curWidth += colWidths[i];
        painter.drawLine(curWidth, 0, curWidth, maxBottom);
    }

    // Draw the titles
    QFontMetrics fm = painter.fontMetrics();
    int textHeight = fm.boundingRect('A').height();
    curWidth = 0;
    int curHeight = (LINE_HEIGHT + textHeight + 1) / 2;
    for (unsigned int i = 0; i < nbCols; ++i)
    {
        int textWidth = fm.horizontalAdvance(colTitles[i]);
        painter.drawText(curWidth + (colWidths[i] - textWidth) / 2,
                         curHeight,  colTitles[i]);
        curWidth += colWidths[i];
    }

    // Draw the history of the game
    int score = 0;
    int nextHeight;
    if (SHOULD_ALIGN)
        nextHeight = curHeight;
    else
        nextHeight = curHeight + LINE_HEIGHT;
    for (unsigned int i = 0; i < history.getSize(); ++i)
    {
        const TurnData &t = history.getTurn(i);
        const Move &m = t.getMove();

        curWidth = TEXT_OFFSET;
        curHeight += LINE_HEIGHT;
        nextHeight += LINE_HEIGHT;

        // Turn number
        painter.drawText(curWidth, curHeight, QString("%1").arg(i + 1));
        curWidth += colWidths[0];

        // Rack
        painter.drawText(curWidth, curHeight,
                         qfw(t.getPlayedRack().toString()));
        curWidth += colWidths[1];

        // Word and coordinates
        if (m.isValid())
        {
            const Round &r = m.getRound();
            painter.drawText(curWidth, nextHeight, qfw(r.getWord()));
            curWidth += colWidths[2];
            painter.drawText(curWidth, nextHeight,
                             qfw(r.getCoord().toString()));
            curWidth += colWidths[3];
        }
        else if (m.isInvalid())
        {
            painter.drawText(curWidth, nextHeight,
                             "<" + qfw(m.getBadWord()) + ">");
            curWidth += colWidths[2];
            painter.drawText(curWidth, nextHeight, qfw(m.getBadCoord()));
            curWidth += colWidths[3];
        }
        else if (m.isNull())
        {
            painter.drawText(curWidth, nextHeight, _q("(NO MOVE)"));
            curWidth += colWidths[2];
            curWidth += colWidths[3];
        }
        else if (m.isPass())
        {
            painter.drawText(curWidth, nextHeight, _q("(PASS)"));
            curWidth += colWidths[2];
            curWidth += colWidths[3];
        }
        else
        {
            painter.drawText(curWidth, nextHeight,
                             "[-" + qfw(m.getChangedLetters()) + "]");
            curWidth += colWidths[2];
            curWidth += colWidths[3];
        }

        // Score
        painter.drawText(curWidth, nextHeight,
                         QString("%1").arg(m.getScore()));
        score += m.getScore();
    }

    // Total score
    nextHeight += LINE_HEIGHT;
    painter.drawText(curWidth, nextHeight, QString("%1").arg(score));

    LOG_INFO("Game printed");
}


void MainWindow::onGameQuit()
{
    close();
}


void MainWindow::onSettingsPreferences()
{
    PrefsDialog *prefsDialog = new PrefsDialog(this);
    QObject::connect(prefsDialog, SIGNAL(prefsUpdated()),
                     this, SLOT(prefsUpdated()));
    prefsDialog->exec();
}


void MainWindow::onSettingsChooseDic()
{
    QString fileName =
        QFileDialog::getOpenFileName(this, _q("Choose a dictionary"), "", "*.dawg");
    changeDictionary(fileName);
}


void MainWindow::onSettingsCreateDic()
{
    DicWizard *wizard = new DicWizard(this, m_dic);
    wizard->setWindowTitle(_q("Dictionary creation wizard"));
    connect(wizard, SIGNAL(notifyInfo(QString)),
            this, SLOT(displayInfoMsg(QString)));
    connect(wizard, SIGNAL(notifyProblem(QString)),
            this, SLOT(displayErrorMsg(QString)));
    connect(wizard, SIGNAL(loadDictionary(QString)),
            this, SLOT(changeDictionary(QString)));
    wizard->show();
}


void MainWindow::onSettingsFavPlayers()
{
    FavPlayersDialog *dialog = new FavPlayersDialog(this);
    QObject::connect(dialog, SIGNAL(notifyProblem(QString)),
                     this, SLOT(displayErrorMsg(QString)));
    dialog->exec();
}


void MainWindow::onSettingsDefineTables()
{
    ASSERT(m_game != NULL, "A game should have been started");

    TablesDialog *dialog = new TablesDialog(this, *m_game);
    QObject::connect(dialog, SIGNAL(notifyProblem(QString)),
                     this, SLOT(displayErrorMsg(QString)));
    if (dialog->exec() == QDialog::Accepted)
    {
        m_gameSignals->notifyGameUpdated();
    }
}


void MainWindow::onWindowsToolbar()
{
    if (m_ui.toolBar->isVisible())
        m_ui.toolBar->hide();
    else
        m_ui.toolBar->show();
    QSettings qs;
    qs.setValue(PrefsDialog::kINTF_SHOW_TOOLBAR, m_ui.toolBar->isVisible());
}


void MainWindow::onWindowsBag()
{
    if (m_bagWindow == NULL)
    {
        // Create the window
        BagWidget2 *bag = new BagWidget2(NULL);
        bag->setGame(m_game);
        m_bagWindow = new AuxWindow(*bag, _q("Bag"), "BagWindow",
                                    m_actionWindowsBag);
        QObject::connect(m_gameSignals, SIGNAL(gameChanged(const PublicGame*)),
                         bag, SLOT(setGame(const PublicGame*)));
        QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                         bag, SLOT(refresh()));
    }
    m_bagWindow->toggleVisibility();
}


void MainWindow::onWindowsBoard()
{
    if (m_boardWindow == NULL)
    {
        // Create the window
        QFrame *frame = new QFrame;
        QLayout *hLayout = new QHBoxLayout;
        frame->setLayout(hLayout);

        QSplitter *vSplitter = new QSplitter(Qt::Vertical);
        hLayout->addWidget(vSplitter);

        QSplitter *hSplitter = new QSplitter(Qt::Horizontal);
        vSplitter->addWidget(hSplitter);

        RackWidget *rackWidget = new RackWidget;
        rackWidget->setShowOnlyLastTurn(true);
        rackWidget->setGame(m_game);
        QObject::connect(rackWidget, SIGNAL(gameUpdated()),
                         m_gameSignals, SLOT(notifyGameUpdated()));
        QObject::connect(m_gameSignals, SIGNAL(gameChanged(const PublicGame*)),
                         rackWidget, SLOT(setGame(const PublicGame*)));
        QObject::connect(m_gameSignals, SIGNAL(currPlayerRackChanged(const PlayedRack&)),
                         rackWidget, SLOT(setRack(const PlayedRack&)));
        hSplitter->addWidget(rackWidget);

        hSplitter->addWidget(new QWidget);

        TimerWidget *timerWidget = new TimerWidget(NULL, *m_timerModel);
        hSplitter->addWidget(timerWidget);

        vSplitter->addWidget(new QWidget);

        BoardWidget *board = new BoardWidget(m_playModel);
        board->setShowTempSigns(false);
        board->setShowOnlyLastTurn(true);
        board->setGame(m_game);
        QObject::connect(m_gameSignals, SIGNAL(gameChanged(const PublicGame*)),
                         board, SLOT(setGame(const PublicGame*)));
        QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                         board, SLOT(refresh()));
        vSplitter->addWidget(board);

        m_boardWindow = new AuxWindow(*frame, _q("Board"), "BoardWindow",
                                      m_actionWindowsBoard);

        // Try to have decent initial sizes
        QList<int> sizesVertical;
        sizesVertical << 80 << 20 << 1;
        vSplitter->setSizes(sizesVertical);
        vSplitter->setStretchFactor(0, 0);
        vSplitter->setStretchFactor(1, 0);
        vSplitter->setStretchFactor(2, 1);

        QList<int> sizesHorizontal;
        sizesHorizontal << 1 << 40 << 150;
        hSplitter->setSizes(sizesHorizontal);
        hSplitter->setStretchFactor(0, 1);
        hSplitter->setStretchFactor(1, 0);
        hSplitter->setStretchFactor(2, 0);
    }
    m_boardWindow->toggleVisibility();
}


void MainWindow::onWindowsHistory()
{
    if (m_historyWindow == NULL)
    {
        // Create the window
        HistoryTabWidget *history = new HistoryTabWidget(NULL);
        history->setGame(m_game);
        m_historyWindow = new AuxWindow(*history, _q("History"), "HistoryWindow",
                                        m_actionWindowsHistory);
        QObject::connect(m_gameSignals, SIGNAL(gameChanged(const PublicGame*)),
                         history, SLOT(setGame(const PublicGame*)));
        QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                         history, SLOT(refresh()));
    }
    m_historyWindow->toggleVisibility();
}


void MainWindow::onWindowsStatistics()
{
    if (m_statsWindow == NULL)
    {
        // Create the window
        StatsWidget *stats = new StatsWidget;
        stats->setGame(m_game);
        m_statsWindow = new AuxWindow(*stats, _q("Statistics"), "StatsWindow",
                                      m_actionWindowsStats);
        QObject::connect(m_gameSignals, SIGNAL(gameChanged(const PublicGame*)),
                         stats, SLOT(setGame(const PublicGame*)));
        QObject::connect(m_gameSignals, SIGNAL(gameUpdated()),
                         stats, SLOT(refresh()));
    }
    m_statsWindow->toggleVisibility();
}


void MainWindow::onWindowsTimer()
{
    if (m_timerWindow == NULL)
    {
        // Create the window
        TimerWidget *timer = new TimerWidget(NULL, *m_timerModel);
        m_timerWindow = new AuxWindow(*timer, _q("Timer"), "TimerWindow",
                                      m_actionWindowsTimer);
    }
    m_timerWindow->toggleVisibility();
}


void MainWindow::onWindowsDicTools()
{
    if (m_dicToolsWindow == NULL)
    {
        // Create the window
        DicToolsWidget *dicTools = new DicToolsWidget(NULL);
        m_dicToolsWindow = new AuxWindow(*dicTools, _q("Dictionary tools"), "DicTools",
                                    m_actionWindowsDicTools);
        QObject::connect(this, SIGNAL(dicChanged(const Dictionary*)),
                         dicTools, SLOT(setDic(const Dictionary*)));
        QObject::connect(dicTools, SIGNAL(requestDefinition(QString)),
                         this, SLOT(showDefinition(QString)));
        // Link the arbitration rack with the "Plus 1" one
        linkArbitrationAnd7P1();
        // Fake a dictionary selection
        dicTools->setDic(m_dic);
        dicTools->setFocus();
    }
    m_dicToolsWindow->toggleVisibility();
}


void MainWindow::onHelpAbout()
{
    QString msg = QString("Eliot %1<br><br>").arg(VERSION);
    msg += _q("Copyright (C) 1999-2012 - Antoine Fraboulet & Olivier Teuliere.");
    msg += "<br><br>";
    msg += _q( \
        "This program is free software; you can redistribute it and/or " \
        "modify it under the terms of the GNU General Public License as " \
        "published by the Free Software Foundation; either version 2 of " \
        "the License, or (at your option) any later version.");
    msg += "<br><br>";
    // TRANSLATORS: If the website is translated in your language,
    // feel free to adapt the URL.
    QString url = _q("http://www.nongnu.org/eliot/en/");
    msg += _q("Web site: %1").arg(QString("<a href=\"%1\">%2</a>").arg(url).arg(url));
    // QMessageBox::about() doesn't add the nice information icon, so we create
    // the box manually (not much work...)
    QMessageBox aboutBox(QMessageBox::Information, _q("About Eliot"),
                         msg, QMessageBox::Ok, this);
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.exec();
}


void MainWindow::onHistoryFirstTurn()
{
    if (m_game == NULL)
        return;

    m_game->firstTurn();
    m_gameSignals->notifyGameUpdated();
    unsigned int currTurn = m_game->getCurrTurn();
    unsigned int nbTurns = m_game->getNbTurns();
    displayInfoMsg(_q("Turn %1/%2").arg(currTurn).arg(nbTurns));
}


void MainWindow::onHistoryPrevTurn()
{
    if (m_game == NULL)
        return;

    m_game->prevTurn();
    m_gameSignals->notifyGameUpdated();
    unsigned int currTurn = m_game->getCurrTurn();
    unsigned int nbTurns = m_game->getNbTurns();
    displayInfoMsg(_q("Turn %1/%2").arg(currTurn).arg(nbTurns));
}


void MainWindow::onHistoryNextTurn()
{
    if (m_game == NULL)
        return;

    m_game->nextTurn();
    m_gameSignals->notifyGameUpdated();
    unsigned int currTurn = m_game->getCurrTurn();
    unsigned int nbTurns = m_game->getNbTurns();
    displayInfoMsg(_q("Turn %1/%2").arg(currTurn).arg(nbTurns));
}


void MainWindow::onHistoryLastTurn()
{
    if (m_game == NULL)
        return;

    m_game->lastTurn();
    m_gameSignals->notifyGameUpdated();
    unsigned int currTurn = m_game->getCurrTurn();
    unsigned int nbTurns = m_game->getNbTurns();
    displayInfoMsg(_q("Turn %1/%2").arg(currTurn).arg(nbTurns));
}


void MainWindow::onHistoryReplayTurn()
{
    if (m_game == NULL)
        return;

    // Ask for a confirmation, because this may lead to data loss
    QString msg = _q("Replaying this turn will modify the game history "
                     "by deleting the turns after the displayed one (i.e. "
                     "turns \"in the future\").");
    if (!QtCommon::requestConfirmation(PrefsDialog::kCONFO_REPLAY_TURN,
                                       msg, "", this))
    {
        return;
    }

    unsigned int currTurn = m_game->getCurrTurn();
    m_game->clearFuture();
    m_gameSignals->notifyGameUpdated();
    displayInfoMsg(_q("Replaying from turn %1").arg(currTurn));
}

