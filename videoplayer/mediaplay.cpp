#include "mediaplay.h"
#include "ui_mediaplay.h"
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QMediaContent>
#include <QTimer>
#include <QPixmap>
#include <QInputDialog>
mediaplay::mediaplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mediaplay)
{
    ui->setupUi(this);
    cur_vol = 100;

    //初始化变量
    srand(time(NULL));
    layer = new QVBoxLayout;
    player = new QMediaPlayer;
    VideoWidget = new QVideoWidget;    //创建视频新对象
    playlist = new QMediaPlaylist;
    fullflag = false;
    for(int i=0; i<100; i++)
        srcpath[i] = "-2";
    ui->screen->setVisible(false);
    ui->progress_bar->setEnabled(false);
    ui->volumn->setValue(100);  //初始化音量为100
    ui->progress_bar->setMinimum(0);  //设置进度条最小为零
    ui->progress_bar->setSingleStep(1);  //设置进度条每步走的大小
    playlist->setPlaybackMode(QMediaPlaylist::Loop);  //设置播放模式一开始为列表循环
    ui->pushButton_order->setStyleSheet("border-image: url(:/src/img/xunhuan.png);");
    player->setPlaybackRate(1.0);  //设置一开始的播放速度为1.0倍，即正常速度
    before.setRect(width()*645/794,0,width()*151/796,height()*40/47);  //记录播放列表框listwidget的位置
    start_up = true;  //设置当前为第一次打开状态
    //ui->show_hide->setStyleSheet("QPushButton{color:#000000}");  //设置按钮文字颜色为白色
    ui->show_hide->setEnabled(false);
    startplayer();
    ItemModel = new QStandardItemModel(this);
    listViewAddItem(0);//添加列表,0为视频列表，1为音乐列表

    //连接槽函数
    //获取当前播放的视频流的总时间
    QObject::connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(getduration(qint64)));

    //设置播放进度
    QObject::connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(myautosetValue(qint64)));
    QObject::connect(ui->progress_bar, SIGNAL(sliderMoved(int)), this, SLOT(mymanualsetValue(int)));
    //进度条相关
    QObject::connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(onPlayerDurationChanged(qint64)));
    QObject::connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(onPlayerPositionChanged(qint64)));
    QObject::connect(ui->progress_bar,SIGNAL(sliderMoved(int)),this,SLOT(onSliderMoved(int)));
    QObject::connect(ui->progress_bar,SIGNAL(sliderReleased()),this,SLOT(onSliderPositionChanged()));

    //设置音量
    QObject::connect(ui->volumn,SIGNAL(sliderMoved(int)),this,SLOT(setvolvalue(int)));
    QObject::connect(ui->volumn,SIGNAL(sliderReleased()),this,SLOT(on_trumpet_clicked()));

    //listwidget双击事件槽函数
     connect(ui->videolist,SIGNAL(clicked(QModelIndex)),this,SLOT(showClick(QModelIndex)));


    //设置窗口标题栏，实现实时显示当前播放的文件名字
    //QObject::connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(setwindowtitle(int)));

    //QObject::connect(ui->openURL,SIGNAL(sliderReleased()),this,SLOT(onSliderPositionChanged()));
    //设置播放顺序
    //QObject::connect(this,SIGNAL(playnext(int)),this,SLOT(setnext(int)));
    //QObject::connect(this,SIGNAL(playpre(int)),this,SLOT(setpre(int)));

    //监听播放媒体改变
    //QObject::connect(player,SIGNAL(currentMediaChanged(const QMediaContent)),this,SLOT(judgechangeway()));

    //静音
    QObject::connect(ui->trumpet,SIGNAL(clicked()),this,SLOT(mute()));

    //播放顺序切换
    QObject::connect(ui->pushButton_order,SIGNAL(clicked()),this,SLOT(changeplayorder()));

    //皮肤切换
    QObject::connect(ui->pushButton_pifu,SIGNAL(clicked()),this,SLOT(changepifu()));

    //媒体切换
    QObject::connect(ui->pushButton_mediatype,SIGNAL(clicked()),this,SLOT(changemediatype()));

    //播放按钮图片切换
    QObject::connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(changeplaybtnicon()));
}

