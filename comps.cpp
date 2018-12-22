#include "comps.h"


extern cv::Mat getpcenter(cv::Mat& cppoints){
    //eCenter
     std::cout<< "pc" << std::endl;
    cv::Mat pcenter(cv::Mat_<float>(3,1));

    pcenter = cppoints.col(0);
    std::cout<< pcenter << std::endl;
    return pcenter;
}

extern cv::Mat geticenter(cv::Mat& imagepoints){
     std::cout<< "ic" << std::endl;
    cv::Mat icenter(cv::Mat_<float>(2,1));
    icenter = imagepoints.col(0);
    std::cout<< icenter << std::endl;
    return icenter;
}

extern cv::Mat coorsToMatrix(std::vector<float> in_vec, int coordims){
    int l = in_vec.size();
    int h= coordims;
    int w= l/coordims;
    cv::Mat outputMatrix;

    outputMatrix.reshape(w,h);
    return outputMatrix;
}


extern cv::Mat getTMatrix(cv::Mat& cppoints,cv::Mat& imagepoints){
    std::cout<< "input cpp :"<< std::endl << cppoints << std::endl
             << "input imgp "<< std::endl<< imagepoints<< std::endl;

    using namespace  cv;

    cv::Mat icenter = geticenter(imagepoints);
    cv::Mat pcenter = getpcenter(cppoints);



   // Mat Pz= Mat (cv::Mat_<int>(2,3));
  //  Mat Ez= Mat (cv::Mat_<int>(3,3));
    Mat Ez= Mat (3,3,CV_32F);
    Mat Pz = Mat(2,3,CV_32F);
    for (int  i=0 ; i<3; i++){
        Ez.col(i)= cppoints.col(i) - pcenter;
    }
    std::cout<< Ez << std::endl;

    for (int  i=0 ; i<3; i++){
        Pz.col(i)= imagepoints.col(i) - icenter;
    }

    std::cout << "Pz" << std::endl << Pz<< std::endl;

    Mat Pinv ;

    invert(Pz, Pinv, DECOMP_SVD);

    Mat T = Ez * Pinv;
    std::cout<< T << std::endl;


    return T;


};

extern cv::Mat getonimgpipettecoors(cv::Mat T,std::vector<float>mpos,
                                               cv::Mat& icenter,cv::Mat& pcenter){
    cv::Mat pipcoors;
    cv::Mat mousecoors;//(cv::Mat_<float>(2,1));
    mousecoors.push_back(mpos);
    std::cout << mousecoors << "mouse coors MAT "<< std:: endl;
    std::cout<< T << std::endl<< "tin"<< std::endl;
    std::cout<< icenter <<"icenter" <<std::endl;
    std::cout<< pcenter <<"pcenter" <<std::endl;
    cv::Mat mc_m_ic= mousecoors-icenter;
    std::cout<< mc_m_ic << std::endl<< "mousecoorsminusicenter"<<std::endl;
    cv::Mat TM_mic = T*mc_m_ic;
    std::cout<< TM_mic << std::endl<< "TM*mousecoorsminusicenter"<<std::endl;
    pipcoors = TM_mic+pcenter;
      std::cout<< pipcoors<< std::endl<< "TM*mousecoorsminusicenterpluspcenter"<<std::endl;
//   pipcoors =  T* (mousecoors-icenter)+pcenter;


    return pipcoors;
}














