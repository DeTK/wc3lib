/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>

#include <QtCore>

#include <KMimeType>
#include <KDebug>
#include <KComponentData>
#include <KGlobal>
#include <KUrl>
#include <KDebug>
#include <KLocale>
#include <kde_file.h>
#include <kio/global.h>
#include <KUser>
#include <KTemporaryFile>

#include "kio_mpq.hpp"
#include "../platform.hpp"

static const int debugArea = 7200;

namespace wc3lib
{

namespace editor
{

extern "C" int KDE_EXPORT kdemain(int argc, char **argv)
{
	kDebug(debugArea) << "Entering function";
	KComponentData instance("kio_mpq");

	if (argc != 4)
	{
		fprintf(stderr, "Usage: kio_mpq protocol domain-socket1 domain-socket2\n");
		exit(-1);
	}

	MpqSlave slave(argv[2], argv[3]);
	slave.dispatchLoop();

	return 0;
}

const char *MpqSlave::protocol= "mpq";

MpqSlave::MpqSlave(const QByteArray &pool, const QByteArray &app) : KIO::SlaveBase(protocol, pool, app), m_hasAttributes(false), m_attributesVersion(0), m_extendedAttributes(mpq::Attributes::ExtendedAttributes::None), m_seekPos(0)
{
	kDebug(debugArea) << "Created MPQ slave";
}

MpqSlave::~MpqSlave()
{
}

bool MpqSlave::parseUrl(const KUrl &url, QString &fileName, QByteArray &archivePath)
{
	kDebug(debugArea) << "MPQ: Parsing url " << url.prettyUrl();

	QString path = url.path();

	bool appended = false;

	if (path.at(path.size() - 1) != KDIR_SEPARATOR)
	{
		path.append(KDIR_SEPARATOR);
		appended = true;
	}

	int pos = 0;
	int nextPos = 0;

	while ((nextPos = path.indexOf(KDIR_SEPARATOR, pos + 1)) != -1)
	{

		if (!QFileInfo(QFile::encodeName(path.left(nextPos))).exists())
		{
			break;
		}

		pos = nextPos;
	}

	if (pos == 0)
	{
		return false;
	}

	if (appended)
	{
		path.chop(1);
	}

	fileName = QFile::encodeName(path.left(pos));

	if (!QFileInfo(fileName).isFile())
	{
		kDebug(debugArea) << "Is not a file";

		return false;
	}

	toArchivePath(archivePath, path.mid(pos+1, -1));

	kDebug(debugArea) << "Got it with archive path " << archivePath.constData() << "\nand file name " << fileName;

	return true;

}

void MpqSlave::toArchivePath(QByteArray &to, const QString &from)
{
#if KDIR_SEPARATOR == '\\'
	to = from.toUtf8();
#else
	to = from.toUtf8().replace(KDIR_SEPARATOR, '\\');
#endif // KDIR_SEPARATOR == '\\'

}

bool MpqSlave::openArchive(const QString &archive, QString &error)
{
	kDebug(debugArea) << "opening archive " << archive;

	// TEST
	if (!m_archive.isNull())
	{
		kDebug(debugArea) << "With old path " << this->m_archive->path().c_str();
	}
	// TEST END

	// if file paths are equal file must already be open, so don't open again
	if (this->m_archive.isNull() || m_archiveName != archive)
	{
		kDebug(debugArea) << "New opening!";

		MpqArchivePtr ptr(new mpq::Archive());

		try
		{
			ptr->open(archive.toUtf8().constData());
		}
		catch (Exception &exception)
		{
			error = exception.what();

			kDebug(debugArea) << "Error: " << error;

			return false;
		}

		if (ptr->containsAttributesFile())
		{
			mpq::Attributes attributes = ptr->attributesFile();

			if (!attributes.isValid())
			{
				error = i18n("Attributes file is not valid");

				kDebug(debugArea) << "Error: " << error;

				return false;
			}

			try
			{
				attributes.attributes(this->m_attributesVersion, this->m_extendedAttributes, this->m_crcs, this->m_fileTimes, this->m_md5s); // attributes data is required for file time stamps
				m_hasAttributes = true;
			}
			catch (Exception &exception)
			{
				error = exception.what();

				kDebug(debugArea) << "Error: " << error;

				return false;
			}
		}

		m_archive.swap(ptr); // exception safety
		m_archiveName = archive;
	}

	kDebug(debugArea) << "Success on opening archive";

	return true;
}


void MpqSlave::open(const KUrl &url, QIODevice::OpenMode mode)
{
	QString fileName;
	QByteArray archivePath;

	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

		return;
	}

