#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#include <QThread>
#include <QTimer>
#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QProgressDialog>

#include <imagetools.h>
#include <stagecontroller.h>
#include <calibratedialog.h>
#include <comps.h>
#include <controller.h>
#include <cameracv.h>
#include <deeplearning.h>
#include <propertyreader.h>
#include <auto_methods.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    bool Iscameraopen= false;
    int cameraIndex=0;
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setdarkstyle();
    void setdefault();
    cv::Mat TM;
signals:
    // The signal of transmit the coordinates of the mouse position
   // void signalTargetCoordinate(QPointF point);
    void prog_changed(int progress);
    void stich_prog_changed(int progress);
    void scan_finished();
    void close_and_return_hw();
public slots:
   // void show_hw_selector();
    void scan_stopped();
    void set_progressbar( int value );
    void set_stich_progressbar( int value );
    void set_h4(int value);
    void set_pip_man(int value);
protected:
    QProgressDialog* m_stich_prog= nullptr;
    int m_progvalue=0;
    int m_stich_progvalue=0;
    controller* ctrl = nullptr;
    CameraCV* cameracv = nullptr;
    imagetools* imtools= nullptr;
    i_deeplearning* dl= nullptr;
    propertyreader* propreader = nullptr;
    auto_methods* automethods = nullptr;

    std::vector<float>* cpos1 = nullptr;
    std::vector<float>* cpos2 = nullptr;
    std::vector<float>* cpos3= nullptr;
    QPoint* point_mouse= nullptr;

private slots:
    // events
    bool eventFilter(QObject *obj,  QEvent *event);

    void keyPressEvent( QKeyEvent* e );

    void closeEvent (QCloseEvent *event);

    // View

    void calib_frame_view(cv::Mat& disp);

    void update_window();

   // void update_currentpressure();
 //   void show_currentpressure();

    //right click actions

    void move_action();

    void center_this_point_action();

    void on_graphicsView_customContextMenuRequested(const QPoint &pos);

    // Menus

    void on_actionLight_triggered();

    void on_actionDark_Mode_triggered();

    void on_actionCalibrate_Pipette_triggered();

//    void on_actionSpheroid_picker_triggered();

    void on_actionExit_triggered();

    void on_actionHW_selector_triggered();

  //  void on_actionOpen_console_triggered();

    // buttons

    void on_Campushbtn_clicked();

    void update_currentpressure();

     void show_currentpressure();

     void on_Home_pip_clicked();

     void on_p_home_x_clicked();

     void on_p_home_y_clicked();

     void on_p_home_z_clicked();

     void on_SetPressure_clicked();

     void on_atm_button_clicked();

     void on_pc_pulse_button_clicked();

     void on_get_coors_pushButton_clicked();

     void on_s_xp_button_clicked();

     void on_s_xm_button_clicked();

     void on_s_yp_button_clicked();

     void on_s_ym_button_clicked();

     void on_p_xp_button_clicked();

     void on_p_xm_button_clicked();

     void on_p_yp_button_clicked();

     void on_p_ym_button_clicked();

     void on_p_zp_button_clicked();

     void on_p_zm_btton_clicked();

     void on_s_center_button_clicked();

     void on_exptime_button_clicked();

     void on_width_button_clicked();

     void on_height_button_clicked();

     void on_save_image_button_clicked();

     void on_s_get_coors_pushButton_clicked();

     void on_start_screening_clicked();

     void on_p_set_speed_spinbox_valueChanged(int arg1);

     void on_s_speed_spinbox_valueChanged(int arg1);

     void on_p_ep_button_clicked();

     void on_p_em_button_clicked();

     void on_pickup_sph_clicked();

     void on_s_accel_spinbox_valueChanged(int arg1);

     void on_save_m_p_button_clicked();

     void on_predict_sph_clicked();

     void on_pick_and_put_clicked();

     void on_petri_b_clicked();

     void on_view_scan_clicked();

     //auto pickup,scanning methods and helpers

     void lock_ui();

     void unlock_ui();

     void pickup_sph();

     std::vector<float> get_centered_coordinates(std::vector<float>& sph_coors);

     void center_spheroid(std::vector<float>& coors);

     void center_selected_sph(int index);

     void move_to_petri_B();

     void xz_stage_pickup_sph();

     void pick_and_put();

     //scanning

     void predict_sph();

     std::string get_date_time_str();

     void screensample();

     void create_mosaic();

     void on_found_objects_highlighted(int index);

private:
    QTimer *timer= nullptr;
    QTimer *disp_pressure= nullptr;
    QGraphicsScene* scene= nullptr;
    QGraphicsPixmapItem  qpxmi;
    QGraphicsPixmapItem  im_view_pxmi;
    QImage* qframe = nullptr;
    QImage* qfrm_t2= nullptr;
    Ui::MainWindow *ui= nullptr;
    calibratedialog *calib= nullptr;

    std::vector<cv::Mat> scanvector;
    std::vector<std::vector<float>>*global_obj_im_coordinates=nullptr;


    std::atomic_bool m_s_t_acitive;

    //threads
    std::thread* m_screening_thread= nullptr;
    std::thread* m_picking_thread= nullptr;
    std::thread* m_predict_thread = nullptr;
    std::thread* m_move_petri_b_thread = nullptr;
    std::thread* m_put_and_pick_thread = nullptr;
    std::thread* m_create_mosaic_thread = nullptr;
    std::thread* m_center_selected_sph_thread = nullptr;
    //TEST
    float m_status = 0;
    QTimer *timer1= nullptr ;
    QThread *thread1= nullptr;
    float mid_s_x_p=0.0f;
};

#endif // MAINWINDOW_H
