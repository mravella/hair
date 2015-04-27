#include "sceneeditor.h"
#include "ui_sceneeditor.h"

SceneEditor::SceneEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SceneEditor)
{
    ui->setupUi(this);
}

SceneEditor::~SceneEditor()
{
    delete ui;
}