mediaplay::~mediaplay()
{
    mediaplay::saveposition(player->position());
    delete ui;
}
void mediaplay::changeplaybtnicon()
{
    qDebug()<<player->state();
    if (player->state()!=QMediaPlayer::PlayingState)
        ui->pushButton_begin->setStyleSheet("border-image: url(:/src/img/play.png);");
    else if (player->state()!=QMediaPlayer::PausedState)
        ui->pushButton_begin->setStyleSheet("border-image: url(:/src/img/pause.png);");
    //if (player->state()==QMediaPlayer::StoppedState)
        //mediaplay::on_pushButton_next_clicked();
}

void mediaplay::changemediatype()
{
    qDebug()<<"changemediatype";
    currentlist^=1;
    playlist->clear();
    player->stop();
    btnenalbe=false;
    if (currentlist==0)
    {
        start_up=true;
        ui->pushButton_mediatype->setStyleSheet("border-image: url(:/src/img/video.png);");
    }
    else
    {
        start_up=true;
        ui->screen->setVisible(false);
        ui->show_hide->setEnabled(false);
        ui->videolist->setStyleSheet("background-color:transparent;color: rgb(165, 165, 165);font-size:30px;border:none;");
        ui->pushButton_mediatype->setStyleSheet("border-image: url(:/src/img/music.png);");
    }
    listViewAddItem(currentlist);
    //mediaplay::playfirstmedia();
    qDebug()<<"fchangemediatype";
}
void mediaplay::changepifu()
{
    pifuindex+=1;
    pifuindex%=pifulist.count();
    qDebug()<<pifuindex;
    ui->frame_2->setStyleSheet(pifulist[pifuindex]);
}
void mediaplay::changeplayorder()
{
    playorder+=1;
    playorder%=3;
    switch (playorder+1) {
    case 0:
        ui->pushButton_order->setStyleSheet("border-image: url(:/src/img/shunxu.png);");
        playlist->setPlaybackMode(QMediaPlaylist::Sequential);
        break;
    case 1:
        ui->pushButton_order->setStyleSheet("border-image: url(:/src/img/xunhuan.png);");
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        break;
    case 2:
        ui->pushButton_order->setStyleSheet("border-image: url(:/src/img/suiji.png);");
        playlist->setPlaybackMode(QMediaPlaylist::Random);
        break;
    case 3:
        ui->pushButton_order->setStyleSheet("border-image: url(:/src/img/1xunhuan.png);");
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        break;




    }
}
void mediaplay::playfirstmedia()
{
    return ;
    qDebug()<<"playfirstmedia";
    if (playlist->mediaCount()>0)
    {
        playlist->setCurrentIndex(0);
        player->play();
    }
    qDebug()<<"fplayfirstmedia";
}
void mediaplay::mute()
{
    if (!mutestatus)
    {
        volume=player->volume();
        ui->volumn->setValue(0);
        player->setVolume(0);
        ui->trumpet->setStyleSheet("border-image: url(:/src/img/jingyin.png);");
    }
    else
    {
        player->setVolume(volume);
        ui->volumn->setValue(volume);
        ui->trumpet->setStyleSheet("border-image: url(:/src/img/yinliang.png);");
    }
    mutestatus=!mutestatus;
}

void mediaplay::judgechangeway()
{
    qDebug()<<playlist->mediaCount()<<" "<<namelist.count();
    if (playlist->mediaCount()==0||namelist.count()==0||playlist->currentIndex()==-1)
    {
        this->setWindowTitle("星空播放器");
        lastname="mp3";
        return;
    }
    //qDebug()<<"judge "<<mediachangetype;
    //if (mediachangetype==0)
        //emit playnext(playlist->previousIndex());
    if (mediasourse==0)
    {
        this->setWindowTitle(namelist[playlist->currentIndex()]);
        //qDebug()<<playlist->mediaCount()<<" "<<namelist.count();
        lastname=this->windowTitle();
    }
    else
    {
        this->setWindowTitle("网络资源.mp4");
        lastname="mp3";
    }
    //qDebug()<<"judge "<<mediachangetype;
}
void mediaplay::saveposition(qint64 pos)
{
    qDebug()<<"sp";
    if (mediasourse!=0||start_up||currentlist!=0||lastname.mid(lastname.length()-3,lastname.length()) == "mp3")
    {
        qDebug()<<"return";
        return;
    }
    int index=playlist->currentIndex();
    if(pos==player->duration())
        pos=0;
    qSqliteDb.ModifyViideoCurrentTime(filepath[index],pos);
    qDebug()<<"save "<<index<<": "<<pos;
}
//获取视频的时间长度，并用label显示
void mediaplay::getduration(qint64 playtime)
{
    int h,m,s;
    playtime = player->duration();
    playtime /= 1000;
    h = playtime/3600;
    m = (playtime-h*3600)/60;
    s = playtime-h*3600-m*60;
    QString str;
    ui->label_time->setText(str.asprintf("%02d:%02d:%02d",h,m,s));
    //ui->label_time->setStyleSheet("color:black");
    if(playtime != 0)
    {
        ui->progress_bar->setMaximum(playtime);
    }
}

