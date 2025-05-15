#include <filter-dock.hpp>
#include <obs-frontend-api.h>
#include <plugin-support.h>

FilterDock::FilterDock(QWidget *parent) : QWidget(parent)
{
	setLayout(layout);
	list = new QListWidget();
    noSource = new QLabel("No source selected");
    noSource->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list);
    layout->addWidget(noSource);

    setStyleSheet("background: #272A33; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px; border: 1px solid #3C404D; border-top: none;");
    list->setStyleSheet("QListWidget { border-top-left-radius: 0px; border-top-right-radius: 0px; } QListWidget QWidget { border: none; }");

    style()->unpolish(this);
    style()->polish(this);

	obs_frontend_add_event_callback(
		[](enum obs_frontend_event event, void *data) {
			if (OBS_FRONTEND_EVENT_PREVIEW_SCENE_CHANGED != event) {
				return;
			}
			FilterDock *thiss = static_cast<FilterDock *>(data);
			thiss->refreshFilters();

			obs_source_t *scene = obs_frontend_get_current_scene();
			signal_handler *signal_handler = obs_source_get_signal_handler(scene);

			thiss->sourceSelectSignal.Connect(signal_handler, "item_select",
				[](void *data, calldata *sceneItem) {
					FilterDock *thiss = static_cast<FilterDock *>(data);
					OBSSceneItem item =static_cast<obs_sceneitem_t *>(calldata_ptr(sceneItem, "item"));
					obs_source_t *source = obs_sceneitem_get_source(item);
					(*thiss->selectedSources)[obs_frontend_get_current_scene()].push_back(source);
					thiss->refreshFilters();
				},
				thiss);
			thiss->sourceDeselectSignal.Connect(signal_handler, "item_deselect",
				[](void *data, calldata *sceneItem) {
					FilterDock *thiss = static_cast<FilterDock *>(data);
					OBSSceneItem item = static_cast<obs_sceneitem_t *>(calldata_ptr(sceneItem, "item"));
					obs_source_t *source = obs_sceneitem_get_source(item);
					(*thiss->selectedSources)[obs_frontend_get_current_scene()].remove(source);
					thiss->refreshFilters();
				},
				thiss);
		},
		this);
}

FilterDock::~FilterDock()
{
	obs_log(LOG_INFO, "FilterDock destructor called");
}

void FilterDock::refreshFilters()
{
	list->clear();
	std::list<obs_source_t *> sources = (*selectedSources)[obs_frontend_get_current_scene()];
    
    if (sources.empty()) {
        list->hide();
        noSource->show();
        return;
    }
    list->show();
    noSource->hide();
	for (obs_source_t *source : sources) {
		addFilters(source, sources.size() > 1);
	}
}

void FilterDock::addFilters(obs_source_t *source, bool multiSelect)
{
	if (obs_source_filter_count(source) == 0) {
		return;
	}
	if (multiSelect) {
		QListWidgetItem *item = new QListWidgetItem(obs_source_get_name(source));
		item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
		list->addItem(item);
	}
	obs_source_enum_filters(
		source,
		[](obs_source_t *, obs_source_t *filter, void *data) {
			FilterDock *thiss = static_cast<FilterDock *>(data);
			QListWidgetItem *item = new QListWidgetItem();
			thiss->list->addItem(item);
			thiss->addVisiblityIconToFilter(item, filter);
		},
		this);
}

void FilterDock::addVisiblityIconToFilter(QListWidgetItem *item, obs_source_t *filter)
{
	QCheckBox *visibilityIcon = new QCheckBox();
	visibilityIcon->setProperty("class", "checkbox-icon indicator-visibility");
	visibilityIcon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	visibilityIcon->setChecked(obs_source_enabled(filter));
	connect(visibilityIcon, &QCheckBox::toggled, this,
		[this, filter](bool checked) { 
            obs_source_set_enabled(filter, checked); 
    });
	/*not properly working
        filterVisibilitySignal.Connect(obs_source_get_signal_handler(filter), "enable",
		[](void *data, calldata *filter) {
			bool enabled = calldata_bool(filter, "enabled");
			QCheckBox *checkbox = static_cast<QCheckBox *>(data);
			checkbox->setChecked(enabled);
		},
		visibilityIcon);
    */
	const char *filter_name = obs_source_get_name(filter);
	QLabel *label = new QLabel(filter_name);
	label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QHBoxLayout *itemLayout = new QHBoxLayout();
	itemLayout->addWidget(visibilityIcon);
	itemLayout->addWidget(label);
	itemLayout->setContentsMargins(0, 0, 0, 0);

	QWidget *itemWidget = new QWidget();
	itemWidget->setLayout(itemLayout);
	list->setItemWidget(item, itemWidget);
}