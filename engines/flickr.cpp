#include "flickr.hpp"

#include <QByteArray>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QXmlStreamReader>

#define FLICKR_API_KEY "99c82d50dc44a9d5bf7e54291f242511"

using namespace Wally::Engines::Flickr;

Engine::Engine(QObject *parent) :
  ::Wally::Engines::Base(parent),
  _networkAccessManager(new QNetworkAccessManager(this))
{

}

void Engine::selectNext()
{
  QUrlQuery urlQuery;

  urlQuery.addQueryItem("method", "flickr.photos.search");
  urlQuery.addQueryItem("api_key", FLICKR_API_KEY);

  urlQuery.addQueryItem("tags", (QStringList() << "nature").join(","));
  // urlQuery.addQueryItem("tag_mode", (_tagsCondition == And)? "all" : "any");
  urlQuery.addQueryItem("tag_mode", "all");
  /*
  else if (!_text.isEmpty())
  {
    QString newText = _text;

    newText.replace(" ","+");
    url.addQueryItem("text",newText);
  }
  else
    return QUrl();
  */

  urlQuery.addQueryItem("media","photos");
  /*
  const QString searchOrderStrings[] = { "date-posted-asc", "date-posted-desc",
                                         "date-taken-asc", "date-taken-desc",
                                         "interestingness-asc", "interestingness-desc",
                                         "relevance" };
  */

  urlQuery.addQueryItem("sort", "relevance");
  urlQuery.addQueryItem("per_page","1");

  // url.addQueryItem("page",QString::number(pageIndex()));
  urlQuery.addQueryItem("page", "1");

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
      {
        /*
        newPagesCount = qMin(xmlResp.attributes().value("total").toString().toInt(),
                             FLICKR_PAGES_HARD_LIMIT);
        */
      }
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
        if ((xmlResp.attributes().value("height").toInt() *
             xmlResp.attributes().value("width").toInt()) > currentArea)
        {
          currentArea = xmlResp.attributes().value("height").toInt() * xmlResp.attributes().value("width").toInt();
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

  QFile f("/tmp/test.jpg");
  if (!f.open(QIODevice::WriteOnly))
  {
    emit failed();
    return;
  }

  f.write(networkReply->readAll());

  QUrlQuery urlQuery;
  urlQuery.addQueryItem("method", "flickr.photos.getInfo");
  urlQuery.addQueryItem("api_key", FLICKR_API_KEY);
  urlQuery.addQueryItem("photo_id", networkReply->property("photoId").toString());

  QUrl url("https://api.flickr.com/services/rest");
  url.setQuery(urlQuery);

  QNetworkReply *nextNetworkReply = _networkAccessManager->get(QNetworkRequest(url));
  nextNetworkReply->setProperty("outputFileName", "/tmp/test.jpg");
  connect(nextNetworkReply, SIGNAL(finished()), this, SLOT(processPhotoInfo()));
}

void Engine::processPhotoInfo()
{
  sender()->deleteLater();

  QNetworkReply *networkReply = qobject_cast< QNetworkReply * >(sender());
  if (networkReply && (networkReply->error() == QNetworkReply::NoError))
  {
    bool respOk = false;

    QXmlStreamReader xmlResp(networkReply->readAll());

    while (!xmlResp.atEnd())
      if (xmlResp.readNext() == QXmlStreamReader::StartElement)
      {
        if (!xmlResp.name().toString().compare("rsp", Qt::CaseInsensitive))
          respOk = !xmlResp.attributes().value("stat").toString().compare("ok", Qt::CaseInsensitive);
        else if (!xmlResp.name().toString().compare("title", Qt::CaseInsensitive))
        {
          xmlResp.readNext();
          // info.title = xmlResp.text().toString();
        }
        else if (!xmlResp.name().toString().compare("description", Qt::CaseInsensitive))
        {
          xmlResp.readNext();
          // info.description = xmlResp.text().toString();
        }
        else if (!xmlResp.name().toString().compare("owner", Qt::CaseInsensitive))
        {
          /*
          info.owner = xmlResp.attributes().value("realname").toString();
          if (info.owner.isEmpty())
            info.owner = xmlResp.attributes().value("username").toString();
          info.location = xmlResp.attributes().value("location").toString();
          */
        }
        else if (!xmlResp.name().toString().compare("url", Qt::CaseInsensitive) &&
                 !xmlResp.attributes().value("type").toString().compare("photopage", Qt::CaseInsensitive))
        {
          xmlResp.readNext();
          // info.sourceUrl = xmlResp.text().toString();
        }
      }
  }

  emit pictureAvailable(QFileInfo(networkReply->property("outputFileName").toString()));
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
