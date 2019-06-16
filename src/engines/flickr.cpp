/*
  Wally
  Copyright (C) 2018 BeCrux

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "application.hpp"
#include "engines/flickr.hpp"

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QUrlQuery>
#include <QXmlStreamReader>

#define FLICKR_API_KEY "99c82d50dc44a9d5bf7e54291f242511"

using namespace Wally::Engines::Flickr;

Engine::Engine(QObject *pParent) :
  ::Wally::Engines::Base(pParent),
  _currentItem(0),
  _networkAccessManager(new QNetworkAccessManager(this))
{
  QSettings &s = Application::storage();

  SettingsGroupScope sgs(s, name());

  _currentItem = s.value("currentItem").toInt();

  SettingsReadArrayScope sras(s, "items");

  for (int i = 0; i < sras.size(); ++i)
  {
    Item item;

    s.setArrayIndex(i);
    item.tags = s.value("tags").toStringList();
    item.text = s.value("text").toString();
    item.tagsCondition = static_cast< Item::TagsCondition >(s.value("tagsCondition").toInt());
    item.searchOrder = static_cast< Item::SearchOrder >(s.value("searchOrder").toInt());
    item.currentPage = s.value("currentPage").toInt();
    item.pagesCount = s.value("pagesCount").toInt();

    _items.append(item);
  }
}

QString Engine::name() const
{
  return "Flickr";
}

QPixmap Engine::icon() const
{
  return QPixmap();
}

void Engine::selectNext()
{
  static const char * const so[Item::SearchOrderSize] =
    {
      "date-posted-asc",
      "date-posted-desc",
      "date-taken-asc",
      "date-taken-desc",
      "interestingness-asc",
      "interestingness-desc",
      "relevance"
    };

  static const char * const tc[Item::TagsConditionSize] =
    {
      "all",
      "any"
    };

  QUrlQuery urlQuery;

  _currentItem = (_currentItem + 1) % _items.size();
  Item item = _items.at(_currentItem);

  urlQuery.addQueryItem("method", "flickr.photos.search");
  urlQuery.addQueryItem("api_key", FLICKR_API_KEY);

  if (!item.tags.isEmpty())
  {
    urlQuery.addQueryItem("tags", item.tags.join(","));
    urlQuery.addQueryItem("tag_mode", tc[item.tagsCondition]);
  }
  else if (!item.text.isEmpty())
  {
    item.text.replace(" ", "+");
    urlQuery.addQueryItem("text", item.text);
  }
  else
  {
    emit failed();
    return;
  }

  urlQuery.addQueryItem("media", "photos");
  urlQuery.addQueryItem("sort", so[item.searchOrder]);
  urlQuery.addQueryItem("per_page", "1");

  urlQuery.addQueryItem("page", QString::number(item.currentPage));

  QUrl url("https://api.flickr.com/services/rest");
  url.setQuery(urlQuery);

  QNetworkReply *networkReply = _networkAccessManager->get(QNetworkRequest(url));
  connect(networkReply, SIGNAL(finished()), this, SLOT(processSearchResult()));
}

void Engine::processSearchResult()
{
  sender()->deleteLater();

  QNetworkReply *networkReply = qobject_cast< QNetworkReply * >(sender());
  if (!networkReply || (networkReply->error() != QNetworkReply::NoError))
  {
    emit failed();
    return;
  }

  bool respOk = false;
  QXmlStreamReader xmlResp(networkReply->readAll());
  QString photoId;

  while (!xmlResp.atEnd())
    if (xmlResp.readNext() == QXmlStreamReader::StartElement)
    {
      if (!xmlResp.name().toString().compare("rsp", Qt::CaseInsensitive))
        respOk = !xmlResp.attributes().value("stat").toString().compare("ok", Qt::CaseInsensitive);
      else if (!xmlResp.name().toString().compare("photos", Qt::CaseInsensitive))
        _items[_currentItem].pagesCount = xmlResp.attributes().value("total").toInt();
      else if (!xmlResp.name().toString().compare("photo", Qt::CaseInsensitive) &&
               (xmlResp.attributes().value("ispublic").toString() == "1"))
        photoId = xmlResp.attributes().value("id").toString();
    }

  if (!respOk || photoId.isEmpty())
  {
    emit failed();
    return;
  }

  QUrlQuery urlQuery;

  urlQuery.addQueryItem("method", "flickr.photos.getSizes");
  urlQuery.addQueryItem("api_key", FLICKR_API_KEY);
  urlQuery.addQueryItem("photo_id", photoId);

  QUrl url("https://api.flickr.com/services/rest");
  url.setQuery(urlQuery);

  QNetworkReply *nextNetworkReply = _networkAccessManager->get(QNetworkRequest(url));
  nextNetworkReply->setProperty("photoId", photoId);
  connect(nextNetworkReply, SIGNAL(finished()), this, SLOT(processSizeQueryResult()));
}

void Engine::processSizeQueryResult()
{
  sender()->deleteLater();

  QNetworkReply *networkReply = qobject_cast< QNetworkReply * >(sender());
  if (!networkReply || (networkReply->error() != QNetworkReply::NoError))
  {
    emit failed();
    return;
  }

  bool respOk = false;
  QUrl photoUrl;
  int currentArea = 0;

  QXmlStreamReader xmlResp(networkReply->readAll());

  while (!xmlResp.atEnd())
    if (xmlResp.readNext() == QXmlStreamReader::StartElement)
    {
      if (!xmlResp.name().toString().compare("rsp", Qt::CaseInsensitive))
        respOk = !xmlResp.attributes().value("stat").toString().compare("ok", Qt::CaseInsensitive);
      else if (!xmlResp.name().toString().compare("size", Qt::CaseInsensitive))
      {
        int height = xmlResp.attributes().value("height").toInt();
        int width = xmlResp.attributes().value("width").toInt();

        if ((height * width) > currentArea)
        {
          currentArea = height * width;
          photoUrl.setUrl(xmlResp.attributes().value("source").toString());
        }
      }
    }

  if (!respOk || photoUrl.isEmpty())
  {
    emit failed();
    return;
  }

  QNetworkReply *nextNetworkReply = _networkAccessManager->get(QNetworkRequest(photoUrl));
  nextNetworkReply->setProperty("photoId", networkReply->property("photoId").toString());
  connect(nextNetworkReply, SIGNAL(finished()), this, SLOT(savePhoto()));
}

void Engine::savePhoto()
{
  sender()->deleteLater();

  QNetworkReply *networkReply = qobject_cast< QNetworkReply * >(sender());
  if (!networkReply || (networkReply->error() != QNetworkReply::NoError))
  {
    emit failed();
    return;
  }

  QFileInfo outputFile(Application::dataDir(), networkReply->url().fileName());

  {
    QFile f(outputFile.absoluteFilePath());
    if (!f.open(QIODevice::WriteOnly))
    {
      emit failed();
      return;
    }

    f.write(networkReply->readAll());
  }

  QUrlQuery urlQuery;
  urlQuery.addQueryItem("method", "flickr.photos.getInfo");
  urlQuery.addQueryItem("api_key", FLICKR_API_KEY);
  urlQuery.addQueryItem("photo_id", networkReply->property("photoId").toString());

  QUrl url("https://api.flickr.com/services/rest");
  url.setQuery(urlQuery);

  QNetworkReply *nextNetworkReply = _networkAccessManager->get(QNetworkRequest(url));
  nextNetworkReply->setProperty("outputFileName", outputFile.absoluteFilePath());
  connect(nextNetworkReply, SIGNAL(finished()), this, SLOT(processPhotoInfo()));
}

void Engine::processPhotoInfo()
{
  sender()->deleteLater();

  QVariantMap info;
  bool respOk = false;

  QNetworkReply *networkReply = qobject_cast< QNetworkReply * >(sender());
  if (networkReply && (networkReply->error() == QNetworkReply::NoError))
  {
    QXmlStreamReader xmlResp(networkReply->readAll());

    while (!xmlResp.atEnd())
      if (xmlResp.readNext() == QXmlStreamReader::StartElement)
      {
        if (!xmlResp.name().toString().compare("rsp", Qt::CaseInsensitive))
          respOk = !xmlResp.attributes().value("stat").toString().compare("ok", Qt::CaseInsensitive);
        else if (!xmlResp.name().toString().compare("title", Qt::CaseInsensitive))
        {
          xmlResp.readNext();
          info[TITLE] = xmlResp.text().toString();
        }
        else if (!xmlResp.name().toString().compare("description", Qt::CaseInsensitive))
        {
          xmlResp.readNext();
          info[DESCRIPTION] = xmlResp.text().toString();
        }
        else if (!xmlResp.name().toString().compare("owner", Qt::CaseInsensitive))
        {
          QString s = xmlResp.attributes().value("realname").toString();

          if (s.isEmpty())
            s = xmlResp.attributes().value("username").toString();

          info[AUTHOR] = s;
          info[LOCATION] = xmlResp.attributes().value("location").toString();
        }
        else if (!xmlResp.name().toString().compare("url", Qt::CaseInsensitive) &&
                 !xmlResp.attributes().value("type").toString().compare("photopage", Qt::CaseInsensitive))
        {
          xmlResp.readNext();
          info[URL] = xmlResp.text().toString();
        }
      }
  }

  emit pictureAvailable(
    QFileInfo(networkReply->property("outputFileName").toString()),
    (respOk)? info : QVariantMap()
  );
}

void Engine::updateStorage()
{

}

::Wally::Engines::SettingsWidget *Engine::settingsWidget(QWidget *parent)
{
  return new SettingsWidget(parent);
}

SettingsWidget::SettingsWidget(QWidget *parent) :
  ::Wally::Engines::SettingsWidget(parent)
{
}

void SettingsWidget::accept()
{

}