//播放视频的初始化操作
void mediaplay::startplayer()
{
    VideoWidget->installEventFilter(this);  //把VideoWidget加入到过滤器中，以捕获键盘/鼠标事件

    layer->setMargin(0);     //初始化布局

    //指定视频播放的位置为screen的位置，screen为label控件
    VideoWidget->resize(ui->screen->size());    //视频部件和UI的screen连接起来
    layer->addWidget(VideoWidget);
    ui->screen->setLayout(layer);

    //设置输出
    player->setVideoOutput(VideoWidget);  //将QVideoWidget视频输出附加到媒体播放器。
                                          //如果媒体播放器已经附加了视频输出，它将被替换为一个新的。

    player->setPlaylist(playlist);  //设置播放器的播放列表

    player->setPosition(0);  //设置播放器的播放进度，一开始置为0

    player->play();  //开始播放
    //设置焦点
    VideoWidget->setFocus();
}

//重写事件过滤器
bool mediaplay::eventFilter(QObject *recv,QEvent *e)
{
    //定义播放状态标志位，可以用QMediaPlayer::StoppedState这些来代替
    static bool pause_flag = false;

    //判断被观察者是否为视频
    if(recv == VideoWidget)
    {
        //判断事件是否为键盘事件
        if(e->type() == QEvent::KeyPress)
        {
            QKeyEvent *mykey = (QKeyEvent *)e;
            //判断是哪一类键盘事件
            switch(mykey->key())
            {
                case Qt::Key_Escape:
                    if (currentlist==1)
                        break;
                     fullflag = false;
                VideoWidget->setFullScreen(fullflag);
                        break;

                //回车键，控制是否全屏
                case Qt::Key_Return:
                        if (currentlist==1)
                            break;
                        fullflag=!fullflag;
                        VideoWidget->setFullScreen(fullflag);
                    break;

                //方向键左，控制播放进度，快退
                case Qt::Key_Left:
                        curpos = player->position();
                        curpos -= 5000;
                        player->setPosition(curpos);
                    break;

                //方向键右，控制播放进度，快进
                case Qt::Key_Right:
                        curpos = player->position();
                        curpos += 5000;
                        player->setPosition(curpos);
                     break;

                //方向键上，控制音量，增大
                case Qt::Key_Up:
                        if(cur_vol < 100)
                            cur_vol+=5;
                        player->setVolume(cur_vol);
                        ui->volumn->setValue(cur_vol);
                    break;

                //方向键下，控制音量，减少
                case Qt::Key_Down:
                        if(cur_vol > 0)
                            cur_vol-=5;
                        ui->volumn->setValue(cur_vol);
                        player->setVolume(cur_vol);
                     break;

                //控制键，控制播放/暂停
                case Qt::Key_Space:
                        if(pause_flag == false)
                        {
                            player->pause();
                            pause_flag = true;

                        }
                        else if(pause_flag == true)
                        {
                            player->play();
                            pause_flag = false; 
                        }
                        changeplaybtnicon();
                 break;
            }
        }
    }

    //判断是否为鼠标双击事件，用于鼠标双击控制全屏显示
    if(e->type() == QEvent::MouseButtonDblClick&&currentlist==0)
    {
        //因为双击事件也会触发单击事件，导致视频暂停，所以双击事件需要让视频继续播放
        player->play();
        if(fullflag == true)
            fullflag = false;
        else
            fullflag = true;
        VideoWidget->setFullScreen(fullflag);  //设置全屏
    }

    //判断是否为鼠标按下事件，用于鼠标单击控制暂停/播放
    if(e->type() == QEvent::MouseButtonPress&&currentlist==0)
    {
        VideoWidget->setFocus();
        if(pause_flag == false)
        {
            player->pause();
            pause_flag = true;
        }
        else if(pause_flag == true)
        {
            player->play();
            pause_flag = false;
        }
        changeplaybtnicon();
    }
    return QObject::eventFilter(recv,e);
}

