
#include <QApplication>
#include <QAudioOutput>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QUrl>

#include <array>

class MetalPanel : public QWidget
{
public:
    explicit MetalPanel(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_StyledBackground, true);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, false);

        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0.0, QColor(105, 108, 106));
        gradient.setColorAt(0.18, QColor(75, 78, 76));
        gradient.setColorAt(0.50, QColor(56, 59, 57));
        gradient.setColorAt(0.82, QColor(71, 74, 72));
        gradient.setColorAt(1.0, QColor(42, 44, 43));

        painter.fillRect(rect(), gradient);

        for (int y = 2; y < height(); y += 4) {
            painter.setPen(QColor(255, 255, 255, 7));
            painter.drawLine(0, y, width(), y);
        }

        painter.setPen(QColor(15, 16, 15, 180));
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }
};

class StepButton : public QPushButton
{
public:
    explicit StepButton(int step, QWidget *parent = nullptr)
        : QPushButton(parent), step_(step)
    {
        setCheckable(true);
        setFixedSize(23, 27);
        setFocusPolicy(Qt::NoFocus);
        setAccessibleName(QStringLiteral("Step %1").arg(step + 1));
        setToolTip(QStringLiteral("Step %1").arg(step + 1));
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF buttonRect = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);

        QLinearGradient gradient(buttonRect.topLeft(), buttonRect.bottomLeft());

        if (isChecked()) {
            gradient.setColorAt(0.0, QColor(192, 92, 89));
            gradient.setColorAt(0.35, QColor(145, 54, 53));
            gradient.setColorAt(1.0, QColor(83, 32, 32));
        } else {
            gradient.setColorAt(0.0, QColor(118, 121, 118));
            gradient.setColorAt(0.25, QColor(81, 84, 82));
            gradient.setColorAt(1.0, QColor(42, 44, 43));
        }

        painter.setBrush(gradient);
        painter.setPen(QPen(QColor(22, 23, 22), 1.0));
        painter.drawRoundedRect(buttonRect, 3, 3);

        painter.setPen(QPen(
            isChecked() ? QColor(228, 137, 128, 170)
                        : QColor(215, 218, 215, 65),
            1.0));

        painter.drawLine(
            QPointF(buttonRect.left() + 3, buttonRect.top() + 1),
            QPointF(buttonRect.right() - 3, buttonRect.top() + 1));

        if (step_ % 4 == 0) {
            painter.setPen(isChecked()
                ? QColor(255, 213, 200)
                : QColor(220, 220, 212, 180));

            painter.drawPoint(
                QPointF(buttonRect.center().x(), buttonRect.bottom() - 4));
        }
    }

private:
    int step_;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow()
        : player_(new QMediaPlayer(this)),
          audioOutput_(new QAudioOutput(this))
    {
        player_->setAudioOutput(audioOutput_);
        audioOutput_->setVolume(0.75);

        patterns_.fill({});

        setWindowTitle(QStringLiteral("MelodyMix Studio"));
        resize(900, 600);
        setMinimumSize(720, 480);

        buildInterface();
        buildMenus();
        displayPattern(0);

        connect(player_, &QMediaPlayer::errorOccurred,
                this, [this](QMediaPlayer::Error, const QString &message) {
            statusBar()->showMessage(
                QStringLiteral("Preview error: %1").arg(message), 7000);
        });

        connect(player_, &QMediaPlayer::playbackStateChanged,
                this, [this](QMediaPlayer::PlaybackState state) {
            if (state == QMediaPlayer::PlayingState) {
                statusBar()->showMessage(
                    QStringLiteral("Previewing WAV file..."));
            } else if (state == QMediaPlayer::StoppedState) {
                statusBar()->showMessage(
                    QStringLiteral("Ready"), 3000);
            }
        });
    }

