#include <QApplication>
#include <QColor>
#include <QFont>
#include <QFrame>
#include <QGradient>
#include <QHBoxLayout>
#include <QLinearGradient>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSizePolicy>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

#ifndef MELODY_MIX_VERSION
#define MELODY_MIX_VERSION "0.1.0"
#endif

namespace
{
    enum class MetalFinish
    {
        Black,
        Grey,
        Olive
    };

    QColor baseColor(MetalFinish finish)
    {
        switch (finish)
        {
        case MetalFinish::Black:
            return QColor(37, 40, 38);
        case MetalFinish::Grey:
            return QColor(100, 103, 98);
        case MetalFinish::Olive:
            return QColor(83, 89, 62);
        }

        return QColor(37, 40, 38);
    }

    class MetalPanel final : public QFrame
    {
    public:
        explicit MetalPanel(
            MetalFinish finish = MetalFinish::Black,
            QWidget* parent = nullptr)
            : QFrame(parent), finish_(finish)
        {
            setFrameShape(QFrame::NoFrame);
            setAttribute(Qt::WA_OpaquePaintEvent);
        }

    protected:
        void paintEvent(QPaintEvent*) override
        {
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing, false);

            const QColor base = baseColor(finish_);
            const QRect area = rect();

            QLinearGradient gradient(0, 0, 0, height());
            gradient.setColorAt(0.0, base.lighter(135));
            gradient.setColorAt(0.12, base.lighter(112));
            gradient.setColorAt(0.5, base);
            gradient.setColorAt(0.88, base.darker(112));
            gradient.setColorAt(1.0, base.darker(145));

            painter.fillRect(area, gradient);

            // Fine horizontal highlights imitate brushed metal grain.
            for (int y = 2; y < height(); y += 3)
            {
                const int variation = (y * 17 + y / 3 * 7) % 13;

                QColor grain = base.lighter(variation % 2 == 0
                    ? 119 : 107);
                grain.setAlpha(variation < 4 ? 48 : 18);

                painter.setPen(grain);
                painter.drawLine(2, y, width() - 3, y);
            }

            // Recessed border and bright upper edge.
            painter.setPen(base.lighter(145));
            painter.drawLine(0, 0, width() - 1, 0);
            painter.drawLine(0, 0, 0, height() - 1);

            painter.setPen(base.darker(190));
            painter.drawLine(0, height() - 1,
                             width() - 1, height() - 1);
            painter.drawLine(width() - 1, 0,
                             width() - 1, height() - 1);

            // Small corner fasteners.
            if (width() > 80 && height() > 55)
            {
                const QPoint corners[] = {
                    QPoint(9, 9),
                    QPoint(width() - 10, 9),
                    QPoint(9, height() - 10),
                    QPoint(width() - 10, height() - 10)
                };

                for (const QPoint& point : corners)
                {
                    painter.setPen(QPen(QColor(18, 19, 17), 1));
                    painter.setBrush(QColor(115, 117, 106));
                    painter.drawEllipse(point, 3, 3);

                    painter.setPen(QColor(35, 36, 32));
                    painter.drawLine(
                        point.x() - 2, point.y(),
                        point.x() + 2, point.y());
                }
            }
        }