//打开按钮槽函数，用于打开流文件
void mediaplay::on_pushButton_open_clicked()
{
    QStringList filepath = QFileDialog::getOpenFileNames(this, tr("选择转码文件"), tr("/home"), tr("视频文件(*.mp4 *.mp3);;所有文件（*.*);;"));
    if(!filepath.empty())
        qSqliteDb.Insert(filepath);
    listViewAddItem(currentlist);
}

//上一个（首）按钮槽函数
void mediaplay::on_pushButton_last_clicked()
{
    if (btnenalbe==false)
        return;
    /*
    //判断列表索引是否已越界
    if(playlist->previousIndex() == -1)
       return;

    //上一个（首）
    playlist->previous();
    */
    //emit playpre(playlist->currentIndex());
    playlist->previous();
    player->setMedia(playlist->currentMedia());
    player->play();
    QModelIndex qindex = ItemModel->index(playlist->currentIndex(),0);
    ui->videolist->setCurrentIndex(qindex);
    //判断当前状态，如果是停止状态，就让视频开始播放
    /*if(player->state() != QMediaPlayer::PlayingState)
    {
        player->play();

    }*/
}

//开始/暂停按钮槽函数
void mediaplay::on_pushButton_begin_clicked()
{
    //判断是否第一次打开本软件
    //if(start_up == true)
        //return;
    if (btnenalbe==false)
        return;

    //判断当前状态，决定是开始播放还是暂停
    if(player->state() == QMediaPlayer::StoppedState)
        startplayer();
    else if(player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
        ui->pushButton_begin->setStyleSheet("border-image: url(:/src/img/play.png);");
    }
    else if(player->state() == QMediaPlayer::PausedState)
    {
        player->play();
        ui->pushButton_begin->setStyleSheet("border-image: url(:/src/img/pause.png);");
    }
}

//下一首（个）按钮槽函数
void mediaplay::on_pushButton_next_clicked()
{
    qDebug()<<"next";
    //判断列表索引是否已越界
    //if(playlist->nextIndex() == -1)
        //return;
    if (btnenalbe==false)
        return;

    //下一首（个）
    //playlist->next();
    playlist->setCurrentIndex(playlist->nextIndex());
    player->setMedia(playlist->currentMedia());
    QModelIndex qindex = ItemModel->index(playlist->currentIndex(),0);
    qDebug()<<"next"<<playlist->currentIndex();
    ui->videolist->setCurrentIndex(qindex);
    player->play();
    //emit playnext(playlist->currentIndex());
    /*if(player->state() != QMediaPlayer::PlayingState)
    {
        player->play();
    }*/
}

//停止按钮槽函数
void mediaplay::on_pushButton_stop_clicked()
{
    //停止播放
    player->stop();

    //设置屏幕显示初始时的图片
    QPixmap pix(":/video.jpg");
    pix = pix.scaled(ui->screen->size());
    ui->screen->setPixmap(pix);
}

//全屏按钮槽函数
void mediaplay::on_pushButton_fullscreen_clicked()
{
    if(currentlist==1||player->state()==QMediaPlayer::StoppedState)
        return;
    if(fullflag == true)
        fullflag = false;
    else
        fullflag = true;
    VideoWidget->setFullScreen(fullflag);
}

//重写listwidget的双击事件，实现双击播放
//void mediaplay::doubleclick_play(QListWidgetItem *item)
//{

//    for(int i=0; i<100; i++)
//    {
//        if(i%2 != 0)
//            if(srcpath[i] == item->text())
//            {
//                playlist->setCurrentIndex(srcpath[i-1].toInt());
//                judge_str = srcpath[i].section('.', 1, 1);
//                /*if(judge_str == "mp3")
//                    ui->screen->setPixmap(pix);*/

//                //如果当前状态为停止或者暂停，都让视频开始播放
//                if(player->state() == QMediaPlayer::StoppedState)
//                     player->play();
//                else if(player->state() == QMediaPlayer::PausedState)
//                    player->play();

