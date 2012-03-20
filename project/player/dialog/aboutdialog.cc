// aboutdialog.cc
// 11/13/2011

#include "aboutdialog.h"
#include "uistyle.h"
#include "defines.h"
#include "tr.h"
#include "module/qtext/htmltag.h"
#include <QtGui>

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_WS_MAC
  #define WINDOW_FLAGS ( \
    Qt::FramelessWindowHint | \
    WINDOW_FLAGS_BASE )
#else
  #define WINDOW_FLAGS ( \
    Qt::WindowTitleHint | \
    WINDOW_FLAGS_BASE )
#endif // Q_WS_MAC

// - Constructions -

#ifdef Q_WS_MAC
  #define WINDOW_SIZE   QSize(300, 200)
#else
  #define WINDOW_SIZE   QSize(270, 160)
#endif // Q_WS_MAC

QString
AboutDialog::text()
{
  static const char *ret =
    HTML_CENTER_OPEN()
      HTML_STYLE_OPEN(color:purple)
        HTML_BR()

        G_ORGANIZATION " "
        G_APPLICATION " "
        G_VERSION
        HTML_BR()

        HTML_EM_OPEN()
          G_LICENSE
        HTML_EM_CLOSE()
        HTML_BR()

        HTML_BR()

        HTML_A_OPEN("dummy")
          G_HOMEPAGE      HTML_BR()
          G_UPDATEPAGE    HTML_BR()
          G_EMAIL         HTML_BR()
        HTML_A_CLOSE()

      HTML_STYLE_CLOSE()
    HTML_CENTER_CLOSE()
  ;
  return ret;
}

AboutDialog::AboutDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  UiStyle::globalInstance()->setWindowStyle(this);
  setWindowTitle(TR(T_TITLE_ABOUT));

  // Components

  QToolButton *okButton = UiStyle::globalInstance()->makeToolButton(
       UiStyle::PushHint, TR(T_OK), this, SLOT(ok()));

  textEdit_ = UiStyle::globalInstance()->makeTextEdit(
      UiStyle::ReadOnlyHint, TR(T_TITLE_ABOUT));
  textEdit_->setHtml(text());

  // Layout

  QVBoxLayout *col = new QVBoxLayout; {
    col->addWidget(textEdit_);
    col->addWidget(okButton, 0, Qt::AlignHCenter); // stretch = 0
  } setLayout(col);

  // l, t, r, b
  int patch = 0;
  if (!UiStyle::isAeroAvailable())
    patch = 4;
  col->setContentsMargins(patch, patch, patch, 0);
  setContentsMargins(4, 4, 4, patch);

  resize(WINDOW_SIZE);

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  // Focus
  okButton->setFocus();
}

// - Slots -

void
AboutDialog::ok()
{ hide(); }

// EOF
