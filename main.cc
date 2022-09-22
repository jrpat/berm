#include <cstdlib>

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include <QKeySequence>
#include <QMainWindow>
#include <QProxyStyle>
#include <QRegularExpression>
#include <QStyle>
#include <QTextStream>
#include <QtDebug>

#include <functional>

#include "qtermwidget.h"

#ifdef __APPLE__
const QFont DEFAULT_FONT = QFont("Menlo", 10);
#else
const QFont DEFAULT_FONT = QFont("Fira Mono", 10);
#endif


class BQStyle : public QProxyStyle
{
  public:
    int styleHint(StyleHint sh, const QStyleOption* opt,
      const QWidget* w, QStyleHintReturn* hret) const override
    {
      if (sh == SH_ScrollBar_Transient) return true;
      return QProxyStyle::styleHint(sh, opt, w, hret);
    }
};


void add_shortcut(
  QWidget* parent, QKeySequence const& key, std::function<void()> cb)
{
  QAction* action = new QAction(parent);
  action->setShortcut(key);
  QObject::connect(action, &QAction::triggered, cb);
  parent->addAction(action);
}


void load_config(QTermWidget* console)
{
  QFile f {QDir::homePath() + "/.config/berm/berm.conf"};
  if (f.open(QIODevice::ReadOnly)) {
    QTextStream stream (&f);
    QString line;
    while (1) {
      line = stream.readLine();
      if (line.isNull()) break;
      auto tokens = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
      if (tokens[0] == "color") {
        console->setColorScheme(tokens[1]);
      }
    }
  }
  f.close();
}


static int _console_cnt;
static QTermWidget* _console_active = nullptr;

void new_console(QApplication* app)
{
  QTermWidget* console = new QTermWidget(0); // startnow = 0
  _console_cnt += 1;

  console->setTerminalSizeHint(false);
  console->setColorScheme("Nord");
  console->setKeyBindings("default");
  console->setMotionAfterPasting(2); // scroll to end
  console->setTerminalFont(DEFAULT_FONT);
  console->setBidiEnabled(false);
  console->setWorkingDirectory("$HOME");

#ifdef __APPLE__
  console->setScrollBarPosition(
    QTermWidget::ScrollBarPosition::ScrollBarRight);
#endif

  auto close_console = [=]() {
    console->close();
    console->deleteLater();
    if (--_console_cnt == 0) app->quit();
  };

#define CMD(key, body) \
  add_shortcut(console, QKeySequence(key), [=]{ body; })

  CMD( "Meta+C", console->copyClipboard() );
  CMD( "Meta+V", console->pasteClipboard() );
  CMD( "Meta+F", console->toggleShowSearchBar() );
  CMD( "Meta++", console->zoomIn() );
  CMD( "Meta+-", console->zoomOut() );
  CMD( "Meta+N", new_console(app) );
  CMD( "Meta+W", close_console() );

  QObject::connect(
    console, &QTermWidget::urlActivated, [](const QUrl& url, bool) {
      if (QApplication::keyboardModifiers()
          & (Qt::ControlModifier | Qt::MetaModifier))
        QDesktopServices::openUrl(url);
    });
  QObject::connect(console, &QTermWidget::finished, close_console);
  QObject::connect(console, &QTermWidget::termGetFocus,
    [=]() { _console_active = console; });
  if (!_console_active) _console_active = console;

  load_config(console);

//   console->display()->setLineSpacing(10);

  console->startShellProgram();
  console->show();

  CMD("Meta+J", console->sendText("\x1b" "[17~"));
  CMD("Meta+K", console->sendText("\x1b" "[18~"));
  CMD("Meta+H", console->sendText("\x1b" "[19~"));
  CMD("Meta+L", console->sendText("\x1b" "[20~"));
}

int main(int argc, char* argv[])
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_MacDontSwapCtrlAndMeta, true);
  QApplication* app = new QApplication(argc, argv);

#ifdef __APPLE__
  BQStyle* style = new BQStyle;
  style->setBaseStyle(app->style());
  app->setStyle(style);
#endif

  new_console(app);

  return app->exec();
}