//            }
//    }

//}

//自动调节播放进度，显示在滑动条上，并在label上显示时间
void mediaplay::myautosetValue(qint64 cur_val)
{
    static int h,m,s;
    cur_val /= 1000;
    h = cur_val/3600;
    m = (cur_val-h*3600)/60;
    s = cur_val-h*3600-m*60;
    QString str;
    ui->label_playtime->setText(str.asprintf("%02d:%02d:%02d",h,m,s));
    //ui->label_playtime->setStyleSheet("color:black");
    ui->progress_bar->setValue(cur_val);
    //qSqliteDb.ModifyViideoCurrentTime();
}

//手动调节播放进度，显示在滑动条上，并在label上显示时间
void mediaplay::mymanualsetValue(int cur_val)
{
    static int h,m,s;
    h = cur_val/3600;
    m = cur_val/60;
    s = cur_val%60;
    QString str;
    ui->label_playtime->setText(str.asprintf("%02d:%02d:%02d",h,m,s));
    //ui->label_playtime->setStyleSheet("color:black");

    player->setPosition(cur_val*1000);
}

//设置音量
void mediaplay::setvolvalue(int cur_val)
{
    cur_val = ui->volumn->value();
    player->setVolume(cur_val);
}

//设置窗口标题
void mediaplay::setwindowtitle(int pos)
{
    for(int i=0; i<100; i+=2)
    {
        if(srcpath[i].toInt() == pos)
        {
            this->setWindowTitle(srcpath[i+1]);
        }
    }
}

//音量按钮槽函数
void mediaplay::on_trumpet_clicked()
{
    player->setVolume(ui->volumn->value());
//    static bool noiseless = false;
//    if(!noiseless)
//    {
//        player->setVolume(0);
//        ui->volumn->setValue(0);
//        noiseless = true;
//    }
//    else
//    {
//        player->setVolume(cur_vol);
//        ui->volumn->setValue(cur_vol);
//        noiseless = false;
//    }
}

