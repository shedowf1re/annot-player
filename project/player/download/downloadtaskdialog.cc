// downloadtaskdialog.cc
// 2/18/2012

#include "downloadtaskdialog.h"
#include "uistyle.h"
#include "defines.h"
#include "logger.h"
#include "tr.h"
#include "stylesheet.h"
#include "util/textview.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

#define DEBUG "downloaddialog"
#include "module/debug/debug.h"

using namespace Logger;

#ifdef Q_OS_MAC
  #define K_CTRL        "cmd"
#else
  #define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

// - Helper -

namespace { // anonymous
  template <typename T>
  inline QList<T>
  uniqueList(const QList<T> &l)
  {
    QList<T> ret;
    foreach (T t, l)
      if (!ret.contains(t))
        ret.append(t);
    return ret;
  }
} // namespace anonymous

// - Construciton -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

DownloadTaskDialog::DownloadTaskDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  UiStyle::globalInstance()->setWindowStyle(this);
  setWindowTitle(tr("Add download URLs"));
  createLayout();

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  QShortcut *saveShortcut = new QShortcut(QKeySequence::Save, this);
  connect(saveShortcut, SIGNAL(activated()), SLOT(add()));

  // Focus
  textView_->setFocus();
}

void
DownloadTaskDialog::createLayout()
{
  textView_ = new TextView;

  urlButton_ = new QtExt::ToolButton; {
    urlButton_->setText(tr("http://www.nicovideo.jp/watch/sm12159572"));
    urlButton_->setStyleSheet(SS_TOOLBUTTON_TEXT_URL);
    urlButton_->setToolTip(tr("Click to paste the URL example"));
    urlButton_->setCheckable(true);
    urlButton_->setChecked(true);
  } connect(urlButton_, SIGNAL(clicked(bool)), SLOT(showExampleUrl()));

  QLabel *urlLabel = new QLabel; {
    urlLabel->setStyleSheet(SS_LABEL);
    urlLabel->setText(TR(T_EXAMPLE) + ":");
    urlLabel->setToolTip(TR(T_EXAMPLE));
    urlLabel->setBuddy(urlButton_);
  }

#define MAKE_BUTTON(_button, _title, _tip, _slot) \
    QToolButton *_button = new QtExt::ToolButton; { \
      _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
      _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
      _button->setText(QString("[ %1 ]").arg(_title)); \
      _button->setToolTip(_tip); \
    } connect(_button, SIGNAL(clicked()), _slot);

  MAKE_BUTTON(addButton, TR(T_ADD), TR(T_ADD) + " [" K_CTRL "+S]", SLOT(add()))
  MAKE_BUTTON(pasteButton, TR(T_PASTE), TR(T_PASTE), SLOT(paste()))
  MAKE_BUTTON(clearButton, TR(T_CLEAR), TR(T_CLEAR), SLOT(clear()))

  addButton->setStyleSheet(SS_TOOLBUTTON_TEXT_HIGHLIGHT);

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    rows->addLayout(header);
    rows->addWidget(textView_);
    rows->addLayout(footer);

    header->addWidget(urlLabel);
    header->addWidget(urlButton_);

    footer->addWidget(clearButton);
    footer->addWidget(pasteButton);
    footer->addStretch();
    footer->addWidget(addButton);

    // l, t, r, b
    int patch = 0;
    if (!UiStyle::isAeroAvailable())
      patch = 9;
    header->setContentsMargins(0, 0, 0, 0);
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(9, patch, 9, patch);
  } setLayout(rows);

#undef MAKE_BUTTON
}

void
DownloadTaskDialog::setText(const QString &urls)
{ textView_->setText(urls); }

void
DownloadTaskDialog::addText(const QString &urls)
{ textView_->append(urls); }

// - Actions -

void
DownloadTaskDialog::add()
{
  hide();
  QString text = textView_->toPlainText();
  QStringList urls;
  foreach (QString t, text.split('\n', QString::SkipEmptyParts)) {
    t = t.trimmed();
    if (t.contains("://"))
      urls.append(t);
  }
  bool batch = false;
  if (!urls.isEmpty()) {
    urls =::uniqueList(urls);
    DOUT("urls =" << urls);
    emit urlsAdded(urls, batch);
  } else if (!text.isEmpty())
    warn(tr("invalid URLs") + ": " + text);
}

void
DownloadTaskDialog::paste()
{
  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard) {
    QString t = clipboard->text().trimmed();
    textView_->append(t);
  }
}

void
DownloadTaskDialog::showExampleUrl()
{ setText(urlButton_->text()); }

void
DownloadTaskDialog::clear()
{ textView_->clear(); }

// EOF
