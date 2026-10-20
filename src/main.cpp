#include <QApplication>
#include <QColor>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPainter>
#include <QPushButton>
#include <QSizePolicy>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QVector>
#include <array>

constexpr int PatternCount = 9;
constexpr int ChannelCount = 4;
constexpr int StepsPerChannel = 16;

using PatternData =
    std::array<std::array<bool, StepsPerChannel>, ChannelCount>;

class MetalPanel : public QFrame
{
public:
    explicit MetalPanel(QWidget *parent = nullptr)
        : QFrame(parent)
    {
        setAttribute(Qt::WA_StyledBackground, false);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0.0, QColor("#414345"));
        gradient.setColorAt(0.45, QColor("#292b2d"));
        gradient.setColorAt(1.0, QColor("#1b1d1f"));
        painter.fillRect(rect(), gradient);

        // Subtle brushed-metal grain.
        painter.setPen(QColor(255, 255, 255, 9));
        for (int y = 3; y < height(); y += 4)
            painter.drawLine(0, y, width(), y);

        painter.setPen(QColor("#55585a"));
        painter.drawRect(rect().adjusted(0, 0, -1, -1));

        painter.setPen(QColor("#171819"));
        painter.drawLine(1, height() - 2, width() - 2, height() - 2);
    }
};

class StepButton : public QPushButton
{
public:
    explicit StepButton(int step, QWidget *parent = nullptr)
        : QPushButton(QString::number(step + 1), parent),
          step_(step)
    {
        setCheckable(true);
        setMinimumSize(29, 36);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setCursor(Qt::PointingHandCursor);

        QFont buttonFont("Arial", 8, QFont::Bold);
        setFont(buttonFont);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRectF outer = QRectF(rect()).adjusted(1, 1, -1, -1);
        const bool active = isChecked();
        const bool hovered = underMouse();

        QColor top;
        QColor bottom;

        if (active) {
            top = QColor("#d77870");
            bottom = QColor("#873d3a");
        } else {
            top = QColor("#555454");
            bottom = QColor("#292829");
        }

        if (hovered && !active) {
            top = QColor("#71615f");
            bottom = QColor("#38302f");
        }

        QLinearGradient gradient(outer.topLeft(), outer.bottomLeft());
        gradient.setColorAt(0.0, top);
        gradient.setColorAt(0.45, top.darker(112));
        gradient.setColorAt(1.0, bottom);

        painter.setPen(QPen(QColor("#141414"), 1.4));
        painter.setBrush(QColor("#181819"));
        painter.drawRoundedRect(outer, 4, 4);

        const QRectF face = outer.adjusted(2, 2, -2, -3);
        painter.setPen(QPen(active ? QColor("#f0a39a")
                                   : QColor("#858080"), 1));
        painter.setBrush(gradient);
        painter.drawRoundedRect(face, 3, 3);

        // Metallic highlight along the top edge.
        painter.setPen(QPen(active ? QColor(255, 207, 198, 110)
                                   : QColor(255, 255, 255, 65), 1));
        painter.drawLine(face.left() + 4, face.top() + 1,
                         face.right() - 4, face.top() + 1);

        // Tiny vertical grain lines give the button a metal finish.
        painter.setPen(QColor(255, 255, 255, active ? 13 : 10));
        for (int x = int(face.left()) + 4;
             x < face.right() - 2; x += 5) {
            painter.drawLine(x, int(face.top()) + 4,
                             x, int(face.bottom()) - 4);
        }

        painter.setPen(active ? QColor("#fff0e8")
                              : QColor("#d4ceca"));
        painter.drawText(face, Qt::AlignCenter,
                         QString::number(step_ + 1));

        if (active) {
            painter.setPen(QColor("#f6b4aa"));
            painter.drawPoint(int(face.center().x()),
                              int(face.bottom()) - 3);
        }
    }

private:
    int step_;
};

