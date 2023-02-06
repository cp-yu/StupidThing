#include <iostream>
#include <tuple>
#include "package\matplotlibcpp.h"
#include <vector>
namespace plt = matplotlibcpp;
using namespace std;
struct Info2Com
{
    float ref[2];
    float *Fb[2];
};

class Source
{
private:
    /* data */
public:
    float temprature;
    float flow;

public:
    Source(float t, float f);
    ~Source();
    tuple<float *, float *> get();
    float *getFlow();
    float *getTemp();
    void set(float t, float f)
    {
        temprature = t;
        flow = f;
    }
};

Source::Source(float t = 1, float f = 1) : temprature(t), flow(f)
{
    // cout << "Source created!" << endl;
}

Source::~Source()
{
    // cout << "Source deleted!" << endl;
}

tuple<float *, float *> Source::get()
{
    return make_tuple(&temprature, &flow);
}
float *Source::getTemp()
{
    return &temprature;
}
float *Source::getFlow()
{
    return &flow;
}

class Throttler
{
private:
    /* data */
public:
    float percentHot, percentFlow, temprature, flow;
    // float *inputTemprature, *inputFlow;
    int inputNum;
    float **inputTemprature = new float *[inputNum];
    float **inputFlow = new float *[inputNum];
    float *inputT = new float[inputNum];
    float *inputF = new float[inputNum];
    float *PH, *PF;
    bool flag = false;

public:
    Throttler(float percentHot, float percentFlow);
    ~Throttler();
    tuple<float *, float *> output();
    void pri();
    void input(float *inputTemp[], float *inputflow[], short int totalSource);
    // void input(float inputTemp[], float inputflow[],short int totalSource);
    void inputTotalSource(short int totalSource);
    void inputInputTemp(float *inputTemp[]);
    void inputInputflow(float *inputflow[]);
    void process();
    void set(tuple<float *, float *> inputControl);
    tuple<float , float > outputValue();
    // void set(float PH, float PF);
};

Throttler::Throttler(float percentHot = 0, float percentFlow = 0) : percentHot(percentHot), percentFlow(percentFlow)
{
    // cout << "Throttler created!" << endl;
}

