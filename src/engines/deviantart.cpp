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

#include "engines/deviantart.hpp"

#include <QByteArray>
#include <QFile>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QXmlStreamReader>

using namespace Wally::Engines::DeviantArt;

Engine::Engine(QObject *pParent) :
  ::Wally::Engines::Base(pParent),
  _networkAccessManager(new QNetworkAccessManager(this))
{

}

QString Engine::name() const
{
  return "DeviantArt";
}

QPixmap Engine::icon() const
{
  return QPixmap(":/images/deviantart");
}

void Engine::selectNext()
{
  QUrlQuery urlQuery;

  urlQuery.addQueryItem("q", "nature");
  urlQuery.addQueryItem("offset", "1");

  QUrl url("http://backend.deviantart.com/rss.xml");
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

  bool endItem = false, found = false;
  QXmlStreamReader xmlResp(networkReply->readAll());
  QUrl photoUrl;

  while (!xmlResp.atEnd() && !found)
    if (xmlResp.readNext() == QXmlStreamReader::StartElement)
    {
      if (!xmlResp.name().toString().compare("item",Qt::CaseInsensitive))
      {
        endItem = false;

        while ((!xmlResp.atEnd()) && (!endItem))
          switch (xmlResp.readNext())
          {
            case QXmlStreamReader::StartElement:
              if (!xmlResp.name().toString().compare("title",Qt::CaseInsensitive))
              {
                xmlResp.readNext();
                // photoTitle = xmlResp.text().toString();
              }
              else if (!xmlResp.name().toString().compare("link",Qt::CaseInsensitive))
              {
                xmlResp.readNext();
                // _sourceUrl = xmlResp.text().toString();
              }
              else if (!xmlResp.prefix().toString().compare("media",Qt::CaseInsensitive) &&
                       !xmlResp.name().toString().compare("description",Qt::CaseInsensitive))
              {
                xmlResp.readNext();
                // photoDescription = xmlResp.text().toString();
              }
              else if (!xmlResp.prefix().toString().compare("media",Qt::CaseInsensitive) &&
                       !xmlResp.name().toString().compare("content",Qt::CaseInsensitive) &&
                       !xmlResp.attributes().value("medium").toString().compare("image",Qt::CaseInsensitive))
              {
                photoUrl = xmlResp.attributes().value("url").toString();
                // photoSize.setWidth(xmlResp.attributes().value("width").toString().toInt());
                // photoSize.setHeight(xmlResp.attributes().value("height").toString().toInt());
              }
              else if (!xmlResp.prefix().toString().compare("media",Qt::CaseInsensitive) &&
                       !xmlResp.name().toString().compare("rating",Qt::CaseInsensitive))
              {
                xmlResp.readNext();
                // isAdultContent = !xmlResp.text().toString().compare("adult",Qt::CaseInsensitive);
              }
              break;

            case QXmlStreamReader::EndElement:
              endItem = !xmlResp.name().toString().compare("item",Qt::CaseInsensitive);
              found = true;
              break;

            default:
              break;
          }
      }
    }

  // newPagesCount = (!found)? 1 : newPagesCount + 1;

  /*
  return (found && _photoUrl.isValid() && !_lastPhotoUrls.contains(_photoUrl) &&
          (!isAdultContent || (_adultFilter == Off)));
  */

  if (!found || !photoUrl.isValid())
  {
    emit failed();
    return;
  }

  QNetworkReply *nextNetworkReply = _networkAccessManager->get(QNetworkRequest(photoUrl));
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

  QFile f("/tmp/test.jpg");
  if (!f.open(QIODevice::WriteOnly))
  {
    emit failed();
    return;
  }

  f.write(networkReply->readAll());
}

::Wally::Engines::SettingsWidget *Engine::settingsWidget(QWidget *pParent)
{
  return new SettingsWidget(pParent);
}

SettingsWidget::SettingsWidget(QWidget *pParent) :
  ::Wally::Engines::SettingsWidget(pParent)
{
}

void SettingsWidget::accept()
{

}
