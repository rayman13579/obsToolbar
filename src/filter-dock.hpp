#pragma once

#include <obs.hpp>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <QLabel>
#include <map>
#include <set>

class FilterDock : public QWidget {
	Q_OBJECT

public:
	explicit FilterDock(QWidget *parent = nullptr);
	~FilterDock();

private:
    QListWidget *list = nullptr;
    std::map<obs_source_t *, std::set<obs_source_t *>> *selectedSources = new std::map<obs_source_t *, std::set<obs_source_t *>>();
    OBSSignal sourceSelectSignal;
    OBSSignal sourceDeselectSignal;
	void refreshFilters();
	void addFilters(obs_source_t *source, bool multiSelect);
    void addVisiblityIconToFilter(QListWidgetItem *item, obs_source_t *filter);
};