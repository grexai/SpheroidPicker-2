#include "comps.h"


cv::Mat getpcenter(const cv::Mat& cppoints){
    std::cout<< "pc" << std::endl;
    cv::Mat tmp = cppoints.col(0);
    cv::Mat pcenter;//(cv::Mat_<float>(3,1));
    tmp.copyTo(pcenter);
    std::cout <<"memory check"<< (unsigned int)(pcenter.data) << " vs " << (unsigned int)(cppoints.col(0).data) << std::endl;
    std::cout<< pcenter << std::endl;
    return pcenter;
}

cv::Mat geticenter(const cv::Mat& imagepoints){
    std::cout<< "ic" << std::endl;
    cv::Mat tmp;
    tmp = imagepoints.col(0);
    cv::Mat icenter;//(cv::Mat_<float>(2,1));
    tmp.copyTo(icenter);
    std::cout <<"memory check"<< (unsigned int)(icenter.data) << " vs " << (unsigned int)(imagepoints.col(0).data) << std::endl;
    std::cout<< icenter << std::endl;
    return icenter;
}


cv::Mat calcTMatrix(cv::Mat& cppoints,cv::Mat& imagepoints,centers &centers){
    std::cout<< "input cpp :"<< std::endl << cppoints << std::endl
             << "input imgp "<< std::endl<< imagepoints<< std::endl;
    using namespace  cv;
    //IMAGE
    Mat Pz = Mat(2,3,CV_32F);
    //PIPETTE
    Mat Ez= Mat (3,3,CV_32F);
    centers.img= geticenter(imagepoints);
    centers.pipette = getpcenter(cppoints);
    std::cout << "calcTM  imagecenters:" << centers.img <<
                 "pipettecenters: " << centers.pipette<<std::endl;

    for (int  i=0 ; i<3; i++){
        Ez.col(i)= cppoints.col(i) - centers.pipette;
    }
 //   std::cout<< Ez << std::endl;
    for (int  i=0 ; i<3; i++){
        Pz.col(i)= imagepoints.col(i) - centers.img;
    }
    Mat Pinv ;
    invert(Pz, Pinv, DECOMP_SVD);
    Mat T = Ez * Pinv;
    std::cout<< T << std::endl;
    return T;
};

cv::Mat calconimgpipettecoors(cv::Mat T,std::vector<float>mpos,
                                               cv::Mat& icenter,cv::Mat& pcenter){
    cv::Mat pipcoors;
    cv::Mat mousecoors;//(cv::Mat_<float>(2,1));
    mousecoors.push_back(mpos);
 //   std::cout << mousecoors << "mouse coors MAT "<< std:: endl;
 //   std::cout<< T << std::endl<< "tin"<< std::endl;
 //   std::cout<< icenter <<"icenter" <<std::endl;
 //   std::cout<< pcenter <<"pcenter" <<std::endl;
    cv::Mat mc_m_ic= mousecoors-icenter;
    //std::cout<< mc_m_ic << std::endl<< "mousecoorsminusicenter"<<std::endl;
    cv::Mat TM_mic = T*mc_m_ic;
    //std::cout<< TM_mic << std::endl<< "TM*mousecoorsminusicenter"<<std::endl;
    pipcoors = TM_mic+pcenter;
    // std::cout<< pipcoors<< std::endl<< "TM*mousecoorsminusicenterpluspcenter"<<std::endl;
    //  pipcoors =  T* (mousecoors-icenter)+pcenter;
    return pipcoors;
}
