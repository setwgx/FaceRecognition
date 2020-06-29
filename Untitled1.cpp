#include<iostream>
#include<cstring>
#include<cstdlib>
#include<cmath>
#include <iomanip> 
#include <windows.h>
#include <fstream>
using namespace std;

const int N = 32*32;//输入层神经元个数 
const int L = 16*16;//隐含层神经元个数 
const int M = 16;//输出层神经元个数 

double epsilon = 0.01;//精度
double alpha = 0.2;//学习率 

double V[N][L];//输入层到隐含层的权值 
double W[L][M];//隐含层到输出层的权值 
double phi[L];//隐含层的阈值
double theta[M];//输出层的阈值
	
double delteV[N][L];//调整阈值时 V的变化量 
double delteW[L][M];//调整阈值时 W的变化量 
double deltephi[L];//调整阈值时隐含层阈值的变化量 
double deltetheta[M];//调整阈值时输出层阈值的变化量  

void Initialization();//初始化函数 
double F(double x);//激励函数 
void GetPixel(double *array, char *imagepath);//获得图片灰度值 
int Training(double *X, double *D);//神经网络训练 
int Match(double *result, double *X);//匹配志愿者编号 

int main()
{
	cout<<"运行很慢，请稍等 ~ QAQ"<<endl;
	cout<<"经过测算，大概需要45s左右..."<<endl; 
	Initialization();//调用函数，初始化数据 
	int times = 200;//定义循环次数，最多循环 200次，如果还没有达到精度就放弃 
	while(times-->0){
		for(int p = 1;p <= 5; p++){//定义志愿者的第一张照片，只训练每个志愿者的前5张图片
			for(int volunteer = 1;volunteer < M; volunteer++){;//定义志愿者编号，序号从 1-15 
				char imagepath[50];//定义 char型数组，用于保存图片的路径 
				memset(imagepath,0,sizeof(imagepath));
				char start_num[3];//志愿者编号 
				char end_num[3];//图片编号 
				memset(start_num,0,sizeof(start_num));
				memset(end_num,0,sizeof(end_num));
				int x1 = volunteer % 10;//获取志愿者编号的两位数字 
				int x2 = volunteer / 10;
				start_num[0] = (x2+'0');//转换成字符型保存在数组中，十位数部分 
				start_num[1] = (x1+'0');//个位数部分 
				start_num[2] = '\0';//加终止符，防止打印出乱码 
				
				if(p<10){//如果照片序号小于10 
					int y1 = p % 10;
					end_num[0] = (y1+'0');//直接保存一位到数组 
					end_num[1] = '\0';
				}else{//如果是十位数则同志愿者编号 
					int y1 = p%10;
					int y2 = p / 10;
					end_num[0] = (y2+'0');
					end_num[1] = (y1+'0');
					end_num[2] = '\0';
				}//因为照片的不需要前面的 0，加入第三章照片就直接输入 3而志愿者编号则需要 03 
				strcat(imagepath,"./YALE/subject");//使用strcat函数处理，找到正确的路径 
				strcat(imagepath,start_num);
				strcat(imagepath,"_");
				strcat(imagepath,end_num);
				strcat(imagepath,".bmp");
				
				double X[N];//定义需要保存图片灰度值的数组 
				memset(X,0,sizeof(X));
				GetPixel(X,imagepath);//引用函数获取灰度值 
				double D[M];//表示期望的输出 
				memset(D,0,sizeof(D));
				D[volunteer] = 1.0;//如果是第 volunteer个志愿者则希望匹配率为 100%则第 volunteer位置为 1 
				int flag = Training(X, D);//调用函数，进行训练 
				if(flag) {break;}//如果达到精度了则不需要进行再一次的训练 
			}
		}
	}
	
	int num = 10;	
	while(num--){//循环输入测试样例 
		cout<<endl;
		cout<<"请分别输入志愿者编号 和 照片序号 --> 例如(01 , 1)，个位数志愿者编号需要前面添加 0"<<endl;//分别输入志愿者编号和照片的数量 
		char t1[3];
		char t2[3];
		memset(t1,0,sizeof(t1));
		memset(t2,0,sizeof(t2));
		cin>>t1;//输入志愿者编号 
		cin>>t2;//照片编号 
		char imagepath[50];//定义路径 
		memset(imagepath,0,sizeof(imagepath));
		strcat(imagepath,"./YALE/subject");
		strcat(imagepath,t1);
		strcat(imagepath,"_");
		strcat(imagepath,t2);
		strcat(imagepath,".bmp");//使用strcat函数进行组合 
		cout<<"图片路径为："<<endl;
		cout<<imagepath<<endl;//打印看看 
		double X[N];//定义需要保存图片灰度值的数组 
		memset(X,0,sizeof(X));
		double result[M];//用来计算保存最后输出的结果 
		memset(result,0,sizeof(result));
		GetPixel(X, imagepath);//获得灰度值 
		int index = Match(result, X);//进行结果的判断 ，返回志愿者编号 
		cout<<"与各个志愿者照片匹配率如下"<<endl;
		for(int i=1; i<M; i++){
			cout<<"volunteer"<<i<<" --> "<<(double)result[i]*100.0<<"%"<<endl;//打印匹配率 
		}
		cout<<"最终匹配到的志愿者编号是："<<index<<endl<<endl;//打印志愿者编号 
	}
	return 0;
} 