	QString errorText;

	if (!openArchive(fileName, errorText))
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", fileName, errorText));

		return;
	}

	mpq::File file = m_archive->findFile(archivePath.constData()); // TODO locale and platform

	kDebug(debugArea) << "Opening: " << archivePath.constData();

	if (!file.isValid())
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

		return;
	}

	this->m_file = file;

	if (mode == QIODevice::ReadOnly || mode == QIODevice::ReadWrite)
	{
		if (QString::fromUtf8(archivePath).endsWith(".mpq", Qt::CaseInsensitive))
		{
			mimeType("application/x-mpq");
		}
		// detect MIME type by name and content, therefore read the whole file
		else
		{
			stringstream stream;

			try
			{
				std::streamsize size = this->m_file.writeData(stream);
				QByteArray data = stream.str().c_str();
				KMimeType::Ptr fileMimeType = KMimeType::findByNameAndContent(url.fileName(), data);
				mimeType(fileMimeType->name());

			}
			catch (Exception &e)
			{
				kDebug(debugArea) << "Unable to read file and detect MIME type: " << url.prettyUrl() << ": " << e.what();
			}
		}
	}


	kDebug(debugArea) << "Successfully open";
	KIO::filesize_t size = 0;

	try
	{
		size = boost::numeric_cast<KIO::filesize_t>(this->m_file.compressedSize());
	}
	catch (Exception &e)
	{
		error(KIO::ERR_ABORTED, i18n("%1: %2", url.prettyUrl(), e.what()));

		return;
	}

	totalSize(size);
	position(0);
	opened();
}

void MpqSlave::close()
{
	this->m_seekPos = 0;
	this->m_file.close();
}

void MpqSlave::read(KIO::filesize_t size)
{
	stringstream sstream;

	try // TODO faster way without writing ALL data
	{
		m_file.writeData(sstream); // write data first to decompress everything
	}
	catch (Exception &e)
	{
		error(KIO::ERR_COULD_NOT_READ, i18n("%1: %2", m_file.path().c_str(), e.what()));

		return;
	}

	sstream.seekg(m_seekPos);
	byte content[size];
	sstream.read(content, size);

	data(QByteArray::fromRawData(content, size));
}

void MpqSlave::seek(KIO::filesize_t offset)
{
	this->m_seekPos += offset;
}

void MpqSlave::mkdir(const KUrl& url, int permissions)
{
	// MPQ archives does not support directory structure
	// Only simulate creating directory

	finished();
}