Throttler::~Throttler()
{
    // cout << "Throttler deleted!" << endl;
}
tuple<float *, float *> Throttler::output()
{
    // process();
    cout << "output temprature is " << temprature << ",    ";
    cout << "output flow is " << flow << endl;
    return make_tuple(&temprature, &flow);
}
tuple<float , float > Throttler::outputValue()
{
    // process();
    cout << "output temprature is " << temprature << ",    ";
    cout << "output flow is " << flow << endl;
    return make_tuple(temprature,flow);
}
void Throttler::pri()
{
    // process();
    cout << "output temprature is " << temprature << ",    ";
    cout << "output flow is " << flow << endl;
    // return make_tuple(&temprature, &flow);
}
void Throttler::input(float *inputTemp[], float *inputflow[], short int totalSource)
{
    inputNum = totalSource;
    for (size_t i = 0; i < inputNum; i++)
    {
        inputTemprature[i] = inputTemp[i];
        inputFlow[i] = inputflow[i];
    }
    process();
}
void Throttler::inputTotalSource(short int totalSource)
{
    inputNum = totalSource;
    // process();
}
void Throttler::inputInputTemp(float *inputTemp[])
{
    for (size_t i = 0; i < inputNum; i++)
    {
        inputTemprature[i] = inputTemp[i];
    }

    // process();
}
void Throttler::inputInputflow(float *inputflow[])
{
    for (size_t i = 0; i < inputNum; i++)
    {
        inputFlow[i] = *(inputflow + i);
    }
    // process();
}
void Throttler::process()
{
    float f0, f1;
    // float inputF[inputNum],inputT[inputNum];
    for (size_t i = 0; i < inputNum; i++)
    {
        inputF[i] = *inputFlow[i];
        inputT[i] = *inputTemprature[i];
    }
    if (flag)
    {
        percentHot = *PH;
        percentFlow = *PF;
    }
    percentHot = percentHot < 0 ? 0 : (percentHot > 1 ? 1 : percentHot);
    percentFlow = percentFlow < 0 ? 0 : (percentFlow > 1 ? 1 : percentFlow);
    // flow = (inputFlow[0] + inputFlow[1])*percentFlow;
    f0 = inputF[0] * percentFlow * percentHot;
    f1 = inputF[1] * percentFlow * (1 - percentHot);
    flow = f0 + f1;
    temprature = (f0 * inputT[0] + f1 * inputT[1]) / flow;
}
void Throttler::set(tuple<float *, float *> inputControl)
{
    flag = true;
    tie(PH, PF) = inputControl;
    percentHot = *PH;
    percentFlow = *PF;
    process();
}
class Feedback
{
private:
    /* data */
public:
    float inputTemprature, inputFlow;
    float outputTemprature, outputFlow;

public:
    void process();
    void input(float temp, float flow);
    tuple<float, float> output();
};
void Feedback::process()
{
    outputTemprature = inputTemprature;
    outputFlow = inputFlow;
}
void Feedback::input(float temp, float flow)
{
    inputTemprature = temp;
    inputFlow = flow;
}
tuple<float, float> Feedback::output()
{
    return make_tuple(outputTemprature, outputFlow);
}
class Comparer
{
private:
    /* data */
public:
    Info2Com *inputSruct;
    float outputTemprature, outputFlow;
    float PIDHot[4]={0},PIDFlow[4]={0,1,0,0};
    float percentHot = 0;
    float percentFlow = 1;

public:
    void input(Info2Com *in);
    tuple<float, float> output();
    void process();
    // float controlFoo();
    tuple<float *, float *> controlFoo();
    tuple<float *, float *> controlPID();
};
void Comparer::input(Info2Com *in)
{
    // for (int i = 0; i < size; i++)
    // {
    //     // inputSruct->ref=
    //     // cout<<"inputFlow["<<i<<"]"<<" is "<<inputFlow[i]<<endl;
    //     // cout<<"inputTemprature["<<i<<"]"<<" is "<<inputTemprature[i]<<endl;
    // }
    inputSruct = in;
    process();
}
void Comparer::process()
{
    outputTemprature = inputSruct->ref[0] - *inputSruct->Fb[0];
    outputFlow = inputSruct->ref[1] - *inputSruct->Fb[1];
}
tuple<float, float> Comparer::output()
{
    // cout << "rerfer temprature diff is " << outputTemprature << ",    ";
    // cout << "rerfer flow diff is " << outputFlow << endl;
    return make_tuple(outputTemprature, outputFlow);
}
tuple<float *, float *> Comparer::controlFoo()
{
    // if(outputTemprature>0)
    // {
    //     percentHot+=0.01;
    // }
    // else{
    //     percentHot-=0.005;
    // }
    percentHot += outputTemprature / 100;
    percentFlow += outputFlow / 200;
    // cout << "percent Hot is " << percentHot << endl;
    return make_tuple(&percentHot, &percentFlow);
}
tuple<float *, float *> Comparer::controlPID()
{
    // temprature
    // PIDHot[0]=outputTemprature / 100;
    PIDHot[1]+=outputTemprature / 200;
    // PIDHot[2]=(outputTemprature-PIDHot[3])/500;
    // PIDHot[3]=outputTemprature;
    percentHot =PIDHot[0]+PIDHot[1]+PIDHot[2] ;
    // flow
    // PIDFlow[0]=outputFlow / 400;
    PIDFlow[1]+=outputFlow / 800;
    // PIDFlow[2]=(outputFlow-PIDFlow[3])/5000;
    // PIDFlow[3]=outputFlow;
    percentFlow =PIDFlow[0]+PIDFlow[1]+PIDFlow[2];
    return make_tuple(&percentHot, &percentFlow);
}
int main()
{
    Source *s = new Source[2];
    Throttler Ter(0, 1);
    Feedback feed;
    Comparer com;
    float *tt[2];
    float *ff[2];
    float *to, *fo;
    float t=0,f=0;

    int count=100;
    float *Temp=new float [count];
    float *Flow=new float [count];
    vector<float> Tempv;
    vector<float> Flowv;
    // tuple<float, float> tup;
    // float tempArray[2], flowArray[2];
    Info2Com info;
    // Parameters set   until pass by pointer
    s[0].set(80, 100);
    s[1].set(10, 100);
    // target 38.8℃ 20
    // tempArray[0] = 38.8;
    // flowArray[0] = 20;
    info.ref[0] = 38.8;
    info.ref[1] = 20;
    // for (int i = 0; i < 2; i++)
    // {
    //     tie(tt, ff) = s[i].get();
    //     Ter.input(i, tt, ff);
    // }
    for (int i = 0; i < 2; i++)
    {
        tt[i] = s[i].getTemp();
        ff[i] = s[i].getFlow();
    }
    // Ter.input(*tt,*ff,2);
    // Ter.inputTotalSource(2);
    // Ter.inputInputflow(ff);
    // Ter.inputInputTemp(tt);
    // Ter.process();
    Ter.input(tt, ff, 2);
    tie(to, fo) = Ter.output();
    info.Fb[0] = to;
    info.Fb[1] = fo;
    com.input(&info);
    for (int zxcv = 0; zxcv <count; zxcv++)
    {

        Ter.process();
        com.process();
        Ter.set(com.controlPID());
        // com.output();
        tie(t,f)=Ter.outputValue();
        Temp[zxcv]=t;
        Flow[zxcv]=f;
        // Temp.push_back(t);
        // Flow.push_back(f);

        // disturb and target change
        if (zxcv == 50)
        {
            s[0].set(90, 150);
            cout << "disturb" << endl;
            // tempArray[0] = 50;
            // flowArray[0] = 20;
        }
        if (zxcv == 75)
        {
            info.ref[0] = 50;
            info.ref[1] = 10;
            cout << "target change" << endl;
        }
    }
    // float T[60];
    for (int i = 0; i < count; i++)
    {
        Tempv.push_back(Temp[i]);
        Flowv.push_back(Flow[i]);
    }
    
    plt::plot(Tempv,Flowv);
    // plt::plot();
    plt::show();
    return 0;
}