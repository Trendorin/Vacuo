#pragma once

#include "scan_model.hpp"
#include "vacuo/core/types.hpp"

#include <QFutureWatcher>
#include <QMainWindow>
#include <QString>

#include <atomic>
#include <vector>

class QLabel;
class QAction;
class QEvent;
class QGroupBox;
class QMenu;
class QProgressBar;
class QPushButton;
class QTableView;
class QTextBrowser;
class LanguageManager;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(LanguageManager* languages, QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void startScan();
    void scanFinished();
    void startClean();
    void cleanFinished();
    void updateDetails();
    void updateSelectionSummary();
    void showAbout();
    void showSecurityInformation();
    void showSettings();

private:
    void setupUi();
    void setupMenus();
    void setBusy(bool busy, const QString& message = {});
    void setSystemSummary();
    void retranslateUi();
    void updateScanSummary();

    LanguageManager* languages_{nullptr};
    vacuo::SystemInfo system_;
    std::vector<vacuo::CleanRule> rules_;
    ScanModel* model_{nullptr};
    QTableView* table_{nullptr};
    QLabel* systemLabel_{nullptr};
    QLabel* subtitleLabel_{nullptr};
    QLabel* summaryLabel_{nullptr};
    QLabel* selectionLabel_{nullptr};
    QTextBrowser* details_{nullptr};
    QGroupBox* detailsGroup_{nullptr};
    QProgressBar* progress_{nullptr};
    QPushButton* scanButton_{nullptr};
    QPushButton* cleanButton_{nullptr};
    QMenu* fileMenu_{nullptr};
    QMenu* editMenu_{nullptr};
    QMenu* helpMenu_{nullptr};
    QAction* rescanAction_{nullptr};
    QAction* quitAction_{nullptr};
    QAction* settingsAction_{nullptr};
    QAction* securityAction_{nullptr};
    QAction* aboutAction_{nullptr};
    QFutureWatcher<vacuo::ScanReport> scanWatcher_;
    QFutureWatcher<vacuo::CleanReport> cleanWatcher_;
    std::atomic_bool cancelled_{false};
};
