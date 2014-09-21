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

#ifndef WC3LIB_EDITOR_OBJECTTREEMODEL_HPP
#define WC3LIB_EDITOR_OBJECTTREEMODEL_HPP

#include <QWidget>
#include <QAbstractItemModel>

#include "objecttreeitem.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class ObjectData;

/**
 * \brief Abstract tree model for object data.
 *
 * For implementing actual object data in a model you have to derive from this class and to reimplement \ref load()
 * which should fill the model.
 *
 * \sa ObjectData
 *
 * \ingroup objectdata
 */
class ObjectTreeModel : public QAbstractItemModel
{
	Q_OBJECT

	public:

		/**
		 * \brief Stores the two object IDs. The original as well as the custom.
		 */
		typedef QPair<QString, QString> ObjectId;
		/**
		 * All items are hashed by the raw data name of the object.
		 */
		typedef QHash<ObjectId, ObjectTreeItem*> Items;

		ObjectTreeModel(QObject *parent = 0);
		virtual ~ObjectTreeModel();

		void insertTopLevelItem(ObjectTreeItem *item);
		void insertStandardItem(ObjectTreeItem *item);
		void insertCustomItem(ObjectTreeItem *item);
		Items& standardItems();
		Items& customItems();

		/**
		 * \return Returns all items which are folders.
		 * \note This function has O(n) complexity and should not be called oftenly.
		 */
		ObjectTreeItem::Children folders();

		/**
		 * Clears all items which are under the "Custom ..." entry.
		 * \note This member function removes their entries from the hash and deletes the items.
		 */
		void clearCustomItems();

		/**
		 * \return Returns the corresponding item of the object with both IDs.
		 */
		ObjectTreeItem* item(const QString &originalObjectId, const QString &customObjectId);

		ObjectTreeItem* item(const QModelIndex &index) const;

		virtual QVariant data(const QModelIndex &index, int role) const override;
		virtual int columnCount(const QModelIndex &parent) const override;
		virtual int rowCount(const QModelIndex &parent) const override;
		virtual QModelIndex parent(const QModelIndex &index) const override;
		virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

		virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

		/**
		 * Loads all standard objects from \p objectData and fills the tree model.
		 * The object data contains standard objects as well as custom objects.
		 *
		 * The default implementation connects signals of changing data from \p objectData with slots that modify the data in the model.
		 * It should be called when loading the data as well.
		 */
		virtual void load(MpqPriorityList *source, ObjectData *objectData, QWidget *window);
		/**
		 * Creates a new item for an object.
		 * This member functions is required whenever new objects are added to the corresponding object data.
		 * It also can be used in \ref load() to create the standard object items.
		 *
		 * The function should use beginInsertRows() and endInsertRows() to emit all signals required to recognize the insertion.
		 */
		virtual ObjectTreeItem* createItem(MpqPriorityList *source, ObjectData *objectData, QWidget *window, const QString &originalObjectId, const QString &customObjectId) = 0;

		int topLevelRow(ObjectTreeItem *item) const;

	protected:
		virtual ObjectTreeItem* itemParent(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId) = 0;

	private:
		Items m_standardItems;
		Items m_customItems;

		ObjectTreeItem::Children m_topLevelItems;

	private slots:
		void createObject(const QString &originalObjectId, const QString &customObjectId);
		void removeObject(const QString &originalObjectId, const QString &customObjectId);
		void resetObject(const QString &originalObjectId, const QString &customObjectId);
		void modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);
};

inline void ObjectTreeModel::insertTopLevelItem(ObjectTreeItem* item)
{
	this->m_topLevelItems.append(item);
}

inline void ObjectTreeModel::insertStandardItem(ObjectTreeItem *item)
{
	const ObjectId objectId(item->originalObjectId(), item->customObjectId());
	this->m_standardItems.insert(objectId, item);
}

inline void ObjectTreeModel::insertCustomItem(ObjectTreeItem *item)
{
	const ObjectId objectId(item->originalObjectId(), item->customObjectId());
	this->m_customItems.insert(objectId, item);
}

inline ObjectTreeModel::Items& ObjectTreeModel::standardItems()
{
	return this->m_standardItems;
}

inline ObjectTreeModel::Items& ObjectTreeModel::customItems()
{
	return this->m_customItems;
}

inline ObjectTreeItem* ObjectTreeModel::item(const QString &originalObjectId, const QString &customObjectId)
{
	const ObjectId objectId(originalObjectId, customObjectId);
	Items::iterator iterator = this->m_standardItems.find(objectId);

	if (iterator != this->m_standardItems.end())
	{
		return iterator.value();
	}

	iterator = this->m_customItems.find(objectId);

	if (iterator != this->m_customItems.end())
	{
		return iterator.value();
	}

	return 0;
}

inline int ObjectTreeModel::topLevelRow(ObjectTreeItem* item) const
{
	return this->m_topLevelItems.indexOf(item);
}

}

}

#endif // WC3LIB_EDITOR_OBJECTTREEMODEL_HPP
