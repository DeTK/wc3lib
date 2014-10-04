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

#ifndef WC3LIB_EDITOR_IDDIALOG_HPP
#define WC3LIB_EDITOR_IDDIALOG_HPP

#include <QDialog>

#include "ui_iddialog.h"

namespace wc3lib
{

namespace editor
{

class IdDialog : public QDialog, protected Ui::IdDialog
{
	public:
		explicit IdDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

		void setId(const QString &id);
		QString id() const;
};

inline void IdDialog::setId(const QString &id)
{
	this->m_lineEdit->setText(id);
}

inline QString IdDialog::id() const
{
	return this->m_lineEdit->text();
}

}

}

#endif // WC3LIB_EDITOR_IDDIALOG_HPP
