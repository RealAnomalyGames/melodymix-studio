#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#ifndef MELODY_MIX_VERSION
#define MELODY_MIX_VERSION "0.1.0"
#endif

namespace
{
    void applyIndustrialStyle(QApplication& app)
    {
        app.setStyleSheet(R"(
            QMainWindow {
                background-color: #363a36;
                color: #e0e0d4;
            }

            QMenuBar {
                background-color: #b6b7aa;
                color: #171914;
                border-bottom: 2px solid #171914;
                padding: 3px;
            }

            QMenuBar::item {
                background: transparent;
                padding: 4px 8px;
            }

            QMenuBar::item:selected {
                background-color: #d0d1c5;
                border: 1px solid #62645a;
            }

            QMenu {
                background-color: #b6b7aa;
                color: #171914;
                border: 1px solid #171914;
            }

            QMenu::item:selected {
                background-color: #747957;
                color: #ffffff;
            }

            QStatusBar {
                background-color: #b6b7aa;
                color: #171914;
                border-top: 2px solid #171914;
            }

            QLabel#titleLabel {
                color: #d6d8c6;
                font-size: 24px;
                font-weight: bold;
            }

            QLabel#subtitleLabel {
                color: #a9ad9a;
                font-size: 12px;
            }

            QLabel#panelLabel {
                background-color: #252824;
                color: #c0c4a7;
                border: 1px solid #55594c;
                padding: 12px;
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

    applyIndustrialStyle(app);

    QMainWindow window;
    window.setWindowTitle("MelodyMix Studio - Studio 1");
    window.resize(960, 640);
    window.setMinimumSize(720, 480);

    auto* centralWidget = new QWidget(&window);
    auto* layout = new QVBoxLayout(centralWidget);

    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    auto* titleLabel = new QLabel("MELODY MIX STUDIO");
    titleLabel->setObjectName("titleLabel");

    auto* subtitleLabel = new QLabel(
        "STUDIO 1  |  INDUSTRIAL AUDIO WORKSTATION"
    );
    subtitleLabel->setObjectName("subtitleLabel");

    auto* panelLabel = new QLabel(
        "SEQUENCER OFFLINE\n\n"
        "The pattern editor is not implemented yet.\n"
        "Your sounds and project files will be supported in a later stage."
    );
    panelLabel->setObjectName("panelLabel");
    panelLabel->setAlignment(Qt::AlignCenter);
    panelLabel->setWordWrap(true);
    panelLabel->setMinimumHeight(240);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addStretch();
    layout->addWidget(panelLabel, 1);
    layout->addStretch();

    window.setCentralWidget(centralWidget);

    auto* fileMenu = window.menuBar()->addMenu("&File");
    fileMenu->addAction("New Project");
    fileMenu->addAction("Open Project...");
    fileMenu->addSeparator();
    fileMenu->addAction("Save Project");
    fileMenu->addAction("Save Project As...");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", &app, &QApplication::quit);

    auto* helpMenu = window.menuBar()->addMenu("&Help");
    helpMenu->addAction("About MelodyMix Studio", [&window]()
    {
        window.statusBar()->showMessage(
            QString("MelodyMix Studio %1")
                .arg(QApplication::applicationVersion()),
            5000
        );
    });

    window.statusBar()->showMessage(
        QString("Studio 1 | Version %1 | Ready")
            .arg(QApplication::applicationVersion())
    );

    window.show();

    return app.exec();
}
