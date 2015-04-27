#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include <QMainWindow>

namespace Ui {
class SceneEditor;
}

class SceneEditor : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SceneEditor(QWidget *parent = 0);
    ~SceneEditor();
    
private:
    Ui::SceneEditor *ui;
};

#endif // SCENEEDITOR_H
