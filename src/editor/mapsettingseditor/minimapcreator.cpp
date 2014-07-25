#include <QtGui>

#include "../qblp/blpiohandler.hpp"

#include "minimapcreator.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

namespace mapsettingseditor
{

MinimapCreator::MinimapCreator(mapsettingseditor::MpqPriorityList* source) : m_source(source)
{

}

QImage MinimapCreator::render(const map::Minimap &minimap, const map::MenuMinimap &menuMinimap, const map::Pathmap &pathmap, map::Pathmap::Type pathType) const
{
	QImage result;
	BlpIOHandler handler;

	if (!handler.read(&result, minimap)) {
		throw Exception();
	}

	// UI/MiniMap/MiniMapIcon/MinimapIconGold.blp
	// UI/MiniMap/MiniMapIcon/MinimapIconNeutralBuilding.blp
	// UI/MiniMap/MiniMapIcon/MinimapIconStartLoc.blp

	Texture goldTexture("UI/MiniMap/MiniMapIcon/MinimapIconGold.blp");
	goldTexture.setSource(this->source());
	goldTexture.loadQt();

	Texture neutralBuildingTexture("UI/MiniMap/MiniMapIcon/MinimapIconNeutralBuilding.blp");
	neutralBuildingTexture.setSource(this->source());
	neutralBuildingTexture.loadQt();

	Texture startLocTexture("UI/MiniMap/MiniMapIcon/MinimapIconStartLoc.blp");
	startLocTexture.setSource(this->source());
	startLocTexture.loadQt();

	QPainter painter(&result);

	BOOST_FOREACH(map::MenuMinimap::Marks::const_reference ref, menuMinimap)
	{
		QPixmap mark;

		switch (ref.iconType())
		{
			case map::MenuMinimap::Mark::IconType::GoldMine:
				mark = QPixmap::fromImage(*goldTexture.qt().data());

				break;

			case map::MenuMinimap::Mark::IconType::House:
				mark = QPixmap::fromImage(*neutralBuildingTexture.qt().data());

				break;

			case map::MenuMinimap::Mark::IconType::PlayerStart:
				mark = QPixmap::fromImage(*startLocTexture.qt().data());

				break;
		}

		painter.drawPixmap(ref.x(), ref.y(), mark);
	}

	const int widthSteps = pathmap.width() / result.width();
	const int heightSteps = pathmap.height() / result.height();

	for (int width = 0; width < pathmap.width() && width < result.width(); width += widthSteps)
	{
		for (int height = 0; height < pathmap.height() && height < result.height(); height += heightSteps)
		{
			// TODO check the whole rect
			// show color of build
			if (pathmap.tilepoints()[width][height] & map::Pathmap::Type::Build && pathType & map::Pathmap::Type::Build)
			{
				painter.setBrush(QBrush(Qt::yellow));
				painter.drawRect(width, height, widthSteps, heightSteps);
			}

			// TODO draw other colors

		}
	}

	return result;
}

}

}

}