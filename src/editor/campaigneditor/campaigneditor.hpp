/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef WC3LIB_EDITOR_CAMPAIGNEDITOR_HPP
#define WC3LIB_EDITOR_CAMPAIGNEDITOR_HPP

#include <kdemacros.h>
#include <KUrl>

#include "../module.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;

/**
 * The campaign editor allows you to edit exactly one single campaign at the same time.
 */
class KDE_EXPORT CampaignEditor : public Module
{
	Q_OBJECT

	public:
		CampaignEditor(MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);

	public slots:
		void openCampaign(const KUrl &url);
		void closeCampaign();

	protected:
		virtual void createFileActions(QMenu *menu) override;
		virtual void createEditActions(QMenu *menu) override;
		virtual void createMenus(QMenuBar *menuBar) override;
		virtual void createWindowsActions(WindowsMenu *menu) override;
		virtual void createToolButtons(ModuleToolBar *toolBar) override;
		virtual SettingsInterface* settings() override;
		virtual void onSwitchToMap(Map *map) override;
		virtual QString actionName() const override;
		virtual QIcon icon() override;
};

inline QString CampaignEditor::actionName() const
{
	return "campaigneditor";
}

}

}

#endif
