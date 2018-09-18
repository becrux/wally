#ifndef WALLY_SETTINGSWINDOW_HPP
#define WALLY_SETTINGSWINDOW_HPP

#include <QDialog>

namespace Wally
{
  class SettingsWindow : public QDialog
  {
    Q_OBJECT

  public:
    explicit SettingsWindow(QWidget *parent = nullptr);
  };
} // namespace Wally

#endif // WALLY_SETTINGSWINDOW_HPP
