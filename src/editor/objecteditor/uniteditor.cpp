/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include "uniteditor.hpp"
#include "objecttreeview.hpp"
#include "objecttreeitem.hpp"
#include "unittreemodel.hpp"
#include "unitselectiondialog.hpp"
#include "iddialog.hpp"
#include "../metadata.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

UnitEditor::UnitEditor(MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : ObjectEditorTab(source, new UnitData(source, parent), parent, f), m_unitSelectionDialog(new UnitSelectionDialog(source, unitData(), this))
{
	setupUi();
}

UnitEditor::~UnitEditor()
{
}

ObjectTreeModel* UnitEditor::createTreeModel()
{
	return new UnitTreeModel(this->source(), this);
}

void UnitEditor::onSwitchToMap(Map *map)
{
}

void UnitEditor::onNewObject()
{
	QItemSelectionModel *selection = this->treeView()->selectionModel();
	qDebug() << "Selection:" << selection;
	QString originalObjectId;

	foreach (QModelIndex index, selection->selectedIndexes())
	{
		const ObjectTreeItem *item = this->treeModel()->item(this->proxyModel()->mapToSource(index));
		originalObjectId = item->originalObjectId();
	}

	if (originalObjectId.isEmpty())
	{  
		originalObjectId = "hpea";
	}

	this->idDialog()->setId(this->unitData()->nextCustomObjectId());

	if (this->idDialog()->exec() == QDialog::Accepted)
	{
		const QString customObjectId = this->idDialog()->id();

		/*
		 * Custom ID is already in use, so ask the user if he wants to overwrite an existing object.
		 */
		if (this->objectData()->isObjectModified(originalObjectId, customObjectId))
		{
			if (KMessageBox::questionYesNo(this, tr("Do you want to overwrite the existing custom object %1?").arg(customObjectId)) == KMessageBox::No)
			{
				return;
			}
		}

		this->unitSelectionDialog()->select(originalObjectId);

		if (this->unitSelectionDialog()->exec() == QDialog::Accepted)
		{
			qDebug() << "Custom Object ID:" << customObjectId;

			this->objectData()->modifyField(this->unitSelectionDialog()->originalObjectId(), customObjectId, "unam", this->unitSelectionDialog()->unitName());
		}
	}
}

void UnitEditor::onRenameObject()
{
}

void UnitEditor::onDeleteObject()
{
}

void UnitEditor::onResetObject()
{
}

void UnitEditor::onResetAllObjects()
{
}

void UnitEditor::onExportAllObjects()
{
}

void UnitEditor::onImportAllObjects()
{
}

void UnitEditor::onCopyObject()
{
}

void UnitEditor::onPasteObject()
{
}

void UnitEditor::onShowRawData(bool show)
{
	/*
	if (show)
	{
		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->standardItems().begin(); iterator != this->treeWidget()->standardItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, QString("%1 (%2)").arg(iterator.key().first).arg(unitName));
		}

		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->customItems().begin(); iterator != this->treeWidget()->customItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, QString("%1:%2 (%3)").arg(iterator.key().first).arg(iterator.key().second).arg(unitName));
		}
	}
	else
	{
		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->standardItems().begin(); iterator != this->treeWidget()->standardItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, unitName);
		}

		for (ObjectTreeWidget::Items::iterator iterator = this->treeWidget()->customItems().begin(); iterator != this->treeWidget()->customItems().end(); ++iterator)
		{
			const QString unitName = this->objectData()->fieldValue(iterator.key().first, iterator.key().second, "unam");
			iterator.value()->setText(0, unitName);
		}
	}
	*/
}

void UnitEditor::activateObject(ObjectTreeItem *item)
{
	qDebug() << "Activated" << item->originalObjectId();
	qDebug() << "with custom ID" << item->customObjectId();
}

void UnitEditor::activateFolder(ObjectTreeItem *item)
{
}

}

}

