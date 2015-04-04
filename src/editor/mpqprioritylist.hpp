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

#ifndef WC3LIB_EDITOR_MPQPRIORITYLIST_HPP
#define WC3LIB_EDITOR_MPQPRIORITYLIST_HPP

#include <map>

#include <boost/ptr_container/ptr_set.hpp>
#include <boost/scoped_ptr.hpp>

#include <QFileInfo>

#include <kdemacros.h>
#include <KUrl>

#include "platform.hpp"
#include "../mpq.hpp"
#include "resource.hpp"
#include "texture.hpp"
#include "ogremdlx.hpp"
#include "warcraftiiishared.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Entry of an MPQ priority list.
 * An entry can either be an archive or a URL of a directory.
 * Each entry has its own priority. Entries with higher priority will be returned more likely than those with less priority.
 *
 * The class supports comparison operators which allow you to compare their priorities
 * or using them as keys for maps based on their priority.
 * Since the priority is not unique you should use std::multimap or std::multiset.
 */
class KDE_EXPORT MpqPriorityListEntry : public boost::operators<MpqPriorityListEntry>
{
	public:
		typedef MpqPriorityListEntry self;
		/**
		 * \brief Each entry in a \ref MpqPriorityList has a numeric priority where entries with higher priorities are prefered over entries with lower priorities.
		 *
		 * The priority is important when resolving file paths since entries with higher priorities are used first
		 * to check if the file exists in that entry.
		 *
		 * For example in Warcraft III we have different MPQ archives from which the "War3Patch.mpq" usually is prefered over the others.
		 */
		typedef std::size_t Priority;

		/**
		 * \param url Has to refer to an archive or a directory.
		 * \param priority The priority of the entry. Higher priorities will be served first.
		 */
		MpqPriorityListEntry(const KUrl &url, Priority priority);
		virtual ~MpqPriorityListEntry();

		Priority priority() const;
		/**
		 * Entries can be local or remote directories or archives.
		 * This function checks if the entry is a local or remote directory.
		 *
		 * \param window GUI window which is used for any feedback of synchronous requests.
		 *
		 * \return Returns true if the entry is a directory.
		 */
		bool isDirectory(QWidget *window) const;

		const KUrl& url() const;

		bool operator<(const self& other) const;
		bool operator==(const self& other) const;

	protected:
		Priority m_priority;
		KUrl m_url;
};

inline MpqPriorityListEntry::Priority MpqPriorityListEntry::priority() const
{
	return this->m_priority;
}

inline const KUrl& MpqPriorityListEntry::url() const
{
	return this->m_url;
}

inline bool MpqPriorityListEntry::operator<(const self &other) const
{
	return this->priority() < other.priority();
}

inline bool MpqPriorityListEntry::operator==(const self &other) const
{
	return this->priority() == other.priority();
}

/**
 * \brief File source handling class which orders sources by their priorities to access files.
 *
 * The MpqPriorityList has three major tasks:
 *
 * <ul>
 * <li>File resolution when downloading or uploading files</li>
 * <li>Resource handling which means to hold several resources</li>
 * <li>Holding shared data such as trigger data and World Editor strings (which is done by \ref WarcraftIIIShared actually which can be accessed via \ref sharedData() </li>
 * </ul>
 *
 *
 * Use the following member functions to handle files using the stored sources and the resolution of file paths of the MpqPriorityList:
 * \ref download()
 * \ref upload()
 * \ref mkdir()
 * The following member functions handle the provided sources such as MPQ archives, directories etc. which are used as base file for the files:
 * \ref addSource()
 * \ref removeSource()
 * \ref sources()
 * As MPQ supports locales you can set a default locale which is used for up- and downloads using \ref setLocale().
 *
 * When downloading a file from a specific URL it checks if the URL is absolute. If not all source URLs will be prepended in the order of priority and it will be checked for the existence of the newly created URL. If the URL exists the file will be downloaded.
 * This means that each source is a possible location for relative URLs (usually file paths) but they are always tested in the order of their priorities.
 * For example if you have multiple MPQ archives as Warcraft III has:
 * <ul>
 * <li>war3.mpq</li>
 * <li>war3x.mpq</li>
 * <li>War3Patch.mpq</li>
 * <li>War3XLocal.mpq</li>
 * </ul>
 * Now you will have to add the MPQ archives as sources to the MpqPriorityList with the correct priority:
 * \code
 * MpqPriorityList list;
 * list.addSource(KUrl("file://home/me/wc3/War3Patch.mpq", 3)
 * list.addSource(KUrl("file://home/me/wc3/War3XLocal.mpq", 2)
 * list.addSource(KUrl("file://home/me/wc3/war3x.mpq", 1)
 * list.addSource(KUrl("file://home/me/wc3/war3.mpq", 0)
 * \endcode
 *
 * When calling \ref download() it checks in the following order:
 * <ul>
 * <li>War3Patch.mpq</li>
 * <li>War3XLocal.mpq</li>
 * <li>war3x.mpq</li>
 * <li>war3.mpq</li>
 * </ul>
 *
 * Use the following member functions to add, remove or get resources stored in a MpqPriorityList:
 * \ref addResource()
 * \ref removeResource()
 * \ref resources()
 *
 * \note You still have to take care about the deletion of resources. They won't be deleted automatically.
 *
 *
 * The member function \ref sharedData() provides access to all shared data required by Warcraft III or the World Editor.
 *
 * \sa MpqPriorityListEntry
 * \sa Resource
 */
