#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <exception>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "segmentation.cpp"
#include "segmentor.cpp"
#include "segment.cpp"
#include "selector.cpp"


using namespace cv;


std::vector<Segment*> segmente;

int averageColor(Mat * image, int rows, int cols, int channel)
{
    int selectedChannel = 0;
    int otherChannel1 = 0;
    int otherChannel2 = 0;

    //select the correct color channel to filter
    switch(channel)
    {
        case 0:
            selectedChannel = 0;
            otherChannel1 = 1;
            otherChannel2 = 2;
            break;
        case 1:
            selectedChannel = 1;
            otherChannel1 = 0;
            otherChannel2 = 2;
            break;
        case 2:
            selectedChannel = 2;
            otherChannel1 = 0;
            otherChannel2 = 1;
            break;
    }


    int color = 0;
    int counter = 0;
    for(int y = 0; y < rows; y++)
    {
        for(int x = 0; x < cols; x++)
        {
            Vec3b point = image->at<Vec3b>(y,x);
            if(point[selectedChannel] > point[otherChannel1] && point[selectedChannel] > point[otherChannel2])
            {
                color += point[selectedChannel];
                counter++;
            }

        }
    }
    return (color/counter);
}



inline bool fileExists(const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

Mat * converting(int *** map, Mat * compare, Mat * result, int channel)
{
    int selectedChannel = 0;
    int otherChannel1 = 0;
    int otherChannel2 = 0;

    //select the correct color channel to filter
    switch(channel)
    {
        case 0:
            selectedChannel = 0;
            otherChannel1 = 1;
            otherChannel2 = 2;
            break;
        case 1:
            selectedChannel = 1;
            otherChannel1 = 0;
            otherChannel2 = 2;
            break;
        case 2:
            selectedChannel = 2;
            otherChannel1 = 0;
            otherChannel2 = 1;
            break;
    }

    int avg = averageColor(result, result->rows,result->cols, channel);
    
    for(int y = 0; y < compare->rows; y++)
    {
        for(int x = 0; x < compare->cols; x++)
        {
            Vec3b point = compare->at<Vec3b>(y,x);
           
            result->at<cv::Vec3b>(y,x)[0] = 0;
            result->at<cv::Vec3b>(y,x)[1] = 0;
            result->at<cv::Vec3b>(y,x)[2] = 0;
            map[0][y][x] = 0;
        
            if(point[selectedChannel] > point[otherChannel1] && point[selectedChannel] > point[otherChannel2])
            {
                        result->at<cv::Vec3b>(y,x)[0] = 255;
                        result->at<cv::Vec3b>(y,x)[1] = 255;
                        result->at<cv::Vec3b>(y,x)[2] = 255;
                        map[0][y][x] = 1;
                if(point[selectedChannel] < avg*0.5)
                {
                        result->at<cv::Vec3b>(y,x)[0] = 0;
                        result->at<cv::Vec3b>(y,x)[1] = 0;
                        result->at<cv::Vec3b>(y,x)[2] = 0;
                        map[0][y][x] = 0;
                }
            }
            
        }          

    }
   
    return result;
}




int main(int argc, char *argv[]) {   


    try{
        //std::cout << "starting" << std::endl;
        /*check parameters*/
        if(argc < 3)
        {
            
            throw runtime_error("Program expects at least 2 Parameters, " + to_string(argc-1) + " given.");
        
        }
            const string picture = argv[1];
            string folder = argv[2];

            folder = (folder.back() == '/') ? folder : folder+"/";

            string jsonFolder = argv[2];
            string jsonFile = "segmente.json";
            int color = 1;

            int minSize = 400;
            if(argc > 3)
            {
                if(argv[3][0] == '-')
                {
                    switch(argv[3][1])
                    {
                        case 'b':
                            color = 0;
                            break;
                        case 'g':
                            color = 1;
                            break;
                        case 'r':
                            color = 2;
                            break;
                    }
                }
                else
                {
                    jsonFolder = argv[3];
                }
            }
          
            
            

            if(!fileExists(picture))
            {
                throw runtime_error("File could not be found at " + picture);
            }
        
        
        

        
        bool debugbool = false;

        Mat image;
        Mat channels[3];
        
        //std::cout << "loading file" << std::endl;

        image = imread(picture);

        

        if( !image.data)
        {
            throw runtime_error("File: " + picture + " could not be loaded ");
        }



        int ** greenmap = new int*[image.rows];
        
        for(int i = 0; i < image.rows; i++)
        {
            greenmap[i] = new int[image.cols];
        }

        
        Mat compare;
        medianBlur(image, compare,  5);    

        Mat result = compare.clone();  
        Mat selectResult = compare.clone();  
        
        
        
        result = * converting(&greenmap,&compare,&result,color);

        int ** mapToSelect = new int*[result.rows];
        
        for (int y = 0; y < result.rows; y++) {
            mapToSelect[y] = new int[result.cols];
            for (int x = 0; x < result.cols; x++) {
                mapToSelect[y][x] = greenmap[y][x];
            }
        }


        Segmentor * harry = new Segmentor(&greenmap,0,result.rows,result.cols);
        //Selector * hermine = new Selector(&mapToSelect,5,result.rows,result.cols);
        

       

        jsonFile = (jsonFolder.back() != '/') ? jsonFolder : jsonFolder+"/"+jsonFile;
        
        ofstream outputJson(jsonFile);

        if(!fileExists(jsonFile))
        {
            throw runtime_error("Json: " + jsonFile + " could not be created ");
        }
        //std::cout << "exporting" << std::endl;
        //std::cout << "JSON" << std::endl;
        outputJson << "[\n";
        string data = "[\n";
        

        for(std::vector<myoSegment>::iterator it = harry->segmente.begin(); it != harry->segmente.end(); ++it)
        {
            if(it[0].height * it[0].width > 300)
            {
                Mat seg(it[0].height,it[0].width,  CV_8UC3);
                for(int y = 0; y < it[0].height; y++)
                {
                    for(int x = 0; x < it[0].width; x++)
                    {
                        if(x >= 0 && x < seg.cols && y >= 0 && y < seg.rows)
                        {
                            seg.at<cv::Vec3b>(y,x)[0] = 255 * it[0].map[y][x];
                            seg.at<cv::Vec3b>(y,x)[1] = 255 * it[0].map[y][x];
                            seg.at<cv::Vec3b>(y,x)[2] = 255 * it[0].map[y][x];
                        }
                    }
                }
            
                if(it != harry->segmente.begin() && it != harry->segmente.end())
                {
                    outputJson << ",\n";
                    data += ",\n";
                }
                string path = folder + "seg" + to_string(it[0].minX) + "-" + to_string(it[0].minY) + ".jpg";

                outputJson << "{\"path\":\""+path+"\",\"y\":"+to_string(it[0].minY)+",\"x\":"+to_string(it[0].minX)+",\"height\":"+to_string(it[0].height)+",\"width\":"+to_string(it[0].width)+"}";
                data += "{\"path\":\""+path+"\",\"y\":"+to_string(it[0].minY)+",\"x\":"+to_string(it[0].minX)+",\"height\":"+to_string(it[0].height)+",\"width\":"+to_string(it[0].width)+"}";

                
                imwrite(path, seg);
            }
        }
        data += "\n]";
        outputJson << "\n]";

        outputJson.close();
    
    

     

        for(std::vector<myoSegment>::iterator it = harry->segmente.begin(); it != harry->segmente.end(); ++it)
        {
            if(it[0].height * it[0].width > minSize)
            {
                Point p1(it[0].minX,it[0].minY);
                Point p2(it[0].minX+it[0].width,it[0].minY+it[0].height);

                rectangle(result,p1,p2,Scalar(0,0,255),2,LINE_8);
            }
        }
        
    	/*int redChannel[10] =   {0, 255, 225, 197, 169, 141, 112, 84,  28};
        int greenChannel[10] = {0, 225, 28,  56,  84,  112, 141, 169, 197};
        int blueChannel[10] =   {0, 225, 28,  56,  84,  112, 141, 169, 197};
        for(int x = 0; x < selectResult.cols; x++)
        {
            for(int y = 0; y < selectResult.rows; y++)
            {
                
                selectResult.at<cv::Vec3b>(y,x)[0] = blueChannel[hermine->map[y][x]];
                selectResult.at<cv::Vec3b>(y,x)[1] = greenChannel[hermine->map[y][x]];
                selectResult.at<cv::Vec3b>(y,x)[2] = redChannel[hermine->map[y][x]];
            }
        }
        namedWindow("Image", cv::WINDOW_NORMAL); // Create a resizable window
        imshow("Image", selectResult);
        waitKey(0);*/

        cout << "{\n status: \"ok\", \n data: " << data.c_str() << " \n}" << endl;



        return 0;
    }
    catch (const runtime_error& e) {
        
        cerr << "{\n status: \"error\", \n message: \"" << e.what() << "\", \n error: \"" << e.what() << "\" \n}" << endl;
    }
}