void MpqSlave::listDir(const KUrl &url)
{
	kDebug(debugArea) << "MpqProtocol::listDir" << url.url();

	QString fileName;
	QByteArray archivePath;

	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

		return;
	}

	if (!archivePath.isEmpty() && archivePath.at(archivePath.size() - 1) != '\\')
	{
		archivePath.append('\\'); // interpret as directory
	}

	QString errorText;

	if (!openArchive(fileName, errorText))
	{
		// use slave defined for custom text other than error code!
		error(KIO::ERR_SLAVE_DEFINED, i18n("%1: \"%2\"", url.prettyUrl(), errorText));

		return;
	}

	// TODO get all files contained in directory using (listfile)
	mpq::Listfile listfile = m_archive->listfileFile();
	mpq::Listfile::Entries allEntries;
	mpq::Listfile::Entries dirFileEntries;
	mpq::Listfile::Entries dirDirectoryEntries;

	/*
	 * If the archive does not contain its own (listfile) file the installed listfiles from
	 * wc3lib are used.
	 * Usually only standard MPQ archives such as "War3Patch.mpq" do not contain a (listfile) file.
	 */
	if (!listfile.isValid())
	{


		foreach (QFileInfo fileInfo, installedListfiles())
		{
			QFile file(fileInfo.absoluteFilePath());

			if (file.open(QIODevice::ReadOnly))
			{
				mpq::Listfile::Entries entries = mpq::Listfile::entries(file.readAll().constData());
				allEntries.insert(allEntries.end(), entries.begin(), entries.end());
			}
		}
	}
	/*
	 * If the archive contains a (listfile) file its entries are used to check which files are in the current directory.
	 */
	else
	{
		allEntries = listfile.entries();
	}

	/*
	 * The filtering prefix must indicate that it is a directory.
	 */
	string prefix;

	if (!archivePath.isEmpty())
	{
		prefix = archivePath.constData() + '\\';
	}

	// only check for the existing files of the current directory
	allEntries = mpq::Listfile::existingEntries(allEntries, *m_archive, prefix, false);

	dirFileEntries = mpq::Listfile::caseSensitiveFileEntries(allEntries, prefix, false);
	dirDirectoryEntries = mpq::Listfile::caseSensitiveDirEntries(allEntries, prefix, false);

	warning(QObject::tr("Out prefix: %1\ncount all entries: %2\n count dir file entries: %3\nCount dir dir entries: %4").arg(prefix.c_str()).arg(allEntries.size()).arg(dirFileEntries.size()).arg(dirDirectoryEntries.size()));

	/*
	 * In the root directory there should be listed the extra files which are not
	 * contained by the (listfile) file.
	 */
	if (archivePath.isEmpty())
	{
		kDebug(debugArea) << "MpqProtocol::listDir is root directory, appending extra files";

		// list files which are not listed in "(listfile)"

		if (m_archive->containsListfileFile()) // should always be the case
		{
			dirFileEntries.push_back("(listfile)");
		}

		if (m_archive->containsAttributesFile())
		{
			dirFileEntries.push_back("(attributes)");
		}

		if (m_archive->containsSignatureFile())
		{
			dirFileEntries.push_back("(signature)");
		}
	}

	/*
	 * List regular files.
	 */
	BOOST_FOREACH (mpq::Listfile::Entries::reference ref, dirFileEntries)
	{
		// ignore empty entries
		if (ref.empty())
		{
			continue;
		}

		const QString fileName = QString::fromUtf8(mpq::Listfile::fileName(ref).c_str());

		kDebug(debugArea) << "New path \"" << fileName << "\"";

		mpq::File file = m_archive->findFile(ref); // TODO locale and platform

		if (file.isValid())
		{
			//quint64 fileTime = 0;
			time_t fileTime = 0;
			bool validFileTime = false;

			// file time is extended attribute!
			if (m_hasAttributes && (m_extendedAttributes & mpq::Attributes::ExtendedAttributes::FileTimeStamps))
			{
				if (m_fileTimes.size() > file.block()->index())
				{
					mpq::FILETIME &storedFileTime = m_fileTimes[file.block()->index()];
					const bool cast = storedFileTime.toTime(fileTime);

					if (!cast)
					{
						kDebug(debugArea) << i18n("%1: Invalid file time for \"%2\": high - %3, low - %4", url.prettyUrl(), ref.c_str(), storedFileTime.highDateTime, storedFileTime.lowDateTime);
					}
					else
					{
						validFileTime = true;
					}
				}
				else
				{
					kDebug(debugArea) << i18n("%1: File time is not stored in \"(attributes)\" file.", url.prettyUrl());
				}
			}

			KIO::UDSEntry entry;
			entry.insert(KIO::UDSEntry::UDS_NAME, fileName);
			entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG);
			entry.insert(KIO::UDSEntry::UDS_SIZE, file.compressedSize());


			if (validFileTime)
			{
				entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, fileTime);
			}

			entry.insert(KIO::UDSEntry::UDS_ACCESS, (S_IRWXU | S_IRWXG | S_IRWXO));

			KMimeType::Ptr ptr = KMimeType::findByPath(QFile::decodeName(ref.c_str())); // TODO find by content?

			if (!ptr.isNull())
			{
				entry.insert(KIO::UDSEntry::UDS_MIME_TYPE, ptr->name());
			}

			listEntry(entry, false);
		}
		else // invalid entry
		{
			warning(i18n("%1: Invalid directory entry \"%2\"", url.prettyUrl(), fileName));
		}
	}

	/*
	 * List directories.
	 */
	BOOST_FOREACH (mpq::Listfile::Entries::reference ref, dirDirectoryEntries)
	{
		kDebug(debugArea) << "Is dir";

		const QString dirName = QString::fromUtf8(mpq::Listfile::fileName(ref).c_str());

		KIO::UDSEntry entry;
		entry.insert(KIO::UDSEntry::UDS_NAME, dirName);
		entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
		entry.insert(KIO::UDSEntry::UDS_ACCESS, (S_IRWXU | S_IRWXG | S_IRWXO));
		listEntry(entry, false);
	}

	listEntry(KIO::UDSEntry(), true); // ready
	finished();
}

