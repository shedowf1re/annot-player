// filteredtableview.cc
// 11/17/2011

#include "filteredtableview.h"
#include "tr.h"
#include "stylesheet.h"
#include "uistyle.h"
#include <QtGui>

// - Constructions -

FilteredTableView::FilteredTableView(QStandardItemModel *sourceModel, QSortFilterProxyModel *proxyModel, QWidget *parent)
  : Base(parent), sourceModel_(sourceModel), proxyModel_(proxyModel)
{
  Q_ASSERT(sourceModel_);
  Q_ASSERT(proxyModel_);
  setContentsMargins(0, 0, 0, 0);

  //setWindowTitle(tr("Process view"));
  //UiStyle::globalInstance()->setWindowStyle(this);

  // Create models

  createLayout();

  // Set initial states
  //proxyView_->sortByColumn(HD_Status, Qt::DescendingOrder);
  //filterColumnComboBox_->setCurrentIndex(HD_Name);
  proxyView_->sortByColumn(0);
  filterColumnComboBox_->setCurrentIndex(0);
  filterPatternEdit_->setFocus();
}

void
FilteredTableView::createLayout()
{
  UiStyle *ui = UiStyle::globalInstance();

  //sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  //setProcessHeaderData(sourceModel_);

  //proxyModel_ = new QSortFilterProxyModel; {
  //  proxyModel_->setSourceModel(sourceModel_);
  //  proxyModel_->setDynamicSortFilter(true);
  //  proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  //}

  // Create widgets

  proxyView_ = new QTreeView; {
    proxyView_->setStyleSheet(SS_TREEVIEW);
    proxyView_->setRootIsDecorated(true);
    proxyView_->setAlternatingRowColors(true);
    proxyView_->setSortingEnabled(true);
    proxyView_->setModel(proxyModel_);
    //proxyView_->setToolTip(tr("Running processes"));
  }

  filterPatternEdit_ = ui->makeComboBox(UiStyle::EditHint, "", TR(T_FILTER_PATTERN));
  QLabel *filterPatternLabel = ui->makeLabel(
        UiStyle::BuddyHint, TR(T_FILTER_PATTERN), filterPatternEdit_);

  filterSyntaxComboBox_ = ui->makeComboBox(UiStyle::ReadOnlyHint, "", TR(T_FILTER_SYNTAX)); {
    filterSyntaxComboBox_->addItem(TR(T_FILTER_REGEX), QRegExp::RegExp);
    filterSyntaxComboBox_->addItem(TR(T_FILTER_WILDCARD), QRegExp::Wildcard);
    filterSyntaxComboBox_->addItem(TR(T_FILTER_FIXED), QRegExp::FixedString);
  }
  QLabel *filterSyntaxLabel = ui->makeLabel(
        UiStyle::BuddyHint, TR(T_FILTER_SYNTAX), filterSyntaxComboBox_);

  filterColumnComboBox_ = ui->makeComboBox(UiStyle::ReadOnlyHint, "", TR(T_FILTER_COLUMN)); {
    for (int i = 0; i < sourceModel_->columnCount(); i++)
      filterColumnComboBox_->addItem(sourceModel_->headerData(i, Qt::Horizontal).toString());
  }
  QLabel *filterColumnLabel = ui->makeLabel(
        UiStyle::BuddyHint, TR(T_FILTER_COLUMN), filterColumnComboBox_);

  countButton_ = ui->makeToolButton(0, "0/0", tr("Count"), this, SLOT(popup()));

  // Set layout

  QGridLayout *layout = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;

    layout->addWidget(proxyView_, r=0, c=0, 1, 3);

    layout->addWidget(filterPatternLabel, ++r, c=0);
    layout->addWidget(filterPatternEdit_, r, ++c, 1, 2);
    layout->addWidget(countButton_, r, ++c, 1, 1, Qt::AlignRight);

    layout->addWidget(filterSyntaxLabel, ++r, c=0);
    layout->addWidget(filterSyntaxComboBox_, r, ++c, 1, 2);

    layout->addWidget(filterColumnLabel, ++r, c=0);
    layout->addWidget(filterColumnComboBox_, r, ++c, 1, 2);

    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);

  // Set up connections
  connect(filterPatternEdit_, SIGNAL(editTextChanged(QString)),
          SLOT(invalidateFilterRegExp()));
  connect(filterPatternEdit_, SIGNAL(editTextChanged(QString)),
          SLOT(invalidateCount()));
  connect(filterSyntaxComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(invalidateFilterRegExp()));
  connect(filterColumnComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(invalidateFilterColumn()));

  connect(proxyView_, SIGNAL(activated(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
  connect(proxyView_, SIGNAL(clicked(QModelIndex)), SIGNAL(currentIndexChanged(QModelIndex)));
}

// - Properties -

QModelIndex
FilteredTableView::currentIndex() const
{ return proxyView_->currentIndex(); }

void
FilteredTableView::removeCurrentRow()
{
  QModelIndex mi = currentIndex();
  if (mi.isValid())
    proxyModel_->removeRow(mi.row());
  invalidateCount();
}

void
FilteredTableView::setCurrentColumn(int col)
{ filterColumnComboBox_->setCurrentIndex(col); }

void
FilteredTableView::sortByColumn(int col, Qt::SortOrder order)
{ proxyView_->sortByColumn(col, order); }

// - Actions -

void
FilteredTableView::popup()
{ filterPatternEdit_->showPopup(); }

void
FilteredTableView::clear()
{
  while (sourceModel_->rowCount())
    sourceModel_->removeRow(0);
  invalidateCount();
}

void
FilteredTableView::invalidateFilterRegExp()
{
  QRegExp::PatternSyntax syntax =
      QRegExp::PatternSyntax(
        filterSyntaxComboBox_->itemData(
          filterSyntaxComboBox_->currentIndex()).toInt());

  QRegExp regExp(filterPatternEdit_->currentText(), Qt::CaseInsensitive, syntax);
  proxyModel_->setFilterRegExp(regExp);
  invalidateCount();
}

void
FilteredTableView::invalidateFilterColumn()
{
  proxyModel_->setFilterKeyColumn(filterColumnComboBox_->currentIndex());
  invalidateCount();
}

void
FilteredTableView::invalidateCount()
{
  int total = sourceModel_->rowCount();
  int count = proxyModel_->rowCount();
  countButton_->setText(
    QString("%1/%2")
      .arg(QString::number(count))
      .arg(QString::number(total))
  );
}

// EOF
