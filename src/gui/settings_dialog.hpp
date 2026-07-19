#pragma once

#include <QDialog>
#include <QString>

class QComboBox;

class SettingsDialog final : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(const QString& language, QWidget* parent = nullptr);

    [[nodiscard]] QString language() const;

private:
    QComboBox* language_ = nullptr;
};
