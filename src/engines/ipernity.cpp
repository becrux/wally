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

#include "engines/ipernity.hpp"

#include <QByteArray>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QXmlStreamReader>

#define IPERNITY_API_KEY "e8315d400000273A461d49b1b40377fd"

using namespace Wally::Engines::Ipernity;

Engine::Engine(QObject *parent) :
  ::Wally::Engines::Base(parent),
  _networkAccessManager(new QNetworkAccessManager(this))
{

}

void Engine::selectNext()
{
  QUrlQuery urlQuery;

  urlQuery.addQueryItem("api_key", IPERNITY_API_KEY);
  urlQuery.addQueryItem("media", "photo");
  urlQuery.addQueryItem("page", "1");
  urlQuery.addQueryItem("per_page","1");

  /*
  if (!_tags.isEmpty())
    searchItem = _tags.join((_condition == And)? ",+" : ",");
  else if (!_text.isEmpty())
  {
    searchItem = _text;
    if (_condition == And)
      searchItem.replace(" "," +");
  }
  else
    return QUrl();

  if (_condition == And)
    searchItem = QString("+") + searchItem;

  if (!_tags.isEmpty())
    url.addQueryItem("tags",searchItem);
  else if (!_text.isEmpty())
    url.addQueryItem("text",searchItem);
  */

  urlQuery.addQueryItem("text", "nature");

  QUrl url("http://api.ipernity.com/api/doc.search/xml");
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

  QXmlStreamReader xmlResp(networkReply->readAll());
  bool respOk = false;
  QString photoId;

  while (!xmlResp.atEnd())
    if (xmlResp.readNext() == QXmlStreamReader::StartElement)
    {
      if (!xmlResp.name().toString().compare("api",Qt::CaseInsensitive))
        respOk = !xmlResp.attributes().value("status").toString().compare("ok",Qt::CaseInsensitive);
      /*
      else if (!xmlResp.name().toString().compare("docs",Qt::CaseInsensitive))
        newPagesCount = xmlResp.attributes().value("total").toString().toInt();
      */
      else if (!xmlResp.name().toString().compare("doc",Qt::CaseInsensitive))
        photoId = xmlResp.attributes().value("doc_id").toString();
    }

  if (!respOk || photoId.isEmpty())
  {
    emit failed();
    return;
  }

  QUrlQuery urlQuery;

  urlQuery.addQueryItem("api_key", IPERNITY_API_KEY);
  urlQuery.addQueryItem("doc_id", photoId);
  urlQuery.addQueryItem("extra", "notes,geo");

  QUrl url("http://api.ipernity.com/api/doc.get/xml");
  url.setQuery(urlQuery);

  QNetworkReply *nextNetworkReply = _networkAccessManager->get(QNetworkRequest(url));
  nextNetworkReply->setProperty("photoId", photoId);
  connect(nextNetworkReply, SIGNAL(finished()), this, SLOT(processInfoQueryResult()));
}

void Engine::processInfoQueryResult()
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
      if (!xmlResp.name().toString().compare("api",Qt::CaseInsensitive))
        respOk = !xmlResp.attributes().value("status").toString().compare("ok",Qt::CaseInsensitive);
      else if (!xmlResp.name().toString().compare("thumb",Qt::CaseInsensitive))
      {
        int w = xmlResp.attributes().value("w").toInt();
        int h = xmlResp.attributes().value("h").toInt();

        if (currentArea < (w * h))
        {
          currentArea = w * h;
          photoUrl = xmlResp.attributes().value("url").toString();
        }
      }
      /*
      else if (!xmlResp.name().toString().compare("doc",Qt::CaseInsensitive))
      {
        photoTitle = xmlResp.attributes().value("title").toString();
        _sourceUrl = xmlResp.attributes().value("link").toString();
      }
      else if (!xmlResp.name().toString().compare("description",Qt::CaseInsensitive))
      {
        xmlResp.readNext();
        photoDescription = xmlResp.text().toString();
      }
      else if (!xmlResp.name().toString().compare("owner",Qt::CaseInsensitive))
        photoOwner = xmlResp.attributes().value("username").toString();
      else if (!xmlResp.name().toString().compare("geo",Qt::CaseInsensitive))
        photoLocation = xmlResp.attributes().value("location").toString();
      */
    }

  if (!respOk || photoUrl.isEmpty())
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
