#ifndef PAUSADIALOG_H
#define PAUSADIALOG_H

#include <QDialog>

namespace Ui {
class PausaDialog;
}

class PausaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PausaDialog(QWidget *parent = nullptr);
    ~PausaDialog();

signals:
    void reiniciarClicked();
    void salirClicked();

private:
    Ui::PausaDialog *ui;
};

#endif // PAUSADIALOG_H
