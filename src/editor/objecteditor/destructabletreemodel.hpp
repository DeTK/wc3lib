/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_DESTRUCTABLETREEMODEL_HPP
#define WC3LIB_EDITOR_DESTRUCTABLETREEMODEL_HPP

#include "objecttreemodel.hpp"

namespace wc3lib
{

namespace editor
{

class WarcraftIIIShared;

class DestructableTreeModel : public ObjectTreeModel
{
	public:
		DestructableTreeModel(MpqPriorityList *source, QWidget *window, QObject *parent = 0);

		virtual ObjectTreeItem* createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString& originalObjectId, const QString& customObjectId) override;

	protected:
		virtual QModelIndex itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId) override;

		QModelIndex objectsIndex(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId);
		QModelIndex categoryIndex(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId);

		void createObjects(WarcraftIIIShared *shared);
		void createCategories(WarcraftIIIShared *shared, int row, QModelIndex parent);

		/**
		 * Stores the corresponding row indices with their field values as key.
		 */
		QHash<QString, int> m_categoryRows;
};

}

}

#endif // WC3LIB_EDITOR_DESTRUCTABLETREEMODEL_HPP
