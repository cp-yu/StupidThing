#include <iostream>
#include <tuple>
using namespace std;
struct Info2Com
{
    float ref[2];
    float *Fb[2];
};

class Source
{
private:
public:
    float temprature;
    float flow;

public:
    Source(float = 1, float = 1);
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

Source::Source(float t, float f)
{
    temprature = t;
    flow = f;
}

Source::~Source()
{
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
public:
    float percentHot, percentFlow, temprature, flow;

    static int inputNum;
    float **inputTemprature = new float *[inputNum];
    float **inputFlow = new float *[inputNum];
    float *inputT = new float[inputNum];
    float *inputF = new float[inputNum];
    float *PH, *PF;
    int test=inputNum;
    bool flag = false;

public:
    Throttler(float = 0, float = 0);
    ~Throttler();
    tuple<float *, float *> output();
    void pri();
    void input(float *inputTemp[], float *inputflow[], short int totalSource);

    void inputTotalSource(short int totalSource);
    void inputInputTemp(float *inputTemp[]);
    void inputInputflow(float *inputflow[]);
    void process();
    void set(tuple<float *, float *> inputControl);
    tuple<float, float> outputValue();
};

Throttler::Throttler(float percentHotin , float percentFlowin )
{
    percentHot = percentHotin;
    percentFlow = percentFlowin;
    // return;
}

Throttler::~Throttler()
{
}
tuple<float *, float *> Throttler::output()
{
    process();
    cout << "output temprature is " << temprature << ",    ";
    cout << "output flow is " << flow << endl;
    return make_tuple(&temprature, &flow);
}
void Throttler::pri()
{

    cout << "output temprature is " << temprature << ",    ";
    cout << "output flow is " << flow << endl;
}
void Throttler::input(float *inputTemp[], float *inputflow[], short int totalSource)
{
    // inputNum = totalSource;
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
}
void Throttler::inputInputTemp(float *inputTemp[])
{
    for (size_t i = 0; i < inputNum; i++)
    {
        inputTemprature[i] = inputTemp[i];
    }
}
void Throttler::inputInputflow(float *inputflow[])
{
    for (size_t i = 0; i < inputNum; i++)
    {
        inputFlow[i] = *(inputflow + i);
    }
}
void Throttler::process()
{
    float f0, f1;

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
public:
    Info2Com *inputSruct;
    float outputTemprature, outputFlow;
    float PIDHot[4] = {0}, PIDFlow[4] = {0, 1, 0, 0};
    float percentHot = 0;
    float percentFlow = 1;

public:
    void input(Info2Com *in);
    tuple<float, float> output();
    void process();

    tuple<float *, float *> controlFoo();
    tuple<float *, float *> controlPID();
};
void Comparer::input(Info2Com *in)
{

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

    return make_tuple(outputTemprature, outputFlow);
}
tuple<float *, float *> Comparer::controlFoo()
{

    percentHot += outputTemprature / 100;
    percentFlow += outputFlow / 200;

    return make_tuple(&percentHot, &percentFlow);
}
tuple<float *, float *> Comparer::controlPID()
{

    PIDHot[1] += outputTemprature / 200;

    percentHot = PIDHot[0] + PIDHot[1] + PIDHot[2];

    PIDFlow[1] += outputFlow / 800;

    percentFlow = PIDFlow[0] + PIDFlow[1] + PIDFlow[2];
    return make_tuple(&percentHot, &percentFlow);
}
int Throttler::inputNum=2;
int main()
{
    Source *s = new Source[2];
    int count = 100;
    // inputNum=2;
    // cout<<"inputNum now is "<<Throttler::inputNum<<endl;
    Throttler Ter(float(0), float(1));
    Feedback feed;
    Comparer com;
    float *tt[2];
    float *ff[2];
    float *to, *fo;
    Info2Com info;

    s[0].set(80, 100);
    s[1].set(10, 100);

    info.ref[0] = 38.8;
    info.ref[1] = 20;

    for (int i = 0; i < 2; i++)
    {
        tt[i] = s[i].getTemp();
        ff[i] = s[i].getFlow();
    }

    Ter.input(tt, ff, 2);
    tie(to, fo) = Ter.output();
    info.Fb[0] = to;
    info.Fb[1] = fo;
    com.input(&info);
    for (int zxcv = 0; zxcv < count; zxcv++)
    {

        Ter.process();
        com.process();
        Ter.set(com.controlPID());

        Ter.pri();

        if (zxcv == 50)
        {
            s[0].set(90, 150);
            cout << "disturb" << endl;
        }
        if (zxcv == 75)
        {
            info.ref[0] = 50;
            info.ref[1] = 10;
            cout << "target change" << endl;
        }
    }
    return 0;
}