private:
    struct Sound
    {
        QString path;
        QString name;
    };

    static constexpr int PatternCount = 9;
    static constexpr int ChannelCount = 4;
    static constexpr int StepCount = 16;

    const std::array<QString, ChannelCount> channelNames_{
        QStringLiteral("KICK"),
        QStringLiteral("SNARE"),
        QStringLiteral("CLOSED HH"),
        QStringLiteral("OPEN HH")
    };

    std::array<
        std::array<std::array<bool, StepCount>, ChannelCount>,
        PatternCount> patterns_{};

    std::array<
        std::array<StepButton *, StepCount>,
        ChannelCount> stepButtons_{};

    std::array<QComboBox *, ChannelCount> assignmentBoxes_{};

    std::array<QString, ChannelCount> assignments_{};

    std::array<QPushButton *, PatternCount> patternButtons_{};

    QList<Sound> sounds_;

    int currentPattern_ = 0;

    QMediaPlayer *player_;
    QAudioOutput *audioOutput_;

    QListWidget *libraryList_ = nullptr;
    QLabel *assignmentSummary_ = nullptr;
    QLabel *patternTitle_ = nullptr;
    QLabel *stepCountLabel_ = nullptr;

    void buildInterface()
    {
        auto *root = new MetalPanel(this);
        auto *rootLayout = new QVBoxLayout(root);
        rootLayout->setContentsMargins(10, 10, 10, 8);
        rootLayout->setSpacing(8);

        auto *header = new QHBoxLayout();

        auto *title = new QLabel(QStringLiteral("MELODY MIX STUDIO"));
        title->setStyleSheet(
            "color: #ecebe1; font-size: 19px; font-weight: bold;"
            " letter-spacing: 2px;");

        auto *version = new QLabel(
            QStringLiteral("STUDIO 1 / BUILD 004 / v%1")
                .arg(QStringLiteral(MELODY_MIX_VERSION)));

        version->setStyleSheet(
            "color: #c1c3bb; font-size: 10px;");

        header->addWidget(title);
        header->addStretch();
        header->addWidget(version);
        rootLayout->addLayout(header);

        auto *transport = new QHBoxLayout();

        auto *playButton = new QPushButton(QStringLiteral("▶  PLAY"));
        auto *stopButton = new QPushButton(QStringLiteral("■  STOP"));

        styleMetalButton(playButton);
        styleMetalButton(stopButton);

        connect(playButton, &QPushButton::clicked, this, [this]() {
            statusBar()->showMessage(
                QStringLiteral("Sequencer playback will be added in Build 005."));
        });

        connect(stopButton, &QPushButton::clicked, this, [this]() {
            player_->stop();
            statusBar()->showMessage(QStringLiteral("Stopped"), 2000);
        });

        transport->addWidget(playButton);
        transport->addWidget(stopButton);

        auto *transportNote = new QLabel(
            QStringLiteral("WAV PREVIEW ACTIVE  |  SEQUENCER AUDIO: BUILD 005"));

        transportNote->setStyleSheet(
            "color: #d4d5ce; font-size: 9px;");

        transport->addStretch();
        transport->addWidget(transportNote);
        rootLayout->addLayout(transport);

        auto *body = new QHBoxLayout();
        body->setSpacing(8);

        // Pattern selector
        auto *patternPanel = makeInsetPanel();
        patternPanel->setFixedWidth(104);

        auto *patternLayout = new QVBoxLayout(patternPanel);
        patternLayout->setContentsMargins(6, 8, 6, 8);
        patternLayout->setSpacing(5);

        auto *patternHeading = makePanelHeading(QStringLiteral("PATTERNS"));
        patternLayout->addWidget(patternHeading);

        for (int i = 0; i < PatternCount; ++i) {
            auto *button = new QPushButton(
                QStringLiteral("PATTERN %1").arg(i + 1));

            button->setCheckable(true);
            button->setMinimumHeight(30);
            button->setStyleSheet(patternButtonStyle());

            patternButtons_[i] = button;
            patternLayout->addWidget(button);

            connect(button, &QPushButton::clicked, this, [this, i]() {
                displayPattern(i);
            });
        }

        patternLayout->addStretch();
        body->addWidget(patternPanel);

        // Sequencer
        auto *sequencerPanel = makeInsetPanel();
        auto *sequencerLayout = new QVBoxLayout(sequencerPanel);
        sequencerLayout->setContentsMargins(8, 8, 8, 8);
        sequencerLayout->setSpacing(7);

        auto *sequencerHeader = new QHBoxLayout();

        patternTitle_ = new QLabel(QStringLiteral("PATTERN 1"));
        patternTitle_->setStyleSheet(
            "color: #f0eee3; font-weight: bold; font-size: 13px;");

        stepCountLabel_ = new QLabel(QStringLiteral("0 ACTIVE STEPS"));
        stepCountLabel_->setStyleSheet(
            "color: #c3c5bc; font-size: 9px;");

        sequencerHeader->addWidget(patternTitle_);
        sequencerHeader->addStretch();
        sequencerHeader->addWidget(stepCountLabel_);
        sequencerLayout->addLayout(sequencerHeader);

        auto *stepNumbers = new QHBoxLayout();
        stepNumbers->setSpacing(3);

        auto *emptyLabel = new QLabel(QString());
        emptyLabel->setFixedWidth(66);
        stepNumbers->addWidget(emptyLabel);

        for (int step = 0; step < StepCount; ++step) {
            auto *number = new QLabel(QString::number(step + 1));
            number->setFixedWidth(23);
            number->setAlignment(Qt::AlignCenter);
            number->setStyleSheet(
                QStringLiteral("color: %1; font-size: 9px;")
                    .arg(step % 4 == 0 ? "#f2d5c7" : "#b8bbb3"));

            stepNumbers->addWidget(number);
        }

        sequencerLayout->addLayout(stepNumbers);

        for (int channel = 0; channel < ChannelCount; ++channel) {
            auto *row = new QHBoxLayout();
            row->setSpacing(3);

            auto *label = new QLabel(channelNames_[channel]);
            label->setFixedWidth(66);
            label->setStyleSheet(
                "color: #e1e1d8; font-size: 9px; font-weight: bold;");

            row->addWidget(label);

            for (int step = 0; step < StepCount; ++step) {
                auto *button = new StepButton(step);
                stepButtons_[channel][step] = button;
                row->addWidget(button);

                connect(button, &QPushButton::toggled,
                        this, [this, channel, step](bool checked) {
                    patterns_[currentPattern_][channel][step] = checked;
                    updateStepCount();
                });
            }

            sequencerLayout->addLayout(row);
        }

        sequencerLayout->addStretch();

        auto *legend = new QLabel(
            QStringLiteral("DARK = OFF     RED = ON     • = BEAT MARKER"));

        legend->setStyleSheet(
            "color: #c1c3ba; font-size: 9px;");

        sequencerLayout->addWidget(legend);
        body->addWidget(sequencerPanel, 1);

        // WAV library and channel assignments
        auto *libraryPanel = makeInsetPanel();
        libraryPanel->setFixedWidth(192);

        auto *libraryLayout = new QVBoxLayout(libraryPanel);
        libraryLayout->setContentsMargins(7, 8, 7, 8);
        libraryLayout->setSpacing(6);

        libraryLayout->addWidget(
            makePanelHeading(QStringLiteral("WAV LIBRARY")));

        libraryList_ = new QListWidget();
        libraryList_->setSelectionMode(QListWidget::SingleSelection);
        libraryList_->setStyleSheet(
            "QListWidget { background: #252725; color: #e2e2d9;"
            " border: 1px solid #171817; padding: 3px; }"
            "QListWidget::item { padding: 4px; }"
            "QListWidget::item:selected { background: #75403d;"
            " color: white; }");

        libraryLayout->addWidget(libraryList_, 1);

        auto *importButton = new QPushButton(QStringLiteral("IMPORT WAV..."));
        auto *previewButton = new QPushButton(QStringLiteral("▶  PREVIEW"));
        auto *removeButton = new QPushButton(QStringLiteral("REMOVE"));

        styleMetalButton(importButton);
        styleMetalButton(previewButton);
        styleMetalButton(removeButton);

        libraryLayout->addWidget(importButton);
        libraryLayout->addWidget(previewButton);
        libraryLayout->addWidget(removeButton);

        connect(importButton, &QPushButton::clicked,
                this, [this]() { importWavFiles(); });

        connect(previewButton, &QPushButton::clicked,
                this, [this]() { previewSelectedSound(); });

        connect(removeButton, &QPushButton::clicked,
                this, [this]() { removeSelectedSound(); });

        libraryLayout->addWidget(
            makePanelHeading(QStringLiteral("CHANNEL ASSIGNMENTS")));

        for (int channel = 0; channel < ChannelCount; ++channel) {
            auto *label = new QLabel(channelNames_[channel]);
            label->setStyleSheet(
                "color: #dedfd5; font-size: 9px; font-weight: bold;");

            auto *combo = new QComboBox();
            combo->setMinimumWidth(0);
            combo->setStyleSheet(
                "QComboBox { background: #292b29; color: #eeeeE5;"
                " border: 1px solid #151615; padding: 4px; }"
                "QComboBox QAbstractItemView { background: #292b29;"
                " color: #eeeeE5; selection-background-color: #75403d; }");

            assignmentBoxes_[channel] = combo;
            libraryLayout->addWidget(label);
            libraryLayout->addWidget(combo);

            connect(combo, &QComboBox::currentIndexChanged,
                    this, [this, channel](int index) {
                if (index < 0) {
                    assignments_[channel].clear();
                } else {
                    assignments_[channel] =
                        assignmentBoxes_[channel]->itemData(index).toString();
                }

                updateAssignmentSummary();
            });
        }

        assignmentSummary_ = new QLabel(
            QStringLiteral("No sounds assigned"));

        assignmentSummary_->setWordWrap(true);
        assignmentSummary_->setStyleSheet(
            "color: #c4c5bc; font-size: 9px;");

        libraryLayout->addWidget(assignmentSummary_);
        body->addWidget(libraryPanel);

        rootLayout->addLayout(body, 1);

        setCentralWidget(root);

        statusBar()->setStyleSheet(
            "QStatusBar { background: #292b29; color: #d9d9d0;"
            " border-top: 1px solid #777a76; }");

        statusBar()->showMessage(
            QStringLiteral("Ready — import a WAV file to begin."));
        refreshAssignmentBoxes();
    }

    void buildMenus()
    {
        auto *fileMenu = menuBar()->addMenu(QStringLiteral("&File"));

        auto *newAction = fileMenu->addAction(QStringLiteral("&New Project"));
        auto *importAction = fileMenu->addAction(
            QStringLiteral("Import &WAV Files..."));

        fileMenu->addSeparator();

        auto *exitAction = fileMenu->addAction(QStringLiteral("E&xit"));

        connect(newAction, &QAction::triggered, this, [this]() {
            for (auto &pattern : patterns_) {
                for (auto &channel : pattern) {
                    channel.fill(false);
                }
            }

            for (auto &assignment : assignments_) {
                assignment.clear();
            }

            displayPattern(0);
            refreshAssignmentBoxes();

            statusBar()->showMessage(
                QStringLiteral("New project created."), 3000);
        });

        connect(importAction, &QAction::triggered,
                this, [this]() { importWavFiles(); });

        connect(exitAction, &QAction::triggered,
                this, [this]() { close(); });

        auto *editMenu = menuBar()->addMenu(QStringLiteral("&Edit"));
        auto *clearAction = editMenu->addAction(
            QStringLiteral("Clear Current Pattern"));

        connect(clearAction, &QAction::triggered, this, [this]() {
            for (int channel = 0; channel < ChannelCount; ++channel) {
                patterns_[currentPattern_][channel].fill(false);
            }

            displayPattern(currentPattern_);
            statusBar()->showMessage(
                QStringLiteral("Current pattern cleared."), 3000);
        });

        auto *helpMenu = menuBar()->addMenu(QStringLiteral("&Help"));
        auto *aboutAction = helpMenu->addAction(
            QStringLiteral("&About MelodyMix Studio"));

        connect(aboutAction, &QAction::triggered, this, [this]() {
            QMessageBox::about(
                this,
                QStringLiteral("About MelodyMix Studio"),
                QStringLiteral(
                    "MelodyMix Studio\n"
                    "Studio 1 — Build 004\n\n"
                    "A Windows 95-inspired industrial step sequencer.\n"
                    "Build 004 introduces the WAV library and audio preview."));
        });
    }

    QWidget *makeInsetPanel()
    {
        auto *panel = new QWidget();
        panel->setStyleSheet(
            "background-color: #393c39;"
            "border: 1px solid #1c1e1c;");
        return panel;
    }

    QLabel *makePanelHeading(const QString &text)
    {
        auto *label = new QLabel(text);
        label->setStyleSheet(
            "color: #e4e3d9; font-weight: bold; font-size: 10px;"
            " padding-bottom: 3px;"
            " border-bottom: 1px solid #666963;");
        return label;
    }

    void styleMetalButton(QPushButton *button)
    {
        button->setMinimumHeight(27);
        button->setStyleSheet(
            "QPushButton { color: #eeeeE5; font-size: 10px;"
            " font-weight: bold; padding: 5px;"
            " background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
            " stop:0 #747874, stop:0.45 #505450, stop:1 #303330);"
            " border: 1px solid #171917; border-top-color: #898c87; }"
            "QPushButton:hover { background: #686c67; }"
            "QPushButton:pressed { background: #303330;"
            " border-top-color: #171917; }");
    }

    QString patternButtonStyle() const
    {
        return QStringLiteral(
            "QPushButton { color: #e2e2d8; font-size: 9px;"
            " font-weight: bold; padding: 3px;"
            " background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
            " stop:0 #70736f, stop:1 #373a37);"
            " border: 1px solid #191b19; }"
            "QPushButton:checked { color: white;"
            " background: #874946; border: 1px solid #bd7770; }"
            "QPushButton:hover { border: 1px solid #b2b4aa; }");
    }

    void displayPattern(int index)
    {
        currentPattern_ = index;

        for (int i = 0; i < PatternCount; ++i) {
            const QSignalBlocker blocker(patternButtons_[i]);
            patternButtons_[i]->setChecked(i == index);
        }

        patternTitle_->setText(
            QStringLiteral("PATTERN %1").arg(index + 1));

        for (int channel = 0; channel < ChannelCount; ++channel) {
            for (int step = 0; step < StepCount; ++step) {
                const QSignalBlocker blocker(stepButtons_[channel][step]);
                stepButtons_[channel][step]->setChecked(
                    patterns_[index][channel][step]);
            }
        }

        updateStepCount();
    }

    void updateStepCount()
    {
        int count = 0;

        for (const auto &channel : patterns_[currentPattern_]) {
            for (bool active : channel) {
                if (active) {
                    ++count;
                }
            }
        }

        stepCountLabel_->setText(
            QStringLiteral("%1 ACTIVE STEPS").arg(count));
    }

    bool isValidWav(const QString &path) const
    {
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        const QByteArray header = file.read(12);

        return header.size() == 12
            && header.mid(0, 4) == QByteArrayLiteral("RIFF")
            && header.mid(8, 4) == QByteArrayLiteral("WAVE");
    }

    void importWavFiles()
    {
        const QStringList files = QFileDialog::getOpenFileNames(
            this,
            QStringLiteral("Import WAV Files"),
            QString(),
            QStringLiteral("WAV Audio (*.wav *.WAV)"));

        if (files.isEmpty()) {
            return;
        }

        int imported = 0;
        int skipped = 0;

        for (const QString &path : files) {
            bool alreadyAdded = false;

            for (const Sound &sound : sounds_) {
                if (QFileInfo(sound.path).canonicalFilePath()
                    == QFileInfo(path).canonicalFilePath()) {
                    alreadyAdded = true;
                    break;
                }
            }

            if (alreadyAdded) {
                ++skipped;
                continue;
            }

            if (!isValidWav(path)) {
                ++skipped;
                QMessageBox::warning(
                    this,
                    QStringLiteral("Invalid WAV File"),
                    QStringLiteral(
                        "This file could not be recognized as a readable "
                        "WAV file:\n\n%1").arg(path));
                continue;
            }

            Sound sound;
            sound.path = QFileInfo(path).absoluteFilePath();
            sound.name = QFileInfo(path).fileName();

            sounds_.append(sound);

            auto *item = new QListWidgetItem(sound.name);
            item->setToolTip(sound.path);
            item->setData(Qt::UserRole, sound.path);
            libraryList_->addItem(item);

            ++imported;
        }

        refreshAssignmentBoxes();

        if (imported > 0) {
            libraryList_->setCurrentRow(libraryList_->count() - imported);
        }

        statusBar()->showMessage(
            QStringLiteral("Imported %1 WAV file(s); skipped %2.")
                .arg(imported)
                .arg(skipped),
            5000);
    }

    void previewSelectedSound()
    {
        QListWidgetItem *item = libraryList_->currentItem();

        if (!item) {
            QMessageBox::information(
                this,
                QStringLiteral("WAV Preview"),
                QStringLiteral("Select a sound in the library first."));
            return;
        }

        const QString path = item->data(Qt::UserRole).toString();

        if (!QFileInfo::exists(path) || !QFileInfo(path).isFile()) {
            QMessageBox::warning(
                this,
                QStringLiteral("WAV File Not Found"),
                QStringLiteral("The original file is no longer available:\n%1")
                    .arg(path));
            return;
        }

        player_->stop();
        player_->setSource(QUrl::fromLocalFile(path));
        player_->play();

        statusBar()->showMessage(
            QStringLiteral("Previewing: %1").arg(item->text()));
    }

    void removeSelectedSound()
    {
        const int row = libraryList_->currentRow();

        if (row < 0 || row >= sounds_.size()) {
            statusBar()->showMessage(
                QStringLiteral("Select a library entry to remove."), 3000);
            return;
        }

        const QString removedPath = sounds_[row].path;

        for (int channel = 0; channel < ChannelCount; ++channel) {
            if (assignments_[channel] == removedPath) {
                assignments_[channel].clear();
            }
        }

        if (player_->source().toLocalFile() == removedPath) {
            player_->stop();
            player_->setSource(QUrl());
        }

        sounds_.removeAt(row);
        delete libraryList_->takeItem(row);

        refreshAssignmentBoxes();

        statusBar()->showMessage(
            QStringLiteral("Removed from library. Original file was not deleted."),
            5000);
    }

    void refreshAssignmentBoxes()
    {
        for (int channel = 0; channel < ChannelCount; ++channel) {
            QComboBox *combo = assignmentBoxes_[channel];

            if (!combo) {
                continue;
            }

            const QSignalBlocker blocker(combo);
            combo->clear();
            combo->addItem(QStringLiteral("(No sound assigned)"), QString());

            for (const Sound &sound : sounds_) {
                combo->addItem(sound.name, sound.path);
            }

            int selectedIndex = 0;

            for (int i = 1; i < combo->count(); ++i) {
                if (combo->itemData(i).toString() == assignments_[channel]) {
                    selectedIndex = i;
                    break;
                }
            }

            combo->setCurrentIndex(selectedIndex);
        }

        updateAssignmentSummary();
    }

    void updateAssignmentSummary()
    {
        int assigned = 0;

        for (const QString &path : assignments_) {
            if (!path.isEmpty()) {
                ++assigned;
            }
        }

        assignmentSummary_->setText(
            QStringLiteral("%1 of %2 channels assigned")
                .arg(assigned)
                .arg(ChannelCount));
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setStyle(QStringLiteral("Fusion"));

    MainWindow window;
    window.show();

    return app.exec();
}