/*
	Name:Initialization()
	Date: 06/06/20 15:42
	Description:没有返回值，用于初始化全局变量数据 
				包括隐藏层到输出层的二维数组 W，和输入层到隐藏的二维数组 V 
				以及一维数组隐含层阈值数组，一维数组输出层阈值数组 
				为他们生成一些接近于 0的随机值 
*/
void Initialization(){
	for(int i = 0; i < N; i++){
		for(int j = 0; j < L; j++){
			V[i][j] = (rand()%100/10000.0);//生成很接近于 0的随机数，精度为小数点后4位小数 
		}
	}
	for(int j = 0; j < L; j++){
		for(int k = 1; k < M; k++){
			W[j][k] = (rand()%100/10000.0);
		}
	}
	for(int j = 0; j < L; j++){
		phi[j] = (rand()%100/10000.0);
	}
	for(int k = 1; k < M; k++){
		theta[k] = (rand()%100/10000.0);
	}
}

/*
	Name: F() 
	Date: 06/06/20 15:48 
	Description:双精度返回值，用于计算激励函数的值 f(x) = 1/(1+e^-x)
*/
double F(double x){
	return 1.0 / (1.0 + exp(-1.0*x));   
}

/*
	Name:GetPixel()
	Date: 06/06/20 16:52 
	Description:没有返回值，两个参数一个双精度二维数组，一个char型数组
				用于将 imagepath路径下的图片转换成灰度值的百分比，并且保存到array数组中
				将指针返回
				首先使用 FILE命令创建一个指针 fp打开 imagepath路径下的文件，然后使用 fseek函数跳过.bmp图片格式的头结构
				定义 head指针获取该图片的必要的信息，比如图片的宽和高
				获取 BIBitCount变量，即每行像素所占字节数，获取灰度图像的颜色表有多少字节
				定义颜色表指针，申请空间，将色彩读进内存
				寻找到 fp指向文件的颜色表，申请位图数据指针，通过这个指针获取图片的灰度值
				并且转化为百分比返回数组 
*/
void GetPixel(double *array, char *imagepath){
	FILE *fp = fopen(imagepath,"rb");//打开路径下的文件 
	if(fp == NULL){
		cout<<"phil to open the file !"<<endl;
		return ;
	}
	fseek(fp, sizeof(BITMAPFILEHEADER),0);//跳过位图文件头结构BITMAPFILEHEADER
	BITMAPINFOHEADER head;  
	fread(&head, sizeof(BITMAPINFOHEADER), 1,fp);//获取图像宽、高、每像素所占位数等信息
	int bmpWidth = head.biWidth;//图片的宽 
	int bmpHeight = head.biHeight;//图片的高 
	
	int biBitCount = head.biBitCount;//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	int lineByte = (bmpWidth * biBitCount/8+3)/4*4;//灰度图像有颜色表，且颜色表表项为256
	RGBQUAD *pColorTable;//颜色表指针
	if(biBitCount == 8)
	{
		//申请颜色表所需要的空间，读颜色表进内存
		pColorTable = new RGBQUAD[256];
		fread(pColorTable,sizeof(RGBQUAD),256,fp);
	}
	//申请位图数据所需要的空间，读位图数据进内存
	unsigned char *pBmpBuf;//读入图像数据的指针
	pBmpBuf = new unsigned char[lineByte * bmpHeight];//将读取的数据保存到内存里面，使用指针 pBmpBuf指向 
	fread(pBmpBuf,1,lineByte * bmpHeight,fp);//搜寻的方式是 pBmpBuf指向 fp内，长度是图片的长度，一次一个字节的方式进行寻找 
	
	int k = 0;//定义一个长度，每读取一个字节数组长度就 +1 
	for(int i = 50-M; i < 50+M; i++){
		for(int j = 40-M; j < 40+M; j++){
			array[k++] = (int)pBmpBuf[(i-1)*80+j] / 255.0;//读取的数据转换成 int型存入数组 array中，并且转换成灰度值百分比 
		}
	}
	fclose(fp);//关闭文件
}