//播放列表显示/隐藏按钮槽函数
void mediaplay::on_show_hide_clicked()
{
    static bool v_flag = false;
    if(!v_flag)
    {
        ui->videolist->setVisible(false);
        v_flag = true;
        ui->show_hide->setText("《");
    }
    else
    {
        ui->videolist->setVisible(true);
        ui->videolist->setGeometry(before);
        v_flag = false;
        ui->show_hide->setText("》");
    }
    this->resize(this->size() - QSize(1,1));

    if(judge_str == "mp3")
    {
        QPixmap pix(":/music.jpg");
        pix = pix.scaled(ui->screen->size());
        ui->screen->setPixmap(pix);
    }
    else
    {
        QPixmap pix(":/video.jpg");
        pix = pix.scaled(ui->screen->size());
        ui->screen->setPixmap(pix);
    }
    this->resize(this->size() + QSize(1,1));
}
//进度条点击事件
void mediaplay::onPlayerDurationChanged(qint64 duration)
{
    //qDebug()<<duration;
    ui->progress_bar->setMaximum(duration);
}
void mediaplay::onPlayerPositionChanged(qint64 position)
{
    //qDebug()<<position;
    //int index=playlist->currentIndex();
    mediaplay::judgechangeway();

    ui->progress_bar ->setValue(position);

}
void mediaplay::onSliderMoved(int value)
{
    player->setPosition(value);
}
void mediaplay::onSliderPositionChanged()
{
    player->setPosition(ui->progress_bar->value());
}
//URL资源打开
void mediaplay::on_openURL_clicked()
{

    if (mediasourse==0)
        mediaplay::saveposition(player->position());
    mediasourse=1;
    QString url =
            QInputDialog::getText(this, tr("Open Url"), tr("Enter the URL you want to play"));

    if (url.isEmpty())
    {
        //player->stop();
        //if (currentlist==1)
            //ui->screen->setVisible(false);
        start_up=true;
        return;
    }
    ui->screen->setVisible(true);
    btnenalbe=true;
    currentlist=0;
    listViewAddItem(currentlist);
    ui->progress_bar->setEnabled(true);
    ui->show_hide->setEnabled(true);
    ui->videolist->setStyleSheet("background-color:transparent;color: rgb(165, 165, 165);font-size:18px;border:none;");
    player->setMedia(QUrl(url));
    layer->setMargin(0);     //初始化布局

    //指定视频播放的位置为screen的位置，screen为label控件
    VideoWidget->resize(ui->screen->size());    //视频部件和UI的screen连接起来
    layer->addWidget(VideoWidget);
    ui->screen->setLayout(layer);

    //设置输出
    player->setVideoOutput(VideoWidget);  //将QVideoWidget视频输出附加到媒体播放器。
    player->play();
}
//播放列表
void mediaplay::listViewAddItem(int judge)//0为视频列表，1为音乐列表
{
    qDebug()<<"listViewAddItem";
    ItemModel->clear();
    QSqlQuery sql_query;
    filepath.clear();
    playlist->clear();
    namelist.clear();
    if(judge == 0)
        sql_query = qSqliteDb.GetVideoList();
    else if(judge == 1)
        sql_query = qSqliteDb.GetAudioList();

    while(sql_query.next())
    {
        QString string = static_cast<QString>(sql_query.value(0).toString());
        filepath.append( sql_query.value(1).toString());
        namelist.append(string);
        qDebug()<<"q "<<string;
        QStandardItem *item = new QStandardItem(string);
        ItemModel->appendRow(item);
    }
    for(int j = 0;j<filepath.length();j++)
    {
        playlist->addMedia(QUrl::fromLocalFile(filepath[j]));
        qDebug()<<filepath[j];
        //qDebug()<<playlist;
    }
    ui->videolist->setModel(ItemModel);
    qDebug()<<"flistViewAddItem"<<namelist.count()<<" "<<playlist->mediaCount();
}
//播放列表点击相应
void mediaplay::showClick(QModelIndex index)
{
    btnenalbe=true;
    if (currentlist==0)
    {
        ui->screen->setVisible(true);
        ui->show_hide->setEnabled(true);
        ui->videolist->setStyleSheet("background-color:transparent;color: rgb(165, 165, 165);font-size:18px;border:none;");
    }


    mediasourse=0;
    if (!start_up)
        mediaplay::saveposition(player->position());
    ui->progress_bar->setEnabled(true);
    start_up=false;
    mediachangetype=1;
    playlist->setCurrentIndex(index.row());
    player->setMedia(playlist->currentMedia());
    qint64 pos=qSqliteDb.Search(filepath[index.row()]);
    //if(1.0*(abs(pos-player->duration())/player->duration())<0.01)
        //pos=0;
    //pos=0;



    player->setPosition(pos);
    //ui->progress_bar->setValue(pos);
    player->play();
    mediachangetype=0;
    qDebug()<<"click:"<<index.row()<<"  "<<pos;
}
/*void mediaplay::setpre(int index)
{
    if (mediasourse!=0)
        return;
    qDebug()<<"# "<<index;
    int num=playlist->mediaCount();
    mediachangetype=1;
    saveposition(player->position());
    qDebug()<<num;
    switch (playorder) {
        case 0:
            index-=1;
            if (index<0)
            {
                player->stop();
                return;
            }
        break;
        case 1:
            index-=1;
            index+=num;
            index%=num;
        break;
        case 2:
            index=rand()%num;
        break;
        case 3:
        break;

    }
    qDebug()<<"## "<<index;
    playlist->setCurrentIndex(index);
    player->setMedia(playlist->currentMedia());
    player->setPosition(0);//qSqliteDb.Search(filepath[index]));

    if(player->state() != QMediaPlayer::PlayingState)
    {

        player->play();
    }
    mediachangetype=0;
}*/
/*void mediaplay::setnext(int index)
{
    if (mediasourse!=0)
        return;
    qDebug()<<"next "<<index;
    int num=playlist->mediaCount();
    mediachangetype=1;
    saveposition(player->position());
    switch (playorder) {
        case 0:
            index+=1;
            if (num==index)
            {
                player->stop();
                return;
            }
        break;
        case 1:
            index+=1;
            index%=num;
        break;
        case 2:
            index=rand()%num;
        break;
        case 3:
        break;

    }
    playlist->setCurrentIndex(index);
    player->setMedia(playlist->currentMedia());
    player->setPosition(0);//qSqliteDb.Search(filepath[index]));

    if(player->state() != QMediaPlayer::PlayingState)
    {

        player->play();
    }
    mediachangetype=0;
}
*/