    private:
        MetalFinish finish_;
    };

    class IndustrialButton final : public QPushButton
    {
    public:
        explicit IndustrialButton(
            const QString& text,
            QWidget* parent = nullptr)
            : QPushButton(text, parent)
        {
            setMinimumHeight(30);
            setCursor(Qt::PointingHandCursor);
            setFocusPolicy(Qt::StrongFocus);

            setStyleSheet(R"(
                QPushButton {
                    color: #e1e2d7;
                    background-color: #454943;
                    border-top: 2px solid #777c70;
                    border-left: 2px solid #777c70;
                    border-right: 2px solid #20231f;
                    border-bottom: 2px solid #20231f;
                    padding: 5px 12px;
                    font-size: 12px;
                    font-weight: bold;
                }

                QPushButton:hover {
                    background-color: #565d48;
                    color: #ffffff;
                }

                QPushButton:pressed {
                    background-color: #30342d;
                    border-top: 2px solid #20231f;
                    border-left: 2px solid #20231f;
                    border-right: 2px solid #777c70;
                    border-bottom: 2px solid #777c70;
                    padding-top: 7px;
                    padding-left: 14px;
                }

                QPushButton:focus {
                    outline: 1px dotted #d7d9c7;
                }

                QPushButton:disabled {
                    color: #85887f;
                    background-color: #393c37;
                }
            )");
        }
    };

    QLabel* makeLabel(
        const QString& text,
        const QString& color,
        int size,
        QWidget* parent = nullptr)
    {
        auto* label = new QLabel(text, parent);

        label->setStyleSheet(
            QString("color: %1; font-size: %2px;")
                .arg(color)
                .arg(size));

        return label;
    }

    void applyApplicationStyle(QApplication& app)
    {
        app.setStyleSheet(R"(
            QMainWindow {
                background-color: #292c28;
            }

            QMenuBar {
                background-color: #b7b9ad;
                color: #171914;
                border-bottom: 2px solid #171914;
                padding: 3px;
            }

            QMenuBar::item {
                background: transparent;
                padding: 4px 10px;
            }

            QMenuBar::item:selected {
                background-color: #d0d2c5;
                border: 1px solid #65675c;
            }

            QMenu {
                background-color: #b7b9ad;
                color: #171914;
                border: 1px solid #171914;
            }

            QMenu::item {
                padding: 5px 24px 5px 22px;
            }

            QMenu::item:selected {
                background-color: #747b57;
                color: white;
            }

            QStatusBar {
                background-color: #b7b9ad;
                color: #171914;
                border-top: 2px solid #171914;
            }

            QStatusBar::item {
                border: none;
            }
        )");
    }
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("MelodyMix Studio");
    app.setApplicationVersion(MELODY_MIX_VERSION);
    app.setOrganizationName("RealAnomalyGames");

    applyApplicationStyle(app);

    QMainWindow window;
    window.setWindowTitle("MelodyMix Studio - Studio 1");
    window.resize(1000, 680);
    window.setMinimumSize(760, 520);

    auto* central = new QWidget(&window);
    central->setStyleSheet("background-color: #292c28;");

    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(8);

    // Top identification panel.
    auto* header = new MetalPanel(MetalFinish::Black);
    header->setMinimumHeight(90);

    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(24, 18, 24, 18);
    headerLayout->setSpacing(18);

    auto* logo = makeLabel(
        "MM", "#c3c99b", 27, header);
    logo->setAlignment(Qt::AlignCenter);
    logo->setFixedSize(58, 48);
    logo->setStyleSheet(R"(
        color: #d0d5a8;
        font-size: 27px;
        font-weight: bold;
        background-color: #363b2c;
        border: 2px solid #858b65;
    )");

    auto* titleBox = new QVBoxLayout;
    titleBox->setSpacing(3);

    auto* title = makeLabel(
        "MELODY MIX STUDIO", "#e4e5d9", 22, header);

    QFont titleFont = title->font();
    titleFont.setBold(true);
    titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.4);
    title->setFont(titleFont);

    auto* subtitle = makeLabel(
        "INDUSTRIAL AUDIO WORKSTATION",
        "#aeb39a", 10, header);

    titleBox->addWidget(title);
    titleBox->addWidget(subtitle);

    auto* version = makeLabel(
        QString("STUDIO 1\nBUILD 002\nv%1")
            .arg(QApplication::applicationVersion()),
        "#c6c9b5", 10, header);
    version->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    headerLayout->addWidget(logo);
    headerLayout->addLayout(titleBox, 1);
    headerLayout->addWidget(version);

    root->addWidget(header);

    // Transport deck: visual controls only in Build 002.
    auto* transport = new MetalPanel(MetalFinish::Grey);
    transport->setMinimumHeight(76);

    auto* transportLayout = new QHBoxLayout(transport);
    transportLayout->setContentsMargins(18, 12, 18, 12);
    transportLayout->setSpacing(10);

    auto* playButton = new IndustrialButton("▶  PLAY", transport);
    auto* stopButton = new IndustrialButton("■  STOP", transport);

    playButton->setMinimumWidth(105);
    stopButton->setMinimumWidth(105);

    auto* transportLabel = makeLabel(
        "TRANSPORT", "#20231f", 10, transport);
    transportLabel->setStyleSheet(
        "color: #252820; font-size: 10px; font-weight: bold;");

    auto* transportSpacer = new QWidget(transport);
    transportSpacer->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* statusIndicator = makeLabel(
        "●  ENGINE OFFLINE", "#e0c77c", 11, transport);
    statusIndicator->setStyleSheet(
        "color: #373b2c; font-size: 11px; font-weight: bold;");

    transportLayout->addWidget(transportLabel);
    transportLayout->addSpacing(8);
    transportLayout->addWidget(playButton);
    transportLayout->addWidget(stopButton);
    transportLayout->addWidget(transportSpacer);
    transportLayout->addWidget(statusIndicator);

    // Main workspace panels.
    auto* workspace = new QHBoxLayout;
    workspace->setSpacing(8);

    auto* leftPanel = new MetalPanel(MetalFinish::Olive);
    leftPanel->setMinimumWidth(170);
    leftPanel->setMaximumWidth(230);

    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(18, 20, 18, 18);
    leftLayout->setSpacing(9);

    auto* patternsTitle = makeLabel(
        "PATTERN SELECT", "#e1e2d1", 13, leftPanel);

    leftLayout->addWidget(patternsTitle);

    for (int i = 1; i <= 9; ++i)
    {
        auto* patternButton = new IndustrialButton(
            QString("PATTERN  %1").arg(i, 2, 10, QChar('0')),
            leftPanel);

        patternButton->setEnabled(false);
        leftLayout->addWidget(patternButton);
    }

    leftLayout->addStretch();

    auto* patternNote = makeLabel(
        "9 PATTERN SLOTS\n16 STEPS PER CHANNEL",
        "#d0d4bc", 10, leftPanel);
    patternNote->setWordWrap(true);

    leftLayout->addWidget(patternNote);

    auto* editorPanel = new MetalPanel(MetalFinish::Black);

    auto* editorLayout = new QVBoxLayout(editorPanel);
    editorLayout->setContentsMargins(20, 20, 20, 20);
    editorLayout->setSpacing(12);

    auto* editorTitle = makeLabel(
        "SEQUENCER WORKSPACE", "#dfe1d4", 16, editorPanel);

    auto* editorSubtitle = makeLabel(
        "STEP EDITOR  /  CHANNEL ROUTING  /  PATTERN CONTROL",
        "#aeb49a", 10, editorPanel);

    auto* placeholder = new MetalPanel(MetalFinish::Grey);
    placeholder->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(18, 18, 18, 18);

    auto* placeholderTitle = makeLabel(
        "SEQUENCER OFFLINE", "#242720", 17, placeholder);

    placeholderTitle->setAlignment(Qt::AlignCenter);

    auto* placeholderText = makeLabel(
        "The industrial interface is ready.\n\n"
        "The editable 16-step channel grid will arrive in Build 003.\n"
        "Audio playback and tempo control will follow in later builds.",
        "#34372f", 11, placeholder);

    placeholderText->setAlignment(Qt::AlignCenter);
    placeholderText->setWordWrap(true);

    placeholderLayout->addStretch();
    placeholderLayout->addWidget(placeholderTitle);
    placeholderLayout->addSpacing(8);
    placeholderLayout->addWidget(placeholderText);
    placeholderLayout->addStretch();

    editorLayout->addWidget(editorTitle);
    editorLayout->addWidget(editorSubtitle);
    editorLayout->addWidget(placeholder, 1);

    workspace->addWidget(leftPanel);
    workspace->addWidget(editorPanel, 1);

    root->addWidget(transport);
    root->addLayout(workspace, 1);

    window.setCentralWidget(central);

    // Classic desktop menu structure.
    auto* fileMenu = window.menuBar()->addMenu("&File");
    fileMenu->addAction("New Project...", [&window]()
    {
        window.statusBar()->showMessage(
            "Project creation will be implemented in a later build.", 4000);
    });
    fileMenu->addAction("Open Project...", [&window]()
    {
        window.statusBar()->showMessage(
            "Project loading will be implemented in a later build.", 4000);
    });
    fileMenu->addSeparator();
    fileMenu->addAction("Save Project", [&window]()
    {
        window.statusBar()->showMessage(
            "Project saving will be implemented in a later build.", 4000);
    });
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", &app, &QApplication::quit);

    auto* viewMenu = window.menuBar()->addMenu("&View");
    auto* blackAction = viewMenu->addAction("Black Metal Finish");
    auto* greyAction = viewMenu->addAction("Grey Metal Finish");
    auto* oliveAction = viewMenu->addAction("Olive Metal Finish");

    blackAction->setEnabled(false);
    greyAction->setEnabled(false);
    oliveAction->setEnabled(false);

    auto* helpMenu = window.menuBar()->addMenu("&Help");
    helpMenu->addAction("About MelodyMix Studio", [&window]()
    {
        window.statusBar()->showMessage(
            QString("MelodyMix Studio %1 | Build 002")
                .arg(QApplication::applicationVersion()),
            5000);
    });

    QObject::connect(playButton, &QPushButton::clicked, [&window]()
    {
        window.statusBar()->showMessage(
            "Audio playback is not implemented in Build 002.", 4000);
    });

    QObject::connect(stopButton, &QPushButton::clicked, [&window]()
    {
        window.statusBar()->showMessage(
            "No audio is currently playing.", 3000);
    });

    window.statusBar()->showMessage(
        QString("MelodyMix Studio %1 | Build 002 | Interface Ready")
            .arg(QApplication::applicationVersion()));

    window.show();

    return app.exec();
}
