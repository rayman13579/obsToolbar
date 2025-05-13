/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <plugin-support.h>
#include <qobject.h>
#include <obs-frontend-api.h>
#include <filter-dock.hpp>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

extern struct obs_source_info my_source;

bool obs_module_load(void)
{
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);
	QWidget *main_window = (QWidget *)obs_frontend_get_main_window();
	FilterDock *dock = new FilterDock(main_window);
	obs_frontend_add_dock_by_id("filter-dock", "Filters", dock);
	return true;
}

void obs_module_unload(void)
{
	obs_frontend_remove_dock("filter-dock");
	obs_log(LOG_INFO, "plugin unloaded");
}
