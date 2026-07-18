#pragma once

#include "vacuo/core/types.hpp"

#include <QAbstractTableModel>

#include <string>
#include <vector>

class ScanModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        Selected = 0,
        Category,
        Scope,
        Risk,
        Size,
        Items,
        Status,
        ColumnCount,
    };

    explicit ScanModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = {}) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QVariant headerData(int section,
                                      Qt::Orientation orientation,
                                      int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    void setReport(vacuo::ScanReport report);
    [[nodiscard]] const vacuo::ScanReport& report() const;
    [[nodiscard]] const vacuo::ScanResult* resultAt(int row) const;
    [[nodiscard]] std::vector<std::string> selectedRuleIds() const;
    [[nodiscard]] std::uint64_t selectedBytes() const;

private:
    vacuo::ScanReport report_;
    std::vector<bool> selected_;
};
