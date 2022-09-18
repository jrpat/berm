#include <cstdlib>

#include <QApplication>
#include <QtDebug>
#include <QIcon>
#include <QFont>
#include <QFontDatabase>
#include <QKeySequence>
#include <QAction>
#include <QMainWindow>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QStyle>
#include <QProxyStyle>

#include <functional>

#include "qtermwidget.h"

#ifdef __APPLE__
extern "C" void macos_hide_titlebar(long winid);
#endif

#ifdef __APPLE__
const QFont DEFAULT_FONT = QFont("SF Mono", 12);
#else
const QFont DEFAULT_FONT = QFont("Fira Mono", 10);
#endif


class BQStyle: public QProxyStyle {
public:
    int styleHint(StyleHint sh, const QStyleOption *opt, const QWidget *w, QStyleHintReturn *hret) const override {
        if (sh == SH_ScrollBar_Transient)
            return true;
        return QProxyStyle::styleHint(sh, opt, w, hret);
    }
};


void add_shortcut(QWidget *parent,
                  QKeySequence const & key,
                  std::function<void()> cb) {
    QAction *action = new QAction(parent);
    action->setShortcut(key);
    QObject::connect(action, &QAction::triggered, cb);
    parent->addAction(action);
}


static int _console_cnt;
static QTermWidget *_console_active = nullptr;

void new_console(QApplication *app) {
    QTermWidget *console = new QTermWidget(0);  // startnow = 0
    _console_cnt += 1;

    console->setTerminalSizeHint(false);
    console->setColorScheme("SolarizedLight");
    console->setKeyBindings("default");
    console->setMotionAfterPasting(2);  // scroll to end
    console->setTerminalFont(DEFAULT_FONT);
    console->setBidiEnabled(false);
    if (_console_active)
        console->setWorkingDirectory(_console_active->workingDirectory());
    else
        console->setWorkingDirectory("$HOME");

#ifdef __APPLE__
    macos_hide_titlebar(console->winId());
    console->setScrollBarPosition(QTermWidget::ScrollBarPosition::ScrollBarRight);
#endif

    auto close_console = [=](){
        console->close();
        console->deleteLater();
        if (--_console_cnt == 0)
            app->quit();
    };

    add_shortcut(console, QKeySequence("Ctrl+Shift+C"),
                 [=](){ console->copyClipboard(); });
    add_shortcut(console, QKeySequence("Meta+C"),
                 [=](){ console->copyClipboard(); });
    add_shortcut(console, QKeySequence("Ctrl+Shift+V"),
                 [=](){ console->pasteClipboard(); });
    add_shortcut(console, QKeySequence("Meta+V"),
                 [=](){ console->pasteClipboard(); });
    add_shortcut(console, QKeySequence("Ctrl+Shift+F"),
                 [=](){ console->toggleShowSearchBar(); });
    add_shortcut(console, QKeySequence("Meta+F"),
                 [=](){ console->toggleShowSearchBar(); });
    add_shortcut(console, QKeySequence(Qt::META, Qt::Key_Minus),
                 [=](){ console->zoomOut(); });
    add_shortcut(console, QKeySequence(Qt::META, Qt::Key_Plus),
                 [=](){ console->zoomIn(); });
    add_shortcut(console, QKeySequence("Meta+n"),
                 [=](){ new_console(app); });
    add_shortcut(console, QKeySequence("Meta+w"), close_console);

    QObject::connect(console, &QTermWidget::urlActivated, [](const QUrl& url, bool) {
        if (QApplication::keyboardModifiers() & (Qt::ControlModifier | Qt::MetaModifier))
            QDesktopServices::openUrl(url);
    });
    QObject::connect(console, &QTermWidget::finished, close_console);
    QObject::connect(console, &QTermWidget::termGetFocus, [=](){
        _console_active = console;
    });
    if (!_console_active)
        _console_active = console;

    console->startShellProgram();
    console->show();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_MacDontSwapCtrlAndMeta, true);
    QApplication* app = new QApplication(argc, argv);

#ifdef __APPLE__
    BQStyle *style = new BQStyle;
    style->setBaseStyle(app->style());
    app->setStyle(style);
#endif

    new_console(app);

    return app->exec();
}
