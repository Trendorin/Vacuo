#include "main_window.hpp"

#include "vacuo/core/cleaner.hpp"
#include "vacuo/version.hpp"

#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <QStyle>

int main(int argc, char* argv[]) {
    QApplication::setOrganizationName(QStringLiteral("Trendorin"));
    QApplication::setOrganizationDomain(QStringLiteral("io.github.trendorin"));
    QApplication::setApplicationName(QStringLiteral("Vacuo"));
    QApplication::setApplicationDisplayName(QStringLiteral("Vacuo"));
    QApplication::setApplicationVersion(QString::fromLatin1(vacuo::kVersion.data(),
                                                             static_cast<qsizetype>(vacuo::kVersion.size())));
    QApplication::setDesktopFileName(QStringLiteral("io.github.trendorin.Vacuo"));

    QApplication application(argc, argv);
    application.setWindowIcon(QIcon::fromTheme(
        QStringLiteral("io.github.trendorin.Vacuo"),
        application.style()->standardIcon(QStyle::SP_TrashIcon)));

    if (vacuo::Cleaner::isRunningAsRoot()) {
        QMessageBox::critical(
            nullptr,
            QObject::tr("Vacuo cannot run as root"),
            QObject::tr("Run Vacuo as your normal user. System cleanup is requested separately through PolicyKit."));
        return 77;
    }

    MainWindow window;
    window.show();
    return application.exec();
}
