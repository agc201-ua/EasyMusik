#ifndef FINDIALOG_H
#define FINDIALOG_H

#include <QDialog>

namespace Ui {
    class FinDialog;
}

class FinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FinDialog(QWidget *parent = nullptr);
    ~FinDialog();

signals:
    void reiniciarClicked();
    void salirClicked();

private:
    Ui::FinDialog *ui;
};

#endif
