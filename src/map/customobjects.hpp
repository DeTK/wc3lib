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

#ifndef WC3LIB_MAP_CUSTOMOBJECTS_HPP
#define WC3LIB_MAP_CUSTOMOBJECTS_HPP

#include "customunits.hpp"

namespace wc3lib
{

namespace map
{

/**
 * Warcraft III: The Frozen Throne allows you to customize every possible object data using Warcraft III's unit data format with some minor changes.
 * Corresponding type of data is stored dynamically (\ref CustomObjects::Type).
 *
 * \sa CustomUnits
 * \sa CustomObjectsCollection
 * \ingroup objectdata
 */
class CustomObjects : public CustomUnits
{
	public:
		enum class Type : int32
		{
			Units,
			Items,
			Destructibles,
			Doodads,
			Abilities,
			Buffs,
			Upgrades
		};

		/**
		 * \brief Single value modfication entry.
		 *
		 * The value id of a modification can be accessed via \ref valueId(). It is the corresponding value of the SLK file entry.
		 * For each type of modification there exists a meta data SLK file which contains information about the field to which
		 * the modification belongs. This information is mostly required by the World Editor because it specifies which values
		 * can be defined by the user.
		 *
		 * This is a list of all meta data SLK files from The Frozen Throne:
		 * <ul>
		 * <li>UnitMetaData.slk - \ref CustomObjects::Type::Units </li>
		 * </ul>
		 *
		 * \ingroup objectdata
		 */
		class Modification : public CustomUnits::Modification
		{
			public:
				Modification(CustomObjects::Type type);
				Modification(const Modification &other);
				virtual ~Modification();

				CustomObjects::Type type() const;

				void setLevel(int32 level);
				/**
				 * Only read for doodads, abilities and upgrades. Doodads use this as variation number.
				 */
				int32 level() const;

				void setData(int32 data);
				/**
				 * Only read for doodads, abilities and upgrades but actually only used by abilities to refer specific raw data:
				 * A, 1 = B, 2 = C, 3 = D, 4 = F, 5 = G, 6 = H
				 */
				int32 data() const;

				virtual std::streamsize read(InputStream &istream) override;
				virtual std::streamsize write(OutputStream &ostream) const override;

			protected:
				CustomObjects::Type m_type;
				int32 m_level; // level/variation
				int32 m_data; // A, 1 = B, 2 = C, 3 = D, 4 = F, 5 = G, 6 = H
		};

		class Object : public CustomUnits::Unit
		{
			public:
				Object(CustomObjects::Type type);
				Object(const Object &other);
				virtual ~Object();

				CustomObjects::Type type() const;

			protected:
				virtual CustomUnits::Modification* createModification() const override;

				CustomObjects::Type m_type;
		};

		CustomObjects(Type type);
		Type type() const;

		virtual const byte* fileName() const override;
		/**
		 * For The Frozen Throne the latest version became 2 instead of 1.
		 * \note The specification http://wc3lib.org/spec/InsideTheW3M.html states that the version is still 1. According to tests this is wrong.
		 */
		virtual uint32 latestFileVersion() const override;

	protected:
		virtual Unit* createUnit() const override;

		Type m_type;
};

inline CustomObjects::Type CustomObjects::Object::type() const
{
	return this->m_type;
}

inline CustomObjects::Type CustomObjects::Modification::type() const
{
	return this->m_type;
}

inline void CustomObjects::Modification::setLevel(int32 level)
{
	this->m_level = level;
}

inline int32 CustomObjects::Modification::level() const
{
	return this->m_level;
}

inline void CustomObjects::Modification::setData(int32 data)
{
	this->m_data = data;
}

inline int32 CustomObjects::Modification::data() const
{
	return this->m_data;
}

inline CustomObjects::Type CustomObjects::type() const
{
	return this->m_type;
}

}

}

#endif