/*
	Name:Training() 
	Date: 06/06/20 16:06
	Description:训练神经网络数组，带有两个参数一个读取的图片的灰度值，一个双精度的数组指针，期望的输出值
				返回 bool型，如果达到精度则返回 True否则返回 false
				首先设定精度，打印出来的数字都是4位小数
				定义双精度数字 h和 y分别用来保存输入层到隐藏的输出值和隐藏层到输出层的结果
				计算输入与输出的误差项以及和隐藏层之间的误差项
				计算权值 V和 W的变化零，计算隐藏层和输出层的误差项
				对上面四者进行调整 
*/
int Training(double *X, double *D){
		cout<<setiosflags(ios::fixed)<<setprecision(4);//规范小数打印的位数，小数点后4位 
		double h[L];//双精度 h保存输入层到隐藏层的结果 
		memset(h,0,sizeof(h));
		for(int j = 0; j < L; j++){
			double delte = 0.0;//按照公式，用来保存 Xi*Vij的累积和 
			for(int i = 0; i < N; i++){
				delte += X[i]*V[i][j];
			}
			h[j] = F(delte+phi[j]);//保存数值 
		}
		double y[M];//双精度 h保存隐藏层到输出层的结果 
		memset(y,0,sizeof(y));
		for(int k = 1; k < M; k++){
			double delte = 0.0;//按照公式，用来保存 hj*Wij的累积和
			for(int j = 0; j < L; j++){
				delte += h[j]*W[j][k];
			}
			y[k] = F(delte+theta[k]);//保存数值 
		}
		
		double e = 0.0;//定义保存总误差的小数 
		for(int k = 1; k < M; k++){
			e += (D[k] - y[k])*(D[k] - y[k]);//计算总误差 
		}
		if(e*0.5 < epsilon){//如果满足1/2*(e)满足精度则返回True 
			return 1;
		}
		
		double delteK[M];//双精度用来保存输入和输出项的误差 
		memset(delteK,0,sizeof(delteK));
		for(int k = 1; k < M; k++){
			delteK[k] = (D[k]-y[k])*y[k]*(1-y[k]);//误差项 
		}

		double delteJ[L];//双精度用来保存输入和隐藏层的误差 
		memset(delteJ,0,sizeof(delteJ));
		for(int j = 0; j < L; j++){
			double beta = 0.0;//定义误差总和项 
			for(int k = 1; k < M; k++){
				beta += delteK[k]*W[j][k];
			}
			delteJ[j] = h[j]*(1-h[j])*beta;//按照公式计算误差 
		}
		
		for(int j = 0; j < L; j++){
			deltephi[j] = alpha*delteJ[j];//计算阈值的调整量
//			deltephi[j] = (alpha/(1.0+L))*(deltephi[j]+1)*delteJ[j]; ??误差有点大?? 
			phi[j] += deltephi[j];//调整隐含层的阈值 
		}
		
		for(int i = 0; i < N; i++){
			for(int j = 0; j < L; j++){
				delteV[i][j] = alpha*delteJ[j]*X[i];//计算权值的调整量 
//				delteV[i][j] = (alpha/(1.0+N))*(delteV[i][j]+1)*delteJ[j]*X[i];
				V[i][j] += delteV[i][j];//调整 V权值
			}
		}
		
		for(int j = 0; j < L; j++){
			for(int k = 1; k < M; k++){
				delteW[j][k] = alpha*delteK[k]*h[j];//计算权值的调整量 
//				delteW[j][k] = (alpha/(1.0+L))*(delteW[j][k]+1)*delteK[k]*h[j];
				W[j][k] += delteW[j][k];//调整 W权值 
			}
		}
		for(int k = 1; k < M; k++){
			deltetheta[k] = alpha*delteK[k];//计算阈值的调整量 
//			deltetheta[k] = (alpha/(1.0+L))*(deltetheta[k]+1)*delteK[k];
			theta[k] += deltetheta[k];//调整输出层的阈值 
		}
	return 0;
}

