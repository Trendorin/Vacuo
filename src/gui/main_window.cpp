#include "main_window.hpp"

#include "language_manager.hpp"
#include "settings_dialog.hpp"
#include "translation_utils.hpp"

#include "vacuo/core/cleaner.hpp"
#include "vacuo/core/rule_catalog.hpp"
#include "vacuo/core/scanner.hpp"
#include "vacuo/core/system_detector.hpp"
#include "vacuo/core/types.hpp"
#include "vacuo/version.hpp"

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QIcon>
#include <QItemSelectionModel>
#include <QKeySequence>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QShortcut>
#include <QStatusBar>
#include <QStyle>
#include <QTableView>
#include <QTextBrowser>
#include <QTimer>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

#include <algorithm>
#include <sstream>

MainWindow::MainWindow(LanguageManager* languages, QWidget* parent)
    : QMainWindow(parent),
      languages_(languages),
      system_(vacuo::SystemDetector::detect()),
      rules_(vacuo::RuleCatalog::build(system_)) {
    Q_ASSERT(languages_ != nullptr);
    setupUi();
    setupMenus();
    setSystemSummary();

    connect(&scanWatcher_, &QFutureWatcher<vacuo::ScanReport>::finished,
            this, &MainWindow::scanFinished);
    connect(&cleanWatcher_, &QFutureWatcher<vacuo::CleanReport>::finished,
            this, &MainWindow::cleanFinished);
    connect(model_, &QAbstractItemModel::dataChanged,
            this, &MainWindow::updateSelectionSummary);
    connect(table_->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &MainWindow::updateDetails);

    QSettings settings;
    restoreGeometry(settings.value(QStringLiteral("window/geometry")).toByteArray());
    QTimer::singleShot(0, this, &MainWindow::startScan);
}

MainWindow::~MainWindow() {
    cancelled_.store(true);
    scanWatcher_.waitForFinished();
    cleanWatcher_.waitForFinished();
}

void MainWindow::changeEvent(QEvent* event) {
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
}