class PatternButton : public QPushButton
{
public:
    explicit PatternButton(int pattern, QWidget *parent = nullptr)
        : QPushButton(
              QString("PATTERN %1").arg(pattern + 1, 2, 10, QChar('0')),
              parent)
    {
        setCheckable(true);
        setMinimumHeight(32);
        setCursor(Qt::PointingHandCursor);
        setFont(QFont("Arial", 9, QFont::Bold));
        setStyleSheet(R"(
            QPushButton {
                color: #d6d2c9;
                text-align: left;
                padding: 5px 10px;
                border: 1px solid #171819;
                border-top: 1px solid #656565;
                border-radius: 3px;
                background: qlineargradient(
                    x1:0, y1:0, x2:0, y2:1,
                    stop:0 #555653, stop:1 #30312f);
            }
            QPushButton:hover {
                color: #ffffff;
                background: #62504d;
            }
            QPushButton:checked {
                color: #fff1e9;
                border: 1px solid #bd766e;
                background: qlineargradient(
                    x1:0, y1:0, x2:0, y2:1,
                    stop:0 #a65c55, stop:1 #62302e);
            }
        )");
    }
};

class MainWindow : public QMainWindow
{
public:
    MainWindow()
    {
        setWindowTitle("MelodyMix Studio - Studio 1");
        resize(900, 600);
        setMinimumSize(720, 480);

        for (auto &pattern : patterns_) {
            for (auto &channel : pattern)
                channel.fill(false);
        }

        buildInterface();
        createMenus();
        selectPattern(0);
        statusBar()->showMessage(
            "Build 003 | Ready | Select a pattern and toggle steps");
    }

private:
    std::array<PatternData, PatternCount> patterns_{};
    QVector<PatternButton *> patternButtons_;
    QVector<StepButton *> stepButtons_;
    QLabel *patternTitle_ = nullptr;
    QLabel *activeCount_ = nullptr;
    QLabel *statusLabel_ = nullptr;
    QWidget *sequencerPanel_ = nullptr;
    int currentPattern_ = 0;

    void buildInterface()
    {
        auto *central = new QWidget(this);
        central->setStyleSheet("background-color: #202224;");
        auto *root = new QVBoxLayout(central);
        root->setContentsMargins(10, 8, 10, 9);
        root->setSpacing(8);

        // Compact hardware-style title strip.
        auto *header = new MetalPanel;
        header->setFixedHeight(57);

        auto *headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(13, 5, 13, 5);

        auto *title = new QLabel("MELODY MIX STUDIO");
        title->setStyleSheet(
            "color: #e7e2d8; font-size: 18px; font-weight: bold;"
            " letter-spacing: 2px; background: transparent;");

        auto *version = new QLabel("STUDIO 1  /  BUILD 003  /  v0.1.0");
        version->setStyleSheet(
            "color: #c9a09a; font-size: 10px; font-weight: bold;"
            " background: transparent;");

        headerLayout->addWidget(title);
        headerLayout->addStretch();
        headerLayout->addWidget(version);
        root->addWidget(header);

        // Transport strip. Audio playback will be connected in a later build.
        auto *transport = new MetalPanel;
        transport->setFixedHeight(48);

        auto *transportLayout = new QHBoxLayout(transport);
        transportLayout->setContentsMargins(9, 5, 9, 5);
        transportLayout->setSpacing(7);

        auto *playButton = new QPushButton("▶  PLAY");
        auto *stopButton = new QPushButton("■  STOP");
        auto *transportLabel = new QLabel("TRANSPORT");
        auto *engineLabel = new QLabel("●  AUDIO ENGINE: NOT CONNECTED");

        for (auto *button : {playButton, stopButton}) {
            button->setMinimumSize(86, 31);
            button->setFont(QFont("Arial", 9, QFont::Bold));
            button->setStyleSheet(R"(
                QPushButton {
                    color: #eee7df;
                    border: 1px solid #111111;
                    border-top: 1px solid #77716b;
                    border-radius: 3px;
                    background: qlineargradient(
                        x1:0, y1:0, x2:0, y2:1,
                        stop:0 #5a5956, stop:1 #292a29);
                }
                QPushButton:hover { background: #6a4c49; }
                QPushButton:pressed { background: #342c2b; }
            )");
        }

        transportLabel->setStyleSheet(
            "color: #d7d1c8; font-weight: bold; font-size: 10px;"
            " background: transparent; padding-left: 7px;");
        engineLabel->setStyleSheet(
            "color: #c98a82; font-size: 10px; font-weight: bold;"
            " background: transparent;");

        connect(playButton, &QPushButton::clicked, this, [this]() {
            statusBar()->showMessage(
                "Playback is not available yet. The audio engine is planned "
                "for a later build.", 4000);
        });

        connect(stopButton, &QPushButton::clicked, this, [this]() {
            statusBar()->showMessage(
                "No audio is playing. Playback will be added in a later build.",
                4000);
        });

        transportLayout->addWidget(playButton);
        transportLayout->addWidget(stopButton);
        transportLayout->addWidget(transportLabel);
        transportLayout->addStretch();
        transportLayout->addWidget(engineLabel);
        root->addWidget(transport);

        // Main work area.
        auto *workspace = new QHBoxLayout;
        workspace->setSpacing(9);

        auto *patternPanel = new MetalPanel;
        patternPanel->setFixedWidth(157);
        auto *patternLayout = new QVBoxLayout(patternPanel);
        patternLayout->setContentsMargins(8, 8, 8, 8);
        patternLayout->setSpacing(6);

        auto *patternHeading = new QLabel("PATTERN SELECT");
        patternHeading->setStyleSheet(
            "color: #d7b2a9; font-size: 11px; font-weight: bold;"
            " background: transparent; padding: 3px;");
        patternLayout->addWidget(patternHeading);

        for (int i = 0; i < PatternCount; ++i) {
            auto *button = new PatternButton(i);
            patternButtons_.append(button);
            patternLayout->addWidget(button);

            connect(button, &QPushButton::clicked, this,
                    [this, i]() { selectPattern(i); });
        }

        patternLayout->addStretch();
        workspace->addWidget(patternPanel);

        sequencerPanel_ = new MetalPanel;
        auto *sequencerLayout = new QVBoxLayout(sequencerPanel_);
        sequencerLayout->setContentsMargins(10, 9, 10, 10);
        sequencerLayout->setSpacing(7);

        auto *sequencerHeader = new QHBoxLayout;

        patternTitle_ = new QLabel("PATTERN 01");
        patternTitle_->setStyleSheet(
            "color: #f0e9df; font-size: 14px; font-weight: bold;"
            " letter-spacing: 1px; background: transparent;");

        activeCount_ = new QLabel("0 / 64 STEPS ACTIVE");
        activeCount_->setStyleSheet(
            "color: #d49a91; font-size: 10px; font-weight: bold;"
            " background: transparent;");

        sequencerHeader->addWidget(patternTitle_);
        sequencerHeader->addStretch();
        sequencerHeader->addWidget(activeCount_);
        sequencerLayout->addLayout(sequencerHeader);

        auto *subtitle = new QLabel(
            "STEP SEQUENCER   /   4 CHANNELS   /   16 STEPS");
        subtitle->setStyleSheet(
            "color: #aaa49b; font-size: 10px; font-weight: bold;"
            " background: transparent;");
        sequencerLayout->addWidget(subtitle);

        // Step numbers.
        auto *grid = new QGridLayout;
        grid->setContentsMargins(0, 2, 0, 0);
        grid->setHorizontalSpacing(3);
        grid->setVerticalSpacing(7);

        auto *channelHeader = new QLabel("CHANNEL");
        channelHeader->setMinimumWidth(65);
        channelHeader->setStyleSheet(
            "color: #aaa49b; font-size: 9px; font-weight: bold;"
            " background: transparent;");
        grid->addWidget(channelHeader, 0, 0);

        for (int step = 0; step < StepsPerChannel; ++step) {
            auto *number = new QLabel(QString::number(step + 1));
            number->setAlignment(Qt::AlignCenter);
            number->setStyleSheet(
                "color: #b8b1a8; font-size: 9px;"
                " background: transparent;");
            grid->addWidget(number, 0, step + 1);
        }

        const QStringList channelNames = {
            "KICK", "SNARE", "CLOSED HH", "OPEN HH"
        };

        for (int channel = 0; channel < ChannelCount; ++channel) {
            auto *name = new QLabel(channelNames[channel]);
            name->setMinimumWidth(65);
            name->setStyleSheet(
                "color: #e0d8cd; font-size: 9px; font-weight: bold;"
                " background: transparent;");
            grid->addWidget(name, channel + 1, 0);

            for (int step = 0; step < StepsPerChannel; ++step) {
                auto *button = new StepButton(step);
                button->setProperty("channel", channel);
                button->setProperty("step", step);
                stepButtons_.append(button);
                grid->addWidget(button, channel + 1, step + 1);

                connect(button, &QPushButton::toggled, this,
                        [this, channel, step](bool checked) {
                    patterns_[currentPattern_][channel][step] = checked;
                    updateActiveCount();
                });
            }
        }

        for (int column = 1; column <= StepsPerChannel; ++column)
            grid->setColumnStretch(column, 1);

        sequencerLayout->addLayout(grid);
        sequencerLayout->addStretch();

        auto *hint = new QLabel(
            "CLICK A STEP TO TOGGLE IT   •   EACH PATTERN HAS ITS OWN STEPS");
        hint->setStyleSheet(
            "color: #a49c91; font-size: 9px;"
            " background: transparent; padding-top: 4px;");
        sequencerLayout->addWidget(hint);

        workspace->addWidget(sequencerPanel_, 1);
        root->addLayout(workspace, 1);

        statusLabel_ = new QLabel("BUILD 003  |  STEP EDITOR READY");
        statusLabel_->setStyleSheet(
            "color: #bdb5aa; font-size: 9px; background: transparent;");
        root->addWidget(statusLabel_);

        setCentralWidget(central);
    }

    void selectPattern(int pattern)
    {
        currentPattern_ = pattern;

        for (int i = 0; i < patternButtons_.size(); ++i) {
            const bool selected = (i == pattern);
            patternButtons_[i]->setChecked(selected);
        }

        patternTitle_->setText(
            QString("PATTERN %1").arg(pattern + 1, 2, 10, QChar('0')));

        int index = 0;
        for (int channel = 0; channel < ChannelCount; ++channel) {
            for (int step = 0; step < StepsPerChannel; ++step) {
                const bool active = patterns_[pattern][channel][step];
                stepButtons_[index]->setChecked(active);
                ++index;
            }
        }

        updateActiveCount();
        statusBar()->showMessage(
            QString("Editing Pattern %1").arg(pattern + 1, 2, 10, QChar('0')));
    }

    void updateActiveCount()
    {
        int count = 0;

        for (const auto &channel : patterns_[currentPattern_]) {
            for (bool active : channel) {
                if (active)
                    ++count;
            }
        }

        activeCount_->setText(
            QString("%1 / 64 STEPS ACTIVE").arg(count));
    }

    void createMenus()
    {
        QMenu *fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("&New Project", this, [this]() {
            for (auto &pattern : patterns_) {
                for (auto &channel : pattern)
                    channel.fill(false);
            }
            selectPattern(0);
            statusBar()->showMessage("New empty project created.", 3000);
        });

        fileMenu->addAction("&Open Project...", this, [this]() {
            statusBar()->showMessage(
                "Project loading will be added in a later build.", 4000);
        });

        fileMenu->addAction("&Save Project", this, [this]() {
            statusBar()->showMessage(
                "Project saving will be added in a later build.", 4000);
        });

        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close);

        QMenu *viewMenu = menuBar()->addMenu("&View");
        QAction *black = viewMenu->addAction("Black Metal Finish");
        QAction *grey = viewMenu->addAction("Grey Metal Finish");
        QAction *olive = viewMenu->addAction("Olive Metal Finish");
        black->setEnabled(false);
        grey->setEnabled(false);
        olive->setEnabled(false);

        QMenu *helpMenu = menuBar()->addMenu("&Help");
        helpMenu->addAction("&About MelodyMix Studio", this, [this]() {
            statusBar()->showMessage(
                "MelodyMix Studio - Studio 1 | Build 003 | Step Sequencer",
                5000);
        });
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle("Fusion");
    app.setFont(QFont("Arial", 9));

    MainWindow window;
    window.show();

    return app.exec();
}
