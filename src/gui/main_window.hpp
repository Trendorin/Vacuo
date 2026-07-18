#pragma once

#include "scan_model.hpp"
#include "vacuo/core/types.hpp"

#include <QFutureWatcher>
#include <QMainWindow>
#include <QString>

#include <atomic>
#include <vector>

class QLabel;
class QProgressBar;
class QPushButton;
class QTableView;
class QTextBrowser;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void startScan();
    void scanFinished();
    void startClean();
    void cleanFinished();
    void updateDetails();
    void updateSelectionSummary();
    void showAbout();
    void showSecurityInformation();

private:
    void setupUi();
    void setupMenus();
    void setBusy(bool busy, const QString& message = {});
    void setSystemSummary();

    vacuo::SystemInfo system_;
    std::vector<vacuo::CleanRule> rules_;
    ScanModel* model_{nullptr};
    QTableView* table_{nullptr};
    QLabel* systemLabel_{nullptr};
    QLabel* summaryLabel_{nullptr};
    QLabel* selectionLabel_{nullptr};
    QTextBrowser* details_{nullptr};
    QProgressBar* progress_{nullptr};
    QPushButton* scanButton_{nullptr};
    QPushButton* cleanButton_{nullptr};
    QFutureWatcher<vacuo::ScanReport> scanWatcher_;
    QFutureWatcher<vacuo::CleanReport> cleanWatcher_;
    std::atomic_bool cancelled_{false};
};
