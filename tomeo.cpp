//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//
using namespace std;
#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QMenuBar>
#include <QLineEdit>
#include <QScrollArea>
#include <QFormLayout>
#include <QGridLayout>
#include <QSlider>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>
#include "the_player.h"
#include "the_button.h"

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}

int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {

        const int result = QMessageBox::information(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    //create main window
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 550);
    window.setMaximumSize(1800, 1000);
    window.setStyleSheet("background-color: #181818");
    //media player
    ThePlayer *player = new ThePlayer;

    //menu bar of the prototype
        QMenuBar *menuBar = new QMenuBar;
        QMenu *file = new QMenu("File");
        file->addAction("Open File");
        file->addAction("Open Disc");
        file->addAction("Open Network");
        file->addAction("Open Capture Device");
        file->addAction("Open Recent");
        file->addAction("Close Window");
        file->addAction("Convert / Stream");
        file->addAction("Save Playlist");
        QAction *fileAct = new QAction();

        QMenu *edit = new QMenu("Edit");
        edit->addAction("Undo");
        edit->addAction("Redo");
        edit->addAction("Cut");
        edit->addAction("Copy");
        edit->addAction("Paste");
        edit->addAction("Delete");
        edit->addAction("Select All");
        edit->addAction("Find");

        QMenu *view = new QMenu("View");
        view->addAction("Show Tab Bar");
        view->addAction("Show All Tab");
        view->addAction("Show Previous & Next");
        view->addAction("Show Shuffle & Repeat");
        view->addAction("Show Audio Effects");
        view->addAction("Show Sidebar");
        view->addAction("Playlist Table Columns");

        QMenu *audio = new QMenu("Audio");
        audio->addAction("Increase Volume");
        audio->addAction("Decrease Volume");
        audio->addAction("Mute");
        audio->addAction("Audio Track");
        audio->addAction("Sterio Audio Mode");
        audio->addAction("Visualisations");
        audio->addAction("Audio Device");

        QMenu *videoMenu = new QMenu("Video");
        videoMenu->addAction("Full Screen");
        videoMenu->addAction("Aspect Ratio");
        videoMenu->addAction("Crop");
        videoMenu->addAction("Deinterlace");
        videoMenu->addAction("Deinterlace Mode");
        videoMenu->addAction("Post Processing");
        videoMenu->addAction("Video Track");

        QMenu *subtitle = new QMenu("Subtitle");
        subtitle->addAction("Add Subtitle File");
        subtitle->addAction("Subtitle Track");
        subtitle->addAction("Text Size");
        subtitle->addAction("Text Color");
        subtitle->addAction("Outline Thickness");

        QMenu *help = new QMenu("Help");
        help->addAction("License");
        help->addAction("Outline Documentation");
        help->addAction("Online Forum");
        help->addAction("Make A Donation");
        help->addAction("Outline Thickness");

        menuBar->addMenu(file);
        menuBar->addMenu(edit);
        menuBar->addMenu(view);
        menuBar->addMenu(audio);
        menuBar->addMenu(videoMenu);
        menuBar->addMenu(subtitle);
        menuBar->addMenu(help);
        menuBar->setStyleSheet("border: 1px solid black; background-color: #222120; color: white; font-family: Serif");

        top->setMenuBar(menuBar);

    // the widget that will show the whole video layout
    QVBoxLayout *videoLayout = new QVBoxLayout();
    QWidget *video = new QWidget();
    QVideoWidget *videoWidget = new QVideoWidget;
    videoWidget->setStyleSheet("QVideoWidget{background-color: #181818");

    // the QMediaPlayer which controls the playback
    player->setVideoOutput(videoWidget);

    //title bar
    QWidget *titleBar = new QWidget();
    QHBoxLayout *titleBarLayout = new QHBoxLayout();
    QLabel *title = new QLabel();
    title->setText("VIDEO TITLE");
    title->setStyleSheet("color: white;font-family: Serif");

    QPushButton *fullscreen = new QPushButton();
    fullscreen->setIcon(QIcon(":/fullscreen.png"));
    fullscreen->setFixedSize(QSize(30,30));
    fullscreen->setIconSize(QSize(30,30));
    fullscreen->setStyleSheet("QPushButton{border: none}");

    //Dropdown Menu
    QMenu *dotsButtonMenu = new QMenu();
    dotsButtonMenu->addAction("Properties");
    QMenu *playbackSpeed = new QMenu("Playback Speed");
    playbackSpeed->addAction("0.25x");
    playbackSpeed->addAction("0.50x");
    playbackSpeed->addAction("0.75x");
    playbackSpeed->addAction("1x");
    playbackSpeed->addAction("1.25x");
    playbackSpeed->addAction("1.50x");
    playbackSpeed->addAction("1.75x");
    playbackSpeed->addAction("2x");
    dotsButtonMenu->addMenu(playbackSpeed);
    dotsButtonMenu->addAction("Equaliser");
    dotsButtonMenu->addAction("Cast to Device");
    QPushButton *dotsButton = new QPushButton();
    dotsButton->setFixedSize(QSize(25, 25));
    dotsButton->setStyleSheet("border: none");
    dotsButton->setIcon(QIcon(":/feature.png"));
    dotsButton->setIconSize(QSize(25,25));
    dotsButton->setStyleSheet("font-weight: bold");
    dotsButton->setMenu(dotsButtonMenu);
    titleBarLayout->addWidget(title);
    titleBarLayout->addStretch();
    titleBarLayout->addWidget(fullscreen);
    titleBarLayout->addWidget(dotsButton);
    titleBar->setLayout(titleBarLayout);

    videoWidget->setStyleSheet("background-color: #181818; border: none");

   // a row of buttons
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged horizontally
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *library = new QLabel();
    library->setText("My Library");
    library->setStyleSheet("color: white; font-family: Serif");
    layout->addWidget(library);

    QWidget *search = new QWidget();
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLineEdit *searchBox = new QLineEdit();
    searchBox->setPlaceholderText("Search");
    searchBox->setMaximumSize(170, 25);
    searchBox->setStyleSheet("background-color: white;border-radius: 5px; font-family: Serif");
    QPushButton *searchButton = new QPushButton();
    searchButton->setIcon(QIcon(":/searchicon.png"));
    searchButton->setIconSize(QSize(20,20));
    searchLayout->addWidget(searchBox);
    searchLayout->addWidget(searchButton);
    search->setLayout(searchLayout);
    layout->addWidget(search);

    // create the four buttons
    for ( int i = 0; i < 7; i++ ) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);
        layout->addWidget(button);
        layout->setStretch(i, 2);
        button->init(&videos.at(i));
        button->setStyleSheet("border: none");
        layout->setSpacing(20);
    }

    QPushButton *addNew = new QPushButton("+Add New");
    addNew->setMaximumSize(200, 25);
    layout->addWidget(addNew);
    addNew->setStyleSheet("background-color: white;font-family:Serif");
    QMessageBox *addNewBox= new QMessageBox();
    addNewBox->setWindowTitle("Add New");
    addNewBox->setText("Loading Video...");
    addNewBox->setStandardButtons(QMessageBox::Ok);
    addNewBox->connect(addNew, SIGNAL(clicked()), addNewBox, SLOT(exec()));
    buttonWidget->setLayout(layout);
    buttonWidget->setStyleSheet("background-color: #181818;border: none");
    QScrollArea *scrollarea = new QScrollArea();
    scrollarea->setWidgetResizable(true);
    scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollarea->setWidget(buttonWidget);
    scrollarea->setStyleSheet("QScrollArea{background-color: ##181818}");
    scrollarea->setStyleSheet("QScrollBar{background-color: #181818; border: grey; border: 1px }");

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    //video slider and time label
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    QWidget *videoSliderWidget = new QWidget();
    QLabel *time = new QLabel("00:00:38");
    time->setStyleSheet("color: white; font-family: Serif");
    QSlider *videoSlider = new QSlider(Qt::Horizontal);
    videoSlider->connect(player, &QMediaPlayer::durationChanged, videoSlider, &QSlider::setMaximum);
    videoSlider->connect(player, &QMediaPlayer::positionChanged, videoSlider, &QSlider::setValue);
    videoSlider->connect(videoSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    sliderLayout->addWidget(videoSlider);
    sliderLayout->addWidget(time);
    videoSliderWidget->setStyleSheet("QSlider::handle{background-color: white; border-radius: 12px}");
    videoSliderWidget->setLayout(sliderLayout);

    //Widget that holds the video buttons
    QWidget *videoButton = new QWidget();
    QHBoxLayout *videoButtonLayout = new QHBoxLayout();
    const QSize btnsize = QSize(40, 40);

    QPushButton *backward = new QPushButton();
    backward->setFixedSize(btnsize);
    backward->setIcon(QIcon(":/backwards.png"));
    backward->setIconSize(QSize(30,30));
    backward->setStyleSheet("border: none");

    QPushButton *forward = new QPushButton();
    forward->setFixedSize(btnsize);
    forward->setIcon(QIcon(":/forwards.png"));
    forward->setIconSize(QSize(30,30));
    forward->setStyleSheet("border: none");
//    forward->connect(forward, SIGNAL(clicked()), videoSlider, SLOT(setValue(20)));

    QPushButton *play = new QPushButton();
    play->setFixedSize(btnsize);
    play->setIcon(QIcon(":/play.png"));
    play->setIconSize(QSize(30,30));
    play->setStyleSheet("border: none");
    play->connect(play, SIGNAL(clicked()), player, SLOT(play()));

    QPushButton *pause = new QPushButton();
    pause->setFixedSize(btnsize);
    pause->setIcon(QIcon(":/pauseicon.png"));
    pause->setIconSize(QSize(50,50));
    pause->setStyleSheet("QPushButton{border: none}");
    pause->connect(pause, SIGNAL(clicked()), player, SLOT(pause()));
    pause->connect(pause, SIGNAL(clicked()), pause, SLOT(setEnabled(false)));

    QPushButton *replay = new QPushButton();
    replay->setFixedSize(btnsize);
    replay->setIcon(QIcon(":/whitereplay.png"));
    replay->setIconSize(QSize(30,30));
    replay->setStyleSheet("QPushButton{border: none}");
    replay->connect(replay, SIGNAL(clicked()), player, SLOT(stop()));

    QPushButton *volume = new QPushButton();
    volume->setIcon(QIcon(":/volumeicon.png"));
//    volume->setFixedSize(btnsize);
    volume->setIconSize(QSize(40,40));
    volume->isFlat();
    volume->setStyleSheet("QPushButton{border: none}");

    QPushButton *mute = new QPushButton();
    mute->setIcon(QIcon(":/muteicon.png"));
    volume->setFixedSize(btnsize);
    mute->setIconSize(QSize(30,25));
    mute->isFlat();
    mute->setStyleSheet("QPushButton{border: none}");

    QPushButton *favourite = new QPushButton();
    favourite->setIcon(QIcon(":/love.png"));
    favourite->setFixedSize(btnsize);
    favourite->setIconSize(QSize(30,30));
    favourite->setStyleSheet("QPushButton{border: none}");
    QMessageBox *favouriteBox= new QMessageBox();
    favouriteBox->setWindowTitle("Favourites");
    favouriteBox->setText("Video successfully added to Favourites!");
    favouriteBox->setStandardButtons(QMessageBox::Ok);
    favouriteBox->setIcon(QMessageBox::Information);
    favouriteBox->connect(favourite, SIGNAL(clicked()), favouriteBox, SLOT(exec()));

    QPushButton *share = new QPushButton();
    share->setIcon(QIcon(":/shareicon.png"));
    share->setFixedSize(btnsize);
    share->setIconSize(QSize(25,25));
    share->setStyleSheet("QPushButton{border: none}");
    QMessageBox *shareBox= new QMessageBox();
    shareBox->setWindowTitle("Share Video");
    shareBox->setText("Link successfully copied to clipboard!");
    shareBox->setStandardButtons(QMessageBox::Ok);
    shareBox->setIcon(QMessageBox::Information);
    shareBox->connect(share, SIGNAL(clicked()), shareBox, SLOT(exec()));

    QPushButton *bin = new QPushButton();
    bin->setIcon(QIcon(":/binicon.png"));
    bin->setFixedSize(btnsize);
    bin->setIconSize(QSize(25,25));
    bin->setStyleSheet("QPushButton{border: none}");
    QMessageBox *deleteBox= new QMessageBox();
    deleteBox->setWindowTitle("Delete Video");
    deleteBox->setText("Are you sure you want to delete this video?");
    deleteBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    deleteBox->setIcon(QMessageBox::Question);
    deleteBox->setDefaultButton(QMessageBox::Yes);
    bin->connect(bin, SIGNAL(clicked()), deleteBox, SLOT(exec()));

    videoButtonLayout->addWidget(replay);
    videoButtonLayout->addWidget(play);
    videoButtonLayout->addWidget(pause);
    videoButtonLayout->addWidget(backward);
    videoButtonLayout->addWidget(forward);

    videoButtonLayout->addStretch();
    videoButtonLayout->addWidget(mute);
    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    videoButtonLayout->addWidget(volumeSlider);
    videoButtonLayout->addWidget(volume);
    videoButton->setLayout(videoButtonLayout);
    volumeSlider->setStyleSheet("QSlider::handle{background-color: white}");

    //Video Slider and Timeline
    video->setLayout(videoLayout);


    //details and other buttons
    QWidget *details = new QWidget();
    QHBoxLayout *detailsLayout = new QHBoxLayout();
    QLabel *videoDetails = new QLabel();
    videoDetails->setText("VIDEO DETAILS");
    videoDetails->setStyleSheet("color: white; font-family: Serif");
    detailsLayout->addWidget(videoDetails);
 //    detailsLayout->addStretch();
    detailsLayout->addWidget(favourite);
    detailsLayout->addWidget(share);
    detailsLayout->addWidget(bin);
    details->setLayout(detailsLayout);

    //add everything into the video layout
    videoLayout->addWidget(titleBar);
    videoLayout->addWidget(videoWidget);
    videoLayout->addWidget(videoSliderWidget);
    videoLayout->addWidget(videoButton);
    videoLayout->addWidget(details);


    //layout to video and buttons
    QHBoxLayout *body = new QHBoxLayout();


    video->setStyleSheet("background-color: #181818");
    body->addWidget(video);
    body->addWidget(scrollarea);
    body->setStretchFactor(video, 2);
    body->setStretchFactor(buttonWidget, 1);



    // add the video and the buttons to the top level widget
    top->addLayout(body);


    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
