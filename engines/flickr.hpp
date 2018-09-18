#ifndef WALLY_ENGINE_FLICKR_HPP
#define WALLY_ENGINE_FLICKR_HPP

#include "engine.hpp"

#include <QWidget>

class QNetworkAccessManager;

namespace Wally
{
  namespace Engines
  {
    namespace Flickr
    {
      class Engine : public ::Wally::Engines::Base
      {
        Q_OBJECT

        QNetworkAccessManager *_networkAccessManager;

      private slots:
        void processSearchResult();
        void processSizeQueryResult();
        void savePhoto();
        void processPhotoInfo();

      public:
        explicit Engine(QObject *parent = nullptr);

        ::Wally::Engines::SettingsWidget *settingsWidget(QWidget *parent) override;

      public slots:
        void selectNext() override;
      };

      class SettingsWidget : public ::Wally::Engines::SettingsWidget
      {
      public:
        explicit SettingsWidget(QWidget *parent = nullptr);

        void accept() override;
      };
    } // namespace Flickr
  } // namespace Engines
} // namespace Wally

#endif // WALLY_ENGINE_FLICKR_HPP