class KDE_EXPORT MpqPriorityList
{
	public:
		typedef MpqPriorityList self;

		/**
		 * List of available sources for one single \ref MpqPriorityList.
		 * The sources are ordered by their priority to resolve sources when dowloading or uploading a file.
		 * Sources with higher priority should therefore be used first.
		 *
		 * \note About the ordering it seems that elements with little priority come first in the set, so you need to reverse the set if you want to resolve by priority.
		 */
		typedef boost::ptr_multiset<MpqPriorityListEntry> Sources;
		typedef MpqPriorityListEntry Source;

		/**
		 * Shared data contains such data as trigger data, trigger strings or World Editor strings
		 * which is shared by many components.
		 */
		typedef boost::scoped_ptr<WarcraftIIIShared> SharedDataPtr;

		/**
		 * \brief Each MpqPriorityList may hold resources which are ordered by their URL for fast access.
		 */
		typedef std::map<KUrl, Resource*> Resources;

		MpqPriorityList();
		virtual ~MpqPriorityList();

		/**
		 * \return Returns all shared data for the World Editor.
		 */
		const SharedDataPtr& sharedData() const;

		/**
		 * Sets the default locale which is used to resolve files which are being uploaded or downloaded to or from MPQ archives.
		 *
		 * \sa locale()
		 */
		void setLocale(mpq::File::Locale locale);
		/**
		 * \sa setLocale()
		 */
		mpq::File::Locale locale() const;

		/**
		 * Adds a source with URL \p url and priorioty \p priority to the priority list.
		 * Each URL should be unique and refer to an archive (mostly MPQ) or directory.
		 * \param priority 0 is default value. Priorities needn't to be unique.
		 * \return Returns true if the URL has been added to the list (this doesn't happen if there already is a source with the given URL or if it refers to an absolute file path which is no archive or directory).
		 * \todo Improve archive detection.
		 */
		virtual bool addSource(const KUrl &url, MpqPriorityListEntry::Priority priority = 0);
		/**
		 *
		 * \section defaultMPQs Default MPQ archives:
		 * These are the priority values for Warcraft's default MPQ archives:
		 * <ul>
		 * <li>war3.mpq - 20</li>
		 * <li>War3Patch.mpq - 22<li>
		 * <li>war3x.mpq - 21</li>
		 * <li>War3xlocal.mpq - 21</li>
		 * </ul>
		 */
		virtual bool addWar3Source();
		/**
		 * \ref defaultMPQs
		 */
		virtual bool addWar3PatchSource();
		/**
		 * \ref defaultMPQs
		 */
		virtual bool addWar3XSource();
		/**
		 * \ref defaultMPQs
		 */
		virtual bool addWar3XLocalSource();
		/**
		 * Adds all default MPQ archives of Warcraft III as possible sources to the list.
		 * \ref defaultMPQs
		 */
		virtual bool addDefaultSources();
		/**
		 * Removes a source from the MpqPriorityList.
		 * \return Returns true if the source has been removed properly.
		 *
		 * @{
		 */
		/**
		 * \note Searches with linear complexity (std::find_if) which might be slow.
		 */
		virtual bool removeSource(const KUrl &url);
		virtual bool removeSource(Source &source);
		/**
		 * @}
		 */
		virtual bool removeWar3Source();
		virtual bool removeWar3PatchSource();
		virtual bool removeWar3XSource();
		virtual bool removeWar3XLocalSource();
		virtual bool removeDefaultSources();