/*
	Name:Match()
	Date: 06/06/20 16:20
	Description:返回志愿者编号，一个参数接收需要进行匹配的图片的灰度值
				判断最终匹配的是哪一号志愿者，同样先计算输出
				然后分别于每一位支援者的图片的期望值进行对比，找到误差值最小
				当误差值最小时，返回该志愿者编号 
*/
int Match(double *result, double *X){
		int index = 0;//最终的输出的结果，志愿者编号 
		double h[L]; //双精度 h保存输入层到隐藏层的结果 
		memset(h,0,sizeof(h));
		for(int j = 0; j<L; j++){
			double delte = 0.0;//按照公式，用来保存 Xi*Vij的累积和 
			for(int i = 0; i<N; i++){
				delte += X[i]*V[i][j];
			}
			h[j] = F(delte-phi[j]);//保存数值 
		}
		double y[M];//双精度 h保存隐藏层到输出层的结果 
		memset(y,0,sizeof(y));
		for(int k = 1; k < M; k++){
			double delte = 0.0;//按照公式，用来保存 hj*Wij的累积和
			for(int j = 0; j < L; j++){
				delte += h[j]*W[j][k];
			}
			y[k] = F(delte-theta[k]);//保存数值 
		}
		
		double min = 9999999.0;//定义一个很大的误差项，用于最后的对比 
		for(int n=1; n<M; n++){//循环遍历15个志愿者 
			double e = 0.0;//定义与当前志愿者之间的误差项 
			double D[M];//期望的输出结果 
			memset(D,0,sizeof(D));
			D[n] = 1.0;///当前是第几位志愿者，期待与之匹配，即最期望的输出 
			for(int k = 1; k < M; k++){
				e += abs((D[k] - y[k])*(D[k] - y[k]));//计算当前的误差值 
			}
			result[n] = 1.0/exp(e*0.5);//用于保存计算的误差公式，结果越小误差越小匹配度越高 
			if(e*0.5 < min){
				min = e*0.5;//记录最小的误差 
				index = n;//记录志愿者编号 
			}
		}
		return index;//返回志愿者编号 
	}
