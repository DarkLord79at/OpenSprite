#include "fileimport.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sprite.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    opt.show_grid_lines = true;
    opt.spriteview = this->ui->graphicsView;

    this->ui->graphicsView->set_opt(&opt);
    this->ui->label_palette->showPalette(&opt);


    leftradio.addButton(this->ui->radio_transparent_left,BUTTONS::TRANSPARENT);
    leftradio.addButton(this->ui->radio_sprite_left,BUTTONS::COLOR);
    leftradio.addButton(this->ui->radio_mc1_left,BUTTONS::MC1);
    leftradio.addButton(this->ui->radio_mc2_left,BUTTONS::MC2);
    leftradio.addButton(this->ui->radio_overlay_left,BUTTONS::OVERLAY);

    rightradio.addButton(this->ui->radio_transparent_right,BUTTONS::TRANSPARENT);
    rightradio.addButton(this->ui->radio_sprite_right,BUTTONS::COLOR);
    rightradio.addButton(this->ui->radio_mc1_right,BUTTONS::MC1);
    rightradio.addButton(this->ui->radio_mc2_right,BUTTONS::MC2);
    rightradio.addButton(this->ui->radio_overlay_right,BUTTONS::OVERLAY);
    this->ui->radio_transparent_right->setChecked(true);

    connect(&leftradio,&QButtonGroup::idToggled, this, [=](int id, bool checked){if (checked) this->opt.left_button = id;});
    connect(&rightradio,&QButtonGroup::idToggled, this, [=](int id, bool checked){if (checked) this->opt.right_button = id;});

    connect(this->ui->label_palette, &Palette::palette_clicked, this, [=](int button, int id){
        if ((button == Qt::LeftButton && leftradio.checkedId() == BUTTONS::TRANSPARENT)
                || (button == Qt::RightButton && rightradio.checkedId() == BUTTONS::TRANSPARENT))
            this->ui->combo_transparent->setCurrentIndex(id);

        if ((button == Qt::LeftButton && leftradio.checkedId() == BUTTONS::COLOR)
                || (button == Qt::RightButton && rightradio.checkedId() == BUTTONS::COLOR))
            this->ui->combo_sprite_col->setCurrentIndex(id);

        if ((button == Qt::LeftButton && leftradio.checkedId() == BUTTONS::MC1)
                || (button == Qt::RightButton && rightradio.checkedId() == BUTTONS::MC1))
            this->ui->combo_multicol_1->setCurrentIndex(id);

        if ((button == Qt::LeftButton && leftradio.checkedId() == BUTTONS::MC2)
                || (button == Qt::RightButton && rightradio.checkedId() == BUTTONS::MC2))
            this->ui->combo_multicol_2->setCurrentIndex(id);

        if ((button == Qt::LeftButton && leftradio.checkedId() == BUTTONS::OVERLAY)
                || (button == Qt::RightButton && rightradio.checkedId() == BUTTONS::OVERLAY))
            this->ui->combo_overlay->setCurrentIndex(id);
    });

    for (int i = 0; i < 16; i++)
    {
        this->ui->combo_transparent->addItem(opt.col_names.at(i));
        this->ui->combo_transparent->setItemIcon(i,this->createIconFromColor(opt.col_list.at(i)));
        this->ui->combo_sprite_col->addItem(opt.col_names.at(i));
        this->ui->combo_sprite_col->setItemIcon(i,this->createIconFromColor(opt.col_list.at(i)));
        this->ui->combo_multicol_1->addItem(opt.col_names.at(i));
        this->ui->combo_multicol_1->setItemIcon(i,this->createIconFromColor(opt.col_list.at(i)));
        this->ui->combo_multicol_2->addItem(opt.col_names.at(i));
        this->ui->combo_multicol_2->setItemIcon(i,this->createIconFromColor(opt.col_list.at(i)));
        this->ui->combo_overlay->addItem(opt.col_names.at(i));
        this->ui->combo_overlay->setItemIcon(i,this->createIconFromColor(opt.col_list.at(i)));
    }
    connect(this->ui->combo_transparent, &QComboBox::currentIndexChanged, this, [=](int index){opt.background = index; this->ui->graphicsView->setBackgroundBrush(opt.col_list.at(index));});
    connect(this->ui->combo_sprite_col, &QComboBox::currentIndexChanged, this, [=](int index){opt.sprite_list.at(current_sprite)->sprite_color = index; this->ui->graphicsView->scene()->update();});
    connect(this->ui->combo_multicol_1, &QComboBox::currentIndexChanged, this, [=](int index){opt.mc1 = index; this->ui->graphicsView->scene()->update();});
    connect(this->ui->combo_multicol_2, &QComboBox::currentIndexChanged, this, [=](int index){opt.mc2 = index; this->ui->graphicsView->scene()->update();});
    //connect(this->ui->combo_overlay, &QComboBox::currentIndexChanged, this, [=](int index){opt.background = index;});


    connect(this->ui->graphicsView, &SpriteView::current_sprite_changed, this, [=](int id){
        this->current_sprite = id;
        this->ui->combo_sprite_col->setCurrentIndex(opt.sprite_list.at(id)->sprite_color);
        this->ui->check_multicolor->setChecked(opt.sprite_list.at(id)->multi_color_mode);
    });


    connect(this->ui->check_multicolor, &QCheckBox::toggled, this, [=](bool val){opt.sprite_list.at(this->current_sprite)->multi_color_mode = val; this->ui->graphicsView->scene()->update();});

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionImport_triggered()
{
    QSettings settings;
    QString path = QFileDialog::getOpenFileName(this, "File", settings.value("last_file", QVariant()).toString());//, "Sprite Files(*.spd, *.prg)");
    if (path != "")
    {
        FileImport(path, &opt);
        settings.setValue("last_file", path);
        this->ui->combo_multicol_1->setCurrentIndex(opt.mc1);
        this->ui->combo_multicol_2->setCurrentIndex(opt.mc2);
        this->ui->combo_transparent->setCurrentIndex(opt.background);
        this->ui->graphicsView->change_current_sprite(0);
        this->ui->graphicsView->redraw();
    }
}

