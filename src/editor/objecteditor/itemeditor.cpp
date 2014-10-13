/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#include <QtGui>

#include <KMessageBox>
#include <KFileDialog>

#include "itemeditor.hpp"
#include "objecttreeview.hpp"
#include "objecttreeitem.hpp"
#include "itemtreemodel.hpp"
#include "../metadata.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

ItemEditor::ItemEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, new ItemData(source, parent), parent, f)
{
	setupUi();
}

ItemEditor::~ItemEditor()
{
}

ObjectTreeModel* ItemEditor::createTreeModel()
{
	return new ItemTreeModel(this->source(), this);
}

void ItemEditor::onSwitchToMap(Map *map)
{
}

void ItemEditor::onNewObject()
{
}

}

}