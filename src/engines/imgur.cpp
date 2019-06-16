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
#include "engines/imgur.hpp"

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QUrlQuery>

#define IMGUR_CLIENT_ID "0b403409fec57c7"

using namespace Wally::Engines::Imgur;

Engine::Engine(QObject *parent) :
  ::Wally::Engines::Base(parent),
  _networkAccessManager(new QNetworkAccessManager(this))
{

}

QString Engine::name() const
{
  return "Imgur";
}

QPixmap Engine::icon() const
{
  return QPixmap();
}

void Engine::selectNext()
{
  QUrlQuery urlQuery;

  // Authorization: Client-ID <YOUR_CLIENT_ID>

  urlQuery.addQueryItem("q", "nature");
  // q_all 	Search for all of these words (and)
  // q_any 	Search for any of these words (or)
  // q_exactly 	Search for exactly this word or phrase
  // q_not 	Exclude results matching this
  // q_type 	Show results for any file type, jpg  | png  | gif  | anigif (animated gif)  | album
  // q_size_px 	Size ranges
  //   small (500 pixels square or less)
  //   med (500 to 2,000 pixels square)
  //   big (2,000 to 5,000 pixels square)
  //   lrg (5,000 to 10,000 pixels square)
  //   huge (10,000 square pixels and above)

  // sort optional time | viral | top - defaults to time

  // window 	optional 	Change the date range of the request if the sort is 'top'
  // day | week | month | year | all, defaults to all.

  // https://api.imgur.com/3/gallery/search/{{sort}}/{{window}}/{{page}}?q=cats

  QUrl url("https://api.imgur.com/3/gallery/search/0.json");
  url.setQuery(urlQuery);

  QNetworkRequest nr(url);
  nr.setRawHeader("Authorization", QString("Client-ID %1").arg(IMGUR_CLIENT_ID).toUtf8());

  QNetworkReply *networkReply = _networkAccessManager->get(nr);
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

  QJsonParseError jsonParseError;
  QJsonDocument jsonDoc = QJsonDocument::fromJson(networkReply->readAll(), &jsonParseError);

  if (jsonParseError.error != QJsonParseError::NoError)
  {
    emit failed();
    return;
  }

  QJsonObject jsonRoot = jsonDoc.object();
  if (!jsonRoot["success"].toBool() || (jsonRoot["status"].toInt() != 200))
  {
    emit failed();
    return;
  }

  QJsonArray items = jsonRoot["data"].toArray();
  QJsonObject photoData = items.at(0).toObject();

  // title = photoData["title"].toString()
  // description = photoData["description"].toString()
  // datetime = photoData["datetime"].toInt()
  // photoCount = photoData["images_count"].toInt();

  QJsonObject image = photoData["images"].toArray().at(0).toObject();
  // width = image["width"]
  // height = image["height"]
  QUrl photoUrl = image["link"].toString();

  if (photoUrl.isEmpty())
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

  emit pictureAvailable(QFileInfo(networkReply->property("outputFileName").toString()), QVariantMap());
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
