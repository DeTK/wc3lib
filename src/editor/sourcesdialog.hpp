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

#ifndef WC3LIB_EDITOR_SOURCESDIALOG_HPP
#define WC3LIB_EDITOR_SOURCESDIALOG_HPP

#include <QDialog>

#include <kdemacros.h>

#include "ui_sourcesdialog.h"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;

/**
 * \brief GUI dialog which allows selection of sources based on class \ref MpqPriorityList.
 *
 * It uses a simple KDE edit list box with a URL completion model.
 * All changes on its list do directly affect its corresponding source (\ref source()).
 *
 * The higher entries have a higher priority than the lower entries reordering items will change priorities as well.
 *
 * \note Since MpqPriorityList doesn't provide any signals you'll have to call update when any changes on its source were made.
 */
class KDE_EXPORT SourcesDialog : public QDialog, protected Ui::SourcesDialog
{
	Q_OBJECT

	public slots:
		/**
		 * Updates the list of all source based on stored sources of corresponding source object (\ref source()).
		 */
		void update();

	public:
		/**
		 * Creates a new source dialog using the entries from \p source.
		 *
		 * \param source The source of which the entries are listed in the dialog and can be reordered.
		 * \param parent The parent widget of the dialog.
		 * \param flags The window flags of the dialog.
		 */
		SourcesDialog(MpqPriorityList *source, QWidget *parent = 0, Qt::WFlags flags = 0);

		/**
		 * \return Returns the associated source of which the entries are listed.
		 */
		MpqPriorityList* source() const;

	protected slots:
		void added(const QString &text);
		void ok();
		/**
		 * \note Don't reload any sources which have already been added except there is a new one with higher priority.
		 */
		void apply();
		void restoreDefaults();

	protected:
		virtual void showEvent(QShowEvent *e);

		QString settingsGroup() const;

	private:
		class MpqPriorityList *m_source;
};

inline MpqPriorityList* SourcesDialog::source() const
{
	return m_source;
}

}

}

#endif
