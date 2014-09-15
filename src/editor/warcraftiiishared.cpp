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

#include <boost/format.hpp>

#include <QtGui>

#include "warcraftiiishared.hpp"
#include "../exception.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

WarcraftIIIShared::WarcraftIIIShared(MpqPriorityList *source) : m_source(source)
{
}

void WarcraftIIIShared::refreshDefaultFiles(QWidget *window)
{
	this->refreshWorldEditorStrings(window);
	this->refreshTriggerStrings(window);
	this->refreshTriggerData(window);
}

Texture* WarcraftIIIShared::teamColorTexture(TeamColor teamColor) const
{
	TeamColorTextures::iterator iterator = this->m_teamColorTextures.find(teamColor);

	if (iterator == this->m_teamColorTextures.end())
	{
		std::auto_ptr<Texture> texture(new Texture(teamColorUrl(teamColor)));
		texture->setSource(this->source());
		Texture *result = texture.get(); // improve performance on first allocation by returning it directly without calling "find"
		this->m_teamColorTextures.insert(teamColor, texture);

		return result;
	}

	return this->m_teamColorTextures.find(teamColor)->second;
}

Texture* WarcraftIIIShared::teamGlowTexture(TeamColor teamGlow) const
{
	TeamColorTextures::iterator iterator = this->m_teamGlowTextures.find(teamGlow);

	if (iterator == this->m_teamGlowTextures.end())
	{
		std::auto_ptr<Texture> texture(new Texture(teamGlowUrl(teamGlow)));
		texture->setSource(this->source());
		Texture *result = texture.get(); // improve performance on first allocation by returning it directly without calling "find"
		this->m_teamGlowTextures.insert(teamGlow, texture);

		return result;
	}

	return this->m_teamGlowTextures.find(teamGlow)->second;
}

void WarcraftIIIShared::refreshWorldEditorStrings(QWidget *window, const KUrl &url)
{
	WorldEditorStringsPtr ptr(new MetaData(url));
	ptr->setSource(this->source());
	ptr->load();

	m_worldEditorStrings.swap(ptr); // exception safe
}

QString WarcraftIIIShared::tr(const QString &key, const QString &group, const QString &defaultValue) const
{
	if (this->worldEditorStrings().get() != 0)
	{
		try
		{
			const QString result = this->worldEditorStrings()->value(group, key);

			/*
			 * Some values like "WESTRING_UE_UNITRACE_HUMAN" refer to other keys and have to be resolved recursively until no
			 * reference is found anymore.
			 */
			if (this->worldEditorStrings()->hasValue(group, result))
			{
				return tr(result, group, defaultValue);
			}

			return result;
		}
		/*
		 * If an exception occured or the value does not exist just return the default value in the end.
		 */
		catch (Exception &e)
		{
		}
	}

	if (!defaultValue.isEmpty())
	{
		return defaultValue;
	}

	return group + "[" + key + "]";
}

QIcon WarcraftIIIShared::icon(const KUrl &url, QWidget *window)
{
	Icons::iterator iterator = m_icons.find(url);

	if (iterator != m_icons.end())
	{
		return iterator.value();
	}

	QString iconFile;

	if (this->source()->download(url, iconFile, window))
	{
		iterator = m_icons.insert(url, QPixmap(iconFile));

		return QIcon(iterator.value());
	}

	return QIcon();
}

QIcon WarcraftIIIShared::worldEditDataIcon(const QString& key, const QString& group, QWidget* window)
{
	QString filePath = MetaData::fromFilePath(this->worldEditData()->value(group, key));
	/*
	 * In some cases the extension is not present.
	 */
	QFileInfo fileInfo(filePath);

	if (fileInfo.suffix().toLower() != "blp")
	{
		filePath += ".blp";
	}

	return this->icon(filePath, window);
}

void WarcraftIIIShared::refreshWorldEditData(QWidget* window, const KUrl &url)
{
	WorldEditDataPtr ptr(new MetaData(url));
	ptr->setSource(this->source());
	ptr->load();

	m_worldEditData.swap(ptr); // exception safe
}

void WarcraftIIIShared::refreshTriggerData(QWidget *window, const KUrl &url)
{
	QString target;

	if (!this->source()->download(url, target, window))
	{
		throw Exception(_("Unable to download file \"UI/TriggerData.txt\"."));
	}

	qDebug() << "Trigger data target: " << target;
	TriggerDataPtr ptr(new map::TriggerData());
	ifstream ifstream(target.toStdString(), std::ios::in);

	if (!ifstream)
	{
		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toStdString());
	}

	ptr->read(ifstream);
	m_triggerData.swap(ptr); // exception safe
}

void WarcraftIIIShared::refreshTriggerStrings(QWidget *window, const KUrl &url)
{
	QString target;

	if (!this->source()->download(url, target, window))
	{
		throw Exception(_("Unable to download file \"UI/TriggerStrings.txt\"."));
	}

	qDebug() << "Trigger strings target: " << target;
	TriggerStringsPtr ptr(new map::TriggerStrings());
	ifstream ifstream(target.toStdString(), std::ios::in);

	if (!ifstream)
	{
		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toStdString());
	}

	ptr->read(ifstream);
	m_triggerStrings.swap(ptr); // exception safe
}

}

}