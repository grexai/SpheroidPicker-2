#include "auto_methods.h"

auto_methods::auto_methods()
{
}
auto_methods::~auto_methods(){}


void auto_methods::pickup_sph(float pulse_value,float pulse_time,std::vector<float> objpos){
  //   QTextStream(stdout) << "pciking up" << endl;
       this->pipette_move_to_img_coordinates(objpos);
       std::this_thread::sleep_for(std::chrono::milliseconds(7000));
       this->vaccum_pulse(pulse_value,pulse_time);
       std::this_thread::sleep_for(std::chrono::milliseconds(3000));
       this->pipette_home_z();
}

void auto_methods::scan_sample(std::atomic_bool &m_s_t_acitive){
    using namespace  cv;
    int platesize= 350000;      //    100nm
    float  img_w_5p5x = 27426;  //    100nm
    float  img_h_5p5x = 15421;  //    100nm

    std::string folder = "Scandata_";
    std::string datetime = "_";
    std::stringstream ss;
    ss << (platesize/10000);
    std::string plate_size_mm = ss.str();
    folder.append(plate_size_mm).append("mm_");
    datetime.append((QString::number(QDate::currentDate().year())).toStdString());
    datetime.append((QString::number(QDate::currentDate().month())).toStdString());
    datetime.append((QString::number(QDate::currentDate().day())).toStdString()+"-");
    datetime.append((QString::number(QTime::currentTime().hour())).toStdString());
    datetime.append((QString::number(QTime::currentTime().minute())).toStdString());
    folder.append(datetime+"/");
    if (QDir().exists(folder.c_str())){
        QTextStream(stdout) << "this folder already folder exists"<< endl;
    }
    else{
        QTextStream(stdout)<< folder.c_str()<< "created" << endl;
        QDir().mkdir(folder.c_str());
    }
    QTextStream(stdout)<< "starting..";
    int xpos=this->stage_get_x_coords();
    int ypos=this->stage_get_y_coords();
    int wmax = static_cast<int>(platesize/img_w_5p5x); // um
    int hmax = static_cast<int>(platesize/img_h_5p5x); // um
    this->stage_set_speed(7500.0f);
    QTextStream(stdout)<< "wmax: "<<wmax << " hmax" << hmax;
    int counter = 1;
    float p_v=0.0f;

    folder.append("s_"+datetime);

    for (int j = 0; j < hmax; ++j )
    {
        for (int  i = 0; i< wmax; ++i)
        {
            if(m_s_t_acitive)
            {
                this->stage_move_to_x_sync(static_cast<int>(xpos+img_w_5p5x*i));
                p_v= static_cast<float>((wmax+1)*j+i)/static_cast<float>((hmax+1)*(wmax+1))*100;
           //     prog_changed(static_cast<int>(p_v));
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                int leading = 2;
                std::string num2str= folder + "_H" + std::to_string(j*0.000001).substr(8-leading)+ "_W" + std::to_string(i*0.000001).substr(8-leading);
                std::string posy = std::to_string(j)+ "/" + std::to_string(hmax);
                std::string posx = std::to_string(i)+ "/" + std::to_string(wmax);

          //    ui->current_scaningpos->setText(("Scaning pos: W: "+posx +" H: " + posy).c_str() );
          //    auto tmp = cameracv->get_current_frm();
           //   imtools->saveImg(tmp.get(),num2str.c_str());
             //   scanvector.push_back(*tmp.get());
                counter += 1;
            }else{
                break;
            }
        }
        this->stage_set_speed(10000.0f);
        this->stage_move_to_y_sync(static_cast<int>(ypos+img_h_5p5x*j));
        this->stage_set_speed(7000.0f);


}
}