void MainWindow::setupUi() {
    setWindowTitle(tr("Vacuo — Linux cache cleaner"));
    setMinimumSize(820, 560);
    resize(1080, 720);
    setWindowIcon(QIcon::fromTheme(QStringLiteral("io.github.trendorin.Vacuo"),
                                   style()->standardIcon(QStyle::SP_TrashIcon)));

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(12);

    auto* headingRow = new QHBoxLayout;
    auto* headingColumn = new QVBoxLayout;
    auto* title = new QLabel(tr("Vacuo"), central);
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize() + 7);
    titleFont.setWeight(QFont::DemiBold);
    title->setFont(titleFont);
    title->setAccessibleName(tr("Vacuo application title"));
    subtitleLabel_ = new QLabel(central);
    subtitleLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    headingColumn->addWidget(title);
    headingColumn->addWidget(subtitleLabel_);
    headingRow->addLayout(headingColumn);
    headingRow->addStretch();
    systemLabel_ = new QLabel(central);
    systemLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    systemLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    headingRow->addWidget(systemLabel_);
    layout->addLayout(headingRow);

    auto* separator = new QFrame(central);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);

    summaryLabel_ = new QLabel(tr("Scanning the system…"), central);
    summaryLabel_->setWordWrap(true);
    summaryLabel_->setAccessibleName(tr("Scan summary"));
    layout->addWidget(summaryLabel_);

    model_ = new ScanModel(this);
    table_ = new QTableView(central);
    table_->setModel(model_);
    table_->setAlternatingRowColors(true);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setSortingEnabled(false);
    table_->setShowGrid(false);
    table_->verticalHeader()->setVisible(false);
    table_->horizontalHeader()->setStretchLastSection(false);
    table_->horizontalHeader()->setSectionResizeMode(ScanModel::Selected, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(ScanModel::Category, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(ScanModel::Scope, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(ScanModel::Risk, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(ScanModel::Size, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(ScanModel::Items, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(ScanModel::Status, QHeaderView::ResizeToContents);
    table_->setAccessibleName(tr("Cleanup categories"));
    layout->addWidget(table_, 1);

    detailsGroup_ = new QGroupBox(central);
    auto* detailsLayout = new QVBoxLayout(detailsGroup_);
    details_ = new QTextBrowser(detailsGroup_);
    details_->setOpenExternalLinks(false);
    details_->setFrameShape(QFrame::NoFrame);
    details_->setMaximumHeight(122);
    details_->setAccessibleName(tr("Selected category details"));
    detailsLayout->addWidget(details_);
    layout->addWidget(detailsGroup_);

    auto* footer = new QHBoxLayout;
    selectionLabel_ = new QLabel(tr("Nothing selected"), central);
    selectionLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    footer->addWidget(selectionLabel_);
    progress_ = new QProgressBar(central);
    progress_->setRange(0, 0);
    progress_->setTextVisible(false);
    progress_->setMaximumWidth(180);
    progress_->hide();
    footer->addWidget(progress_);
    footer->addStretch();
    scanButton_ = new QPushButton(tr("Scan again"), central);
    scanButton_->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
    scanButton_->setAccessibleDescription(tr("Scan supported cache locations without changing files"));
    connect(scanButton_, &QPushButton::clicked, this, &MainWindow::startScan);
    footer->addWidget(scanButton_);
    cleanButton_ = new QPushButton(tr("Clean selected"), central);
    cleanButton_->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));
    cleanButton_->setEnabled(false);
    cleanButton_->setDefault(true);
    cleanButton_->setAccessibleDescription(tr("Review and permanently clean selected cache categories"));
    connect(cleanButton_, &QPushButton::clicked, this, &MainWindow::startClean);
    footer->addWidget(cleanButton_);
    layout->addLayout(footer);

    setCentralWidget(central);
    retranslateUi();
}

void MainWindow::setupMenus() {
    fileMenu_ = menuBar()->addMenu(QString());
    rescanAction_ = fileMenu_->addAction(QIcon::fromTheme(QStringLiteral("view-refresh")), QString());
    rescanAction_->setShortcut(QKeySequence::Refresh);
    connect(rescanAction_, &QAction::triggered, this, &MainWindow::startScan);
    fileMenu_->addSeparator();
    quitAction_ = fileMenu_->addAction(QString());
    quitAction_->setShortcut(QKeySequence::Quit);
    connect(quitAction_, &QAction::triggered, this, &QWidget::close);

    editMenu_ = menuBar()->addMenu(QString());
    settingsAction_ = editMenu_->addAction(QString());
    settingsAction_->setShortcut(QKeySequence::Preferences);
    connect(settingsAction_, &QAction::triggered, this, &MainWindow::showSettings);

    helpMenu_ = menuBar()->addMenu(QString());
    securityAction_ = helpMenu_->addAction(QString());
    connect(securityAction_, &QAction::triggered, this, &MainWindow::showSecurityInformation);
    aboutAction_ = helpMenu_->addAction(QString());
    connect(aboutAction_, &QAction::triggered, this, &MainWindow::showAbout);
    retranslateUi();
}

void MainWindow::setSystemSummary() {
    const auto name = QString::fromStdString(system_.prettyName).toHtmlEscaped();
    const auto details = tr("%1 · %2 · %3")
                             .arg(QString::fromStdString(system_.desktop).toHtmlEscaped(),
                                  QString::fromStdString(system_.sessionType).toHtmlEscaped(),
                                  QString::fromStdString(system_.homeFileSystem).toHtmlEscaped());
    systemLabel_->setText(QStringLiteral("<b>%1</b><br><span>%2</span>").arg(name, details));
    systemLabel_->setToolTip(tr("Detected package manager: %1")
                                 .arg(QString::fromStdString(system_.packageManager)));
}

void MainWindow::setBusy(const bool busy, const QString& message) {
    scanButton_->setEnabled(!busy);
    cleanButton_->setEnabled(!busy && !model_->selectedRuleIds().empty());
    table_->setEnabled(!busy);
    progress_->setVisible(busy);
    if (!message.isEmpty()) {
        statusBar()->showMessage(message);
    } else {
        statusBar()->clearMessage();
    }
}

void MainWindow::startScan() {
    if (scanWatcher_.isRunning() || cleanWatcher_.isRunning()) {
        return;
    }
    cancelled_.store(false);
    setBusy(true, tr("Scanning supported cache locations…"));
    summaryLabel_->setText(tr("Scanning is read-only. No files are changed."));
    const auto system = system_;
    const auto rules = rules_;
    scanWatcher_.setFuture(QtConcurrent::run([this, system, rules] {
        return vacuo::Scanner{}.scan(system, rules, &cancelled_);
    }));
}

void MainWindow::scanFinished() {
    if (cancelled_.load()) {
        setBusy(false);
        return;
    }
    model_->setReport(scanWatcher_.result());
    updateScanSummary();
    setBusy(false);
    if (model_->rowCount() > 0) {
        table_->selectRow(0);
    }
    updateSelectionSummary();
    updateDetails();
}

void MainWindow::updateSelectionSummary() {
    const auto ids = model_->selectedRuleIds();
    selectionLabel_->setText(ids.empty()
                                 ? tr("Nothing selected")
                                 : tr("Selected: %1 in %2 categories")
                                       .arg(QString::fromStdString(vacuo::formatBytes(model_->selectedBytes())))
                                       .arg(static_cast<qulonglong>(ids.size())));
    cleanButton_->setEnabled(!scanWatcher_.isRunning() && !cleanWatcher_.isRunning() && !ids.empty());
}

void MainWindow::updateDetails() {
    const auto* result = model_->resultAt(table_->currentIndex().row());
    if (result == nullptr) {
        details_->setHtml(tr("Select a category to see exactly what Vacuo will clean."));
        return;
    }
    QString paths;
    for (const auto& target : result->rule.targets) {
        if (!paths.isEmpty()) {
            paths += QStringLiteral("<br>");
        }
        paths += QStringLiteral("<code>%1</code>")
                     .arg(QString::fromStdString(target.path.string()).toHtmlEscaped());
    }
    const auto warning = result->rule.warning.empty()
        ? QString{}
        : QStringLiteral("<br><b>%1</b> %2")
              .arg(tr("Note:"), translatedVacuoText(result->rule.warning).toHtmlEscaped());
    details_->setHtml(QStringLiteral("<b>%1</b><br>%2%3<br><span>%4</span>")
                          .arg(translatedVacuoText(result->rule.title).toHtmlEscaped(),
                               translatedVacuoText(result->rule.description).toHtmlEscaped(),
                               warning,
                               paths));
}

void MainWindow::startClean() {
    if (scanWatcher_.isRunning() || cleanWatcher_.isRunning()) {
        return;
    }
    const auto selected = model_->selectedRuleIds();
    if (selected.empty()) {
        return;
    }

    bool includesSystem = false;
    bool includesElevated = false;
    for (const auto& id : selected) {
        const auto* rule = vacuo::RuleCatalog::find(rules_, id);
        includesSystem = includesSystem || (rule != nullptr && rule->scope == vacuo::RuleScope::System);
        includesElevated = includesElevated || (rule != nullptr && rule->risk == vacuo::RiskLevel::Elevated);
    }
    QString question = tr("Permanently clean %1 from %2 selected categories?")
                           .arg(QString::fromStdString(vacuo::formatBytes(model_->selectedBytes())))
                           .arg(static_cast<qulonglong>(selected.size()));
    if (includesSystem) {
        question += QStringLiteral("\n\n") +
                    tr("System categories require a PolicyKit authentication prompt.");
    }
    if (includesElevated) {
        question += QStringLiteral("\n\n") +
                    tr("At least one selected category is not reversible.");
    }
    QMessageBox confirmation(QMessageBox::Warning,
                             tr("Confirm cleanup"),
                             question,
                             QMessageBox::NoButton,
                             this);
    auto* confirmButton = confirmation.addButton(tr("Clean"), QMessageBox::AcceptRole);
    auto* cancelButton = confirmation.addButton(tr("Cancel"), QMessageBox::RejectRole);
    confirmation.setDefaultButton(cancelButton);
    confirmation.exec();
    if (confirmation.clickedButton() != confirmButton) {
        return;
    }

    setBusy(true, tr("Cleaning selected categories…"));
    const auto system = system_;
    const auto rules = rules_;
    cleanWatcher_.setFuture(QtConcurrent::run([system, rules, selected] {
        return vacuo::Cleaner(system, rules).execute(selected);
    }));
}

void MainWindow::cleanFinished() {
    const auto report = cleanWatcher_.result();
    setBusy(false);
    QString details;
    for (const auto& action : report.actions) {
        if (!action.success) {
            details += QStringLiteral("%1: %2\n")
                           .arg(QString::fromStdString(action.ruleId),
                                translatedVacuoText(action.message));
        }
    }
    if (report.allSucceeded()) {
        QMessageBox::information(this,
                                 tr("Cleanup complete"),
                                 tr("Freed %1 and removed %2 entries.")
                                     .arg(QString::fromStdString(vacuo::formatBytes(report.bytesFreed)))
                                     .arg(report.itemsRemoved));
    } else {
        QMessageBox message(QMessageBox::Warning,
                            tr("Cleanup completed with issues"),
                            tr("Freed %1. Some categories could not be completed.")
                                .arg(QString::fromStdString(vacuo::formatBytes(report.bytesFreed))),
                            QMessageBox::Ok,
                            this);
        message.setDetailedText(details.trimmed());
        message.exec();
    }
    startScan();
}

void MainWindow::showSecurityInformation() {
    QMessageBox::information(
        this,
        tr("Vacuo security model"),
        tr("Vacuo never follows symbolic links while cleaning. User targets come from a compiled allow-list. "
           "The graphical application never runs as root; fixed system actions are delegated to a small "
           "PolicyKit helper that accepts action identifiers, not commands or paths. Vacuo has no telemetry "
           "and makes no network requests."));
}

void MainWindow::showAbout() {
    const auto version = QString::fromLatin1(vacuo::kVersion.data(),
                                              static_cast<qsizetype>(vacuo::kVersion.size()));
    QMessageBox::about(
        this,
        tr("About Vacuo"),
        tr("<h3>Vacuo %1</h3>"
           "<p>A native C++20 and Qt 6 Widgets cache cleaner for Linux.</p>"
           "<p>Copyright © 2026 Trendorin. Licensed under GPL-3.0-or-later.</p>")
            .arg(version));
}

void MainWindow::showSettings() {
    QSettings settings;
    settings.beginGroup(QStringLiteral("General"));
    const QString currentLanguage =
        settings.value(QStringLiteral("language"), QStringLiteral("system")).toString();
    settings.endGroup();

    SettingsDialog dialog(currentLanguage, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const QString language = LanguageManager::normalizedLanguage(dialog.language());
    settings.beginGroup(QStringLiteral("General"));
    settings.setValue(QStringLiteral("language"), language);
    settings.endGroup();
    settings.sync();
    (void)languages_->applyLanguage(language);
}

void MainWindow::updateScanSummary() {
    if (model_ == nullptr || summaryLabel_ == nullptr) {
        return;
    }
    const auto& report = model_->report();
    if (report.results.empty()) {
        summaryLabel_->setText(tr("Scanning the system…"));
        return;
    }
    summaryLabel_->setText(
        tr("Found %1 across %2 removable entries. Review each category before cleaning.")
            .arg(QString::fromStdString(vacuo::formatBytes(report.totalBytes)))
            .arg(report.totalItems));
}

void MainWindow::retranslateUi() {
    setWindowTitle(tr("Vacuo — Linux cache cleaner"));
    if (subtitleLabel_ != nullptr) {
        subtitleLabel_->setText(tr("Native Linux cache control"));
        summaryLabel_->setAccessibleName(tr("Scan summary"));
        table_->setAccessibleName(tr("Cleanup categories"));
        detailsGroup_->setTitle(tr("Details"));
        details_->setAccessibleName(tr("Selected category details"));
        scanButton_->setText(tr("Scan again"));
        scanButton_->setAccessibleDescription(
            tr("Scan supported cache locations without changing files"));
        cleanButton_->setText(tr("Clean selected"));
        cleanButton_->setAccessibleDescription(
            tr("Review and permanently clean selected cache categories"));
        setSystemSummary();
        model_->retranslate();
        updateScanSummary();
        updateSelectionSummary();
        updateDetails();
    }
    if (fileMenu_ != nullptr) {
        fileMenu_->setTitle(tr("&File"));
        editMenu_->setTitle(tr("&Edit"));
        helpMenu_->setTitle(tr("&Help"));
        rescanAction_->setText(tr("&Scan again"));
        quitAction_->setText(tr("&Quit"));
        settingsAction_->setText(tr("Settings…"));
        securityAction_->setText(tr("Security model"));
        aboutAction_->setText(tr("About Vacuo"));
    }
    if (cleanWatcher_.isRunning()) {
        statusBar()->showMessage(tr("Cleaning selected categories…"));
    } else if (scanWatcher_.isRunning()) {
        statusBar()->showMessage(tr("Scanning supported cache locations…"));
        summaryLabel_->setText(tr("Scanning is read-only. No files are changed."));
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (scanWatcher_.isRunning() || cleanWatcher_.isRunning()) {
        QMessageBox::information(this, tr("Vacuo is busy"), tr("Wait for the current operation to finish."));
        event->ignore();
        return;
    }
    QSettings settings;
    settings.setValue(QStringLiteral("window/geometry"), saveGeometry());
    QMainWindow::closeEvent(event);
}