void MpqSlave::stat(const KUrl &url)
{
	kDebug(debugArea) << "MpqProtocol::stat" << url.url();

	QString fileName;
	QByteArray archivePath;

	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

		return;
	}

	QString errorText;

	if (!openArchive(fileName, errorText))
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", url.prettyUrl(), errorText));

		return;
	}

	kDebug(debugArea) << "MpqProtocol::state Searching file " << archivePath.constData();

	mpq::File file = m_archive->findFile(archivePath.constData()); // TODO locale and platform

	KIO::UDSEntry entry;
	entry.insert(KIO::UDSEntry::UDS_NAME, url.path());
	entry.insert(KIO::UDSEntry::UDS_ACCESS, (S_IRWXU | S_IRWXG | S_IRWXO));


	if (file.isValid()) // TODO includes dirs?
	{
		//quint64 fileTime = 0;
		time_t fileTime = 0;
		bool validFileTime = false;
		const bool hasFileTime = m_hasAttributes && (this->m_extendedAttributes & mpq::Attributes::ExtendedAttributes::FileTimeStamps);

		// file time is extended attribute!
		if (hasFileTime)
		{
			if (m_fileTimes.size() > file.block()->index())
			{
				mpq::FILETIME &storedFileTime = m_fileTimes[file.block()->index()];
				const bool cast = storedFileTime.toTime(fileTime);

				if (!cast)
				{
					kDebug(debugArea) << i18n("%1: Invalid file time for \"%2\": high - %3, low - %4", url.prettyUrl(), file.path().c_str(), storedFileTime.highDateTime, storedFileTime.lowDateTime);
				}
				else
				{
					validFileTime = true;
				}
			}
			else
			{
				kDebug(debugArea) << i18n("%1: File time is not stored in \"(attributes)\" file.", url.prettyUrl());
			}
		}

		entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG);
		entry.insert(KIO::UDSEntry::UDS_SIZE, file.compressedSize());

		if (validFileTime)
		{
			entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, fileTime);
		}

		KMimeType::Ptr ptr = KMimeType::findByPath(url.path()); // TODO find by content?

		if (!ptr.isNull())
		{
			entry.insert(KIO::UDSEntry::UDS_MIME_TYPE, ptr->name());
		}

		statEntry(entry);
		finished();
	}
	/*
	 * Making stat on dir is not supported yet.
	 * It would take some performance to check if any file from all valid listfile entries is a file of any sub directory of the dir.
	 * TODO implement
	else
	{
		// directory path has no file or directory entries and therefore cannot exist
		// TODO check installed listfiles as well
		if (!this->m_archive->containsListfileFile() || mpq::Listfile::caseSensitiveFileEntries(this->m_archive->listfileFile().entries(), archivePath.constData(), true).empty())
		{
			kDebug(debugArea) << "stat(): is no dir, no entries, does not exist.";

			error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

			return;
		}

		kDebug(debugArea) << "stat(): is dir";
		entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
	}
	*/

	error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
}

