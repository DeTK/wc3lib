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

#ifndef WC3LIB_EDITOR_OBJECTTABLEMODEL_HPP
#define WC3LIB_EDITOR_OBJECTTABLEMODEL_HPP

#include <QVector>

#include <QAbstractItemModel>

namespace wc3lib
{

namespace editor
{

class ObjectData;

class ObjectTableModel : public QAbstractItemModel
{
	Q_OBJECT

	public:
		typedef QHash<int, QString> ItemsByRow;
		/**
		 * Stores the row for the corresponding field ID.
		 */
		typedef QHash<QString, int> ItemsByField;

		explicit ObjectTableModel(QObject* parent = 0);

		virtual QVariant data(const QModelIndex& index, int role) const override;
		virtual int columnCount(const QModelIndex& parent) const override;
		virtual int rowCount(const QModelIndex& parent) const override;
		virtual QModelIndex parent(const QModelIndex& child) const override;
		virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

		void load(ObjectData *objectData, const QString &originalObjectId, const QString &customObjectId);

		ObjectData* objectData() const;
		const QString& originalObjectId() const;
		const QString& customObjectId() const;

	private:
		ItemsByRow m_itemsByRow;
		ItemsByField m_itemsByField;
		ObjectData *m_objectData;
		QString m_originalObjectId;
		QString m_customObjectId;

	private slots:
		void modifyField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);
		void resetField(const QString &originalObjectId, const QString &customObjectId, const QString &fieldId);
};

inline ObjectData* ObjectTableModel::objectData() const
{
	return this->m_objectData;
}

inline const QString& ObjectTableModel::originalObjectId() const
{
	return this->m_originalObjectId;
}

inline const QString& ObjectTableModel::customObjectId() const
{
	return this->m_customObjectId;
}

}

}

#endif // WC3LIB_EDITOR_OBJECTTABLEMODEL_HPP