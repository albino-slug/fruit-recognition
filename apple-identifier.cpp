#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <algorithm>
using namespace cv;
using namespace std;

// STEP 0 - UTILITY FUNCTIONS

int clamp(int x, int min, int max){
        if (x < min) return min;
        if (x > max) return max;
        return x;
}

void display_image(Mat image){
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", image);
        waitKey(0);
}

Mat get_image(char* received_image, int opening_type){
        Mat image;
        image = imread(received_image, opening_type);
        return image;
}

Mat get_redness_buffer(Mat source_image){
        Mat redness_buffer = Mat::zeros(source_image.rows, source_image.cols, CV_8UC1);

        for (int i = 0; i < source_image.rows; i++){
                for (int j = 0; j < source_image.cols; j++){
                        Vec3b pixel = source_image.at<Vec3b>(i, j);
                        redness_buffer.at<uchar>(i, j)
                        = clamp(3 * pixel[2] - pixel[1] - pixel[0], 0, 3 * 255) / 3;
                }
        }

        return redness_buffer;
}

// STEPT 1 - TEXTURE BUFFER

int get_rednesss_level(int value){
        return value / 16;
}

Mat get_co_occurrence_matrix(Mat source_image, int x, int y, int axis){//0 on j, > 0 on i
        Mat co_occurence = Mat::zeros(16, 16, CV_8UC1);
        if (axis == 0){
                for(int i = x-2; i < x+3; i++){
                        for(int j = y-2; j < y; j++){

                                co_occurence.at<uchar>(
                                        get_rednesss_level(source_image.at<uchar>(i, j)),
                                        get_rednesss_level(source_image.at<uchar>(i, j + 3)))
                                += 1;

                                co_occurence.at<uchar>(
                                        get_rednesss_level(source_image.at<uchar>(i, j + 3)),
                                        get_rednesss_level(source_image.at<uchar>(i, j)))
                                += 1;
                        }
                }
        }
        else{
                for(int i = x-2; i < x; i++){
                        for(int j = y - 2; j < y + 2; j++){

                                co_occurence.at<uchar>(
                                        get_rednesss_level(source_image.at<uchar>(i, j)),
                                        get_rednesss_level(source_image.at<uchar>(i + 3, j)))
                                += 1;

                                co_occurence.at<uchar>(
                                        get_rednesss_level(source_image.at<uchar>(i + 3, j)),
                                        get_rednesss_level(source_image.at<uchar>(i, j)))
                                += 1;
                        }
                }
        }
        return co_occurence;
}

uchar get_contrast(Mat co_occurrence_matrix){
        int contrast_value = 0;

        for (int i = 0; i < 16; i++){
                for (int j = 0; j < 16; j++){
                        contrast_value +=
                        co_occurrence_matrix.at<uchar>(i,j) * (i - j) * (i - j);
                }
        }

        return (uchar)clamp(contrast_value,0,255);
}

Mat get_texture_buffer(Mat source_image){
        Mat texture_buffer = Mat::zeros(source_image.rows, source_image.cols, CV_8UC1);

        for(int i = 2; i < source_image.rows-2; i++){
                for(int j = 2; j < source_image.cols-2; j++){
                        texture_buffer.at<uchar>(i, j) =
                        get_contrast(get_co_occurrence_matrix(source_image, i, j, 0));
                }
        }
        return texture_buffer;
}

// în plus
Mat red_tex(Mat r, Mat t){
        Mat rt = Mat::zeros(t.rows, t.cols, CV_8UC1);

        for(int i = 0; i < t.rows; i++){
                for(int j = 0; j < t.cols; j++){
                        rt.at<uchar>(i,j) = r.at<uchar>(i,j) / (t.at<uchar>(i,j) + 0.0000001);
                }
        }

        return rt;
}

// STEP 2 - CANNY EDGE DETECTION

//TODO add ISE filter before applying Canny

Mat edge_detection(Mat source_image){
        Mat destination_image = Mat::zeros(source_image.rows, source_image.cols, CV_8UC3);

        Canny(source_image, destination_image, 50, 200, 3);

        return destination_image;
}

// STEP 3

// STEP 4

// STEP X - APPLY PIPELINE
int main(int argc, char** argv ){
        if ( argc != 2 ){
            printf("usage: DisplayImage.out <Image_Path>\n");
            return -1;
        }
        int op;
        do{
                system("cls");
                destroyAllWindows();
                printf("OPTIONS:\n\n");
                printf("\t\t0  -  EXIT\n\n");
                printf("\t\t1  -  TEST IMAGE OPEN\n");
                printf("\t\t2  -  TEST REDNESS BUFFER\n");
                printf("\t\t3  -  TEST CANNY\n");
                printf("\t\t4  -  TEST TEXTURE BUFFER\n");
                printf("\t\t5  -  TEST INVENȚIA LUI ©VIRGIL\n");
                printf("\t\t10 -  TEST CURRENT PIPELINE VERSION\n\n");
                printf("PLS GIV MI OPTION: ");
                scanf("%d",&op);
        	switch (op){
                case 1: display_image(get_image(argv[1], 1));
                break;

        	case 2: display_image(get_redness_buffer(get_image(argv[1], 1)));
                break;

                case 3: display_image(edge_detection(get_image(argv[1], 0)));
                break;

                case 4: display_image(get_texture_buffer(get_image(argv[1], 0)));
                break;

                case 5: display_image(red_tex(get_redness_buffer(get_image(argv[1], 1)), get_texture_buffer(get_redness_buffer(get_image(argv[1], 1)))));
                break;

                case 10: display_image(edge_detection(get_texture_buffer(get_redness_buffer(get_image(argv[1], 1)))));
                break;
                }
        }
        while (op!=0);

    return 0;
}