void MpqSlave::get(const KUrl &url)
{
	QString fileName;
	QByteArray archivePath;

	kDebug(debugArea) << "Getting file " << url.prettyUrl();

	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, i18n("Error on parsing URL: \"%1\"", url.prettyUrl()));

		return;
	}

	kDebug(debugArea) << "Archive file name" << fileName;
	kDebug(debugArea) << "File name" << archivePath;

	QString errorText;

	if (!openArchive(fileName, errorText))
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", url.prettyUrl(), errorText));

		return;
	}

	mpq::File file = m_archive->findFile(archivePath.constData()); // TODO locale and platform

	if (!file.isValid())
	{
		error(KIO::ERR_DOES_NOT_EXIST, i18n("Is no valid file: \"%1\"", url.prettyUrl()));

		return;
	}

	kDebug(debugArea) << "Found file" << archivePath;

	if (!file.isFile())
	{
		error(KIO::ERR_IS_DIRECTORY, url.prettyUrl());

		return;
	}

	kDebug(debugArea) << "Is file";

	const KIO::filesize_t fileSize = file.size();
	totalSize(fileSize);

	qint64 read = 0;

	// directly write into an array without reallocating anything, since we know the exact size
	char buffer[fileSize];
	boost::iostreams::basic_array_sink<char> sr(buffer, fileSize);
	boost::iostreams::stream< boost::iostreams::basic_array_sink<char> > source(sr);

	try
	{
		read = file.writeData(source);
	}
	catch (Exception &exception)
	{
		error(KIO::ERR_COULD_NOT_READ, i18n("%1: \"%2\".", url.prettyUrl(), exception.what()));

		return;
	}

	// fromRawData does not create a copy
	// only use 1024 bytes at maximum to identify the MIME type
	const QByteArray content = QByteArray::fromRawData(buffer, std::min<KIO::filesize_t>(fileSize, 1024));
	KMimeType::Ptr mime = KMimeType::findByNameAndContent(archivePath, content);
	kDebug(debugArea) << "Emitting mimetype" << mime->name();
	mimeType(mime->name());

	// fromRawData does not create a copy
	const QByteArray byteArray = QByteArray::fromRawData(buffer, fileSize);

	data(byteArray);
	processedSize(KIO::filesize_t(read));

	// empty byte array to mark the end of the data
	data(QByteArray());

	finished();
}

void MpqSlave::put(const KUrl &url, int permissions, KIO::JobFlags flags)
{

	QString fileName;
	QByteArray archivePath;

	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

		return;
	}

	QString errorText;

	if (!openArchive(fileName, errorText))
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", fileName, errorText));

		return;
	}

	mpq::File::Locale locale = mpq::File::Locale::Neutral; // TODO get from URL
	mpq::File::Platform platform = mpq::File::Platform::Default;

	// TODO read URL into temporary local file and add file to MPQ archive!

	warning(i18n("Not supported yet!"));

	finished();

	/*
	if (archivePath == "(listfile)" || archivePath == "(signature)" || archivePath == "(attributes)" || archivePath.contains("(patch_metadata)"))
	{
		error(KIO::ERR_WRITE_ACCESS_DENIED, url.prettyUrl());

		return;
	}

	Q

	const mpq::MpqFile *file = this->m_archive->findFile(archivePath.constData(), locale, platform);

	if (file != 0 && !(flags & KIO::Overwrite))
	{
		error(KIO::ERR_CANNOT_DELETE_ORIGINAL, url.prettyUrl());

		return;
	}

	qint64 bytes;
	KIO::filesize_t totalBytes = 0;
	QByteArray buffer;

	while (true)
	{
		dataReq();
		bytes = readData(buffer);

		if (bytes <= 0)
			break;

		totalBytes += bytes;
		processedSize(totalBytes);
	}


	try
	{
		m_archive->addFile(archivePath.constData(), buffer.data(), buffer.size(), (flags & KIO::Overwrite), locale, platform);
	}
	catch (Exception &exception)
	{
		error(KIO::ERR_COULD_NOT_WRITE, i18n("%1: \"%2\".", url.prettyUrl(), exception.what().c_str()));

		return;
	}

	m_modified = QFileInfo(m_archive->path().c_str()).lastModified();

	finished();
	*/
}

}

}