		/**
		 * \copydoc KIO::NetAccess::download()
		 * Considers all entries if it's an relative URL. Otherwise it will at least consider the priority list's locale if none is given.
		 * \note If \p src is an absolute URL it will work just like \ref KIO::NetAccess::download(). Therefore this member function should be a replacement whenever you want to download something to make relative URLs to custom sources available in your application.
		 * \todo If it's an "mpq:/" URL and there is no locale given add one considering \ref locale().
		 */
		virtual bool download(const KUrl &src, QString &target, QWidget *window) const;
		virtual bool upload(const QString &src, const KUrl &target, QWidget *window) const;
		virtual bool mkdir(const KUrl &target, QWidget *window) const;
		/**
		 * \param url The URL which is checked for existence.
		 * \param window GUI window which is used for any feedback.
		 *
		 * \return Returns true if \p url exists in at least one of the entries.
		 */
		virtual bool exists(const KUrl &url, QWidget *window) const;

		/**
		 * \return Returns all sources which are used by the MpqPriorityList.
		 */
		const Sources& sources() const;
		Sources& sources();

		/**
		 * All added resources will also be added to MPQ priority list automaticially.
		 * Therefore there shouldn't occur any problems when you open an external MDL file and need its textures which are contained by the same directory.
		 */
		virtual void addResource(Resource *resource);
		/**
		 * Removes resource from priority list. This function is called by \ref Resource::~Resource() autmatically.
		 * \note You still have to delete the resource manually afterwards.
		 */
		virtual bool removeResource(Resource *resource);
		/**
		 * Removes resource with URL \p url.
		 * \note You still have to delete the resource manually afterwards.
		 */
		virtual bool removeResource(const KUrl &url);

		/**
		 * \return Returns all hold resources.
		 */
		const Resources& resources() const;
		Resources& resources();

		void readSettings(const QString &group);
		void writeSettings(const QString &group);

		/**
		 * Removes all sources.
		 */
		void clear();

		/**
		 * Shows a configuration dialog and waits for acceptance.
		 * Only if the selected sources are accepted it returns true.
		 * Otherwise it returns false.
		 */
		virtual bool configure(QWidget *parent);

	protected:
		SharedDataPtr m_sharedData;
		mpq::File::Locale m_locale;

		Sources m_sources;
		Resources m_resources;
};

inline const MpqPriorityList::SharedDataPtr& MpqPriorityList::sharedData() const
{
	return this->m_sharedData;
}

inline void MpqPriorityList::setLocale(mpq::File::Locale locale)
{
	this->m_locale = locale;
}

inline mpq::File::Locale MpqPriorityList::locale() const
{
	return this->m_locale;
}

inline const MpqPriorityList::Sources& MpqPriorityList::sources() const
{
	return this->m_sources;
}

inline void MpqPriorityList::addResource(Resource *resource)
{
	this->m_resources.insert(std::make_pair(resource->url(), resource));
}


inline bool MpqPriorityList::removeResource(Resource *resource)
{
	return this->removeResource(resource->url());
}

inline bool MpqPriorityList::removeResource(const KUrl &url)
{
	Resources::iterator iterator = this->m_resources.find(url);

	if (iterator == this->m_resources.end())
	{
		return false;
	}

	this->m_resources.erase(iterator);

	return true;
}

inline const MpqPriorityList::Resources& MpqPriorityList::resources() const
{
	return this->m_resources;
}

inline MpqPriorityList::Resources& MpqPriorityList::resources()
{
	return this->m_resources;
}

}

}

#endif
