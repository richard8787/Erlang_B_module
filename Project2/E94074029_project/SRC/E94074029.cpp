#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <queue>
#include <random>
#include <vector>
using namespace std;
struct car
{
    double pos_x;
    double pos_y;
    int face; //1:x+, 2:y-, 3:x-, 4:y+
    int born_time;
    int station = 0;
    int change = 0;
};

double distance(int choose, car thecar);
void turn(car &thecar, int choose); //1:left, 2:straight, 3:right
void policy_best(car &thecar);
void policy_threshold(car &thecar, double thres);
void policy_entropy(car &thecar, double entor);
void policy_my(car &thecar);
double get_Power(car &thecar, int choose);
void showcar(car thecar)
{
    cout << "thecar.pos_x: " << thecar.pos_x << " "
         << "thecar.pos_y: " << thecar.pos_y << " "
         << "thecar.face: " << thecar.face << " "
         << "thecar.born_time: " << thecar.born_time << " "
         << "thecar.station: " << thecar.station << " "
         << "thecar.change: " << thecar.change << endl;
}

const int station_Ax = 330;
const int station_Ay = 350;
const int station_Bx = 640;
const int station_By = 310;
const int station_Cx = 360;
const int station_Cy = 680;
const int station_Dx = 660;
const int station_Dy = 658;
const double T = 15;
const double E = 13;
double lunda;

int main()
{
    double get_time;
    double sum_time = 0;
    lunda = 0.5;
    int therand;
    car temp;
    int count_car = 0;
    double P_counter = 0;
    int N_counter = 0;
    random_device rd;
    mt19937 rng(rd());
    srand(time(NULL));
    vector<car> carD;
    vector<int> total_handoff(86401, 0);

    //the best policy**********************************************************************
    for (int l = 0; l < 3; l++)
    {
        if (l == 0)
            lunda = 0.5;
        if (l == 1)
            lunda = 1.0 / 3.0;
        if (l == 2)
            lunda = 0.2;
        exponential_distribution<double> car_time_g(lunda);
        total_handoff.clear();
        total_handoff.resize(86401);

        //situation for loop
        for (int situation = 0; situation < 36; situation++)
        {
            if (situation == 0)
                cout << "situation : ";
            cout << situation << " ";

            //clear
            carD.clear();
            count_car = 0;
            sum_time = 0;
            //clear end

            //generate born time
            while (sum_time <= 86400)
            {
                get_time = car_time_g.operator()(rng);
                sum_time += get_time;
                temp.born_time = sum_time;
                carD.push_back(temp);
                count_car++;
            }
            //generate born time end

            //initialize car site
            const int hu = 100;
            if (situation < 9)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 100 + hu * (situation % 9);
                    carD[i].pos_y = 0;
                    carD[i].face = 4;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 18)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 0;
                    carD[i].pos_y = 100 + hu * (situation % 9);
                    carD[i].face = 1;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 27)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 100 + hu * (situation % 9);
                    carD[i].pos_y = 1000;
                    carD[i].face = 2;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 36)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 1000;
                    carD[i].pos_y = 100 + hu * (situation % 9);
                    carD[i].face = 3;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            //initialize car site end

            //count the handoff
            int count_handoff = 0;

            for (int ci = 0; ci < count_car; ci++)
            {
                for (int i = 0; i <= 86400; i++)
                {
                    //to decide turn or not
                    if (i != 0 && i % 10 == 0)
                    {
                        therand = rand() % 5;
                        if (therand == 1)
                            turn(carD[ci], 1);
                        if (therand == 2)
                            turn(carD[ci], 3);
                    }

                    //move
                    if (carD[ci].face == 1)
                        carD[ci].pos_x += 10;
                    if (carD[ci].face == 2)
                        carD[ci].pos_y -= 10;
                    if (carD[ci].face == 3)
                        carD[ci].pos_x -= 10;
                    if (carD[ci].face == 4)
                        carD[ci].pos_y += 10;

                    //choose the policy
                    policy_best(carD[ci]);

                    //count the handoff
                    if (carD[ci].change == 1)
                    {
                        count_handoff++;
                        total_handoff[carD[ci].born_time]++;
                    }

                    //born time++
                    carD[ci].born_time++;

                    //to count N and P
                    N_counter++;
                    P_counter += get_Power(carD[ci], carD[ci].station);

                    //to check leave or not
                    if (carD[ci].born_time > 86400)
                        break;
                    if (carD[ci].pos_x <= 0 || carD[ci].pos_x >= 1000)
                        break;
                    if (carD[ci].pos_y <= 0 || carD[ci].pos_y >= 1000)
                        break;
                }
            }
            //cout << "count_handoff: " << count_handoff << endl
            //     << endl;
        }
        //per lunda end , start output the data
        cout << endl
             << endl;
        cout << "*****************************************" << endl;
        if (l == 0)
            cout << "Best Policy, lunda = 1/2" << endl;
        if (l == 1)
            cout << "Best Policy, lunda = 1/3" << endl;
        if (l == 2)
            cout << "Best Policy, lunda = 1/5" << endl;
        int total_handoff_c = 0;
        for (int i = 0; i <= 86400; i++)
            total_handoff_c += total_handoff[i];
        cout << "total_handoff :" << total_handoff_c << endl;
        //cout << "N_counter :" << N_counter << endl;
        //cout << "P_counter :" << P_counter << endl;
        cout << "AVG_Power :" << P_counter / double(N_counter) << endl;
        N_counter = 0;
        P_counter = 0;
        cout << "*****************************************" << endl;
        cout << endl;
        //output the data end

        //start to write the file
        if (l == 0)
        {
            ofstream output_file0("Best_1divide2.csv");
            for (int i = 0; i <= 86400; i++)
                output_file0 << total_handoff[i] << endl;
            output_file0.close();
        }
        if (l == 1)
        {
            ofstream output_file1("Best_1divide3.csv");
            for (int i = 0; i <= 86400; i++)
                output_file1 << total_handoff[i] << endl;
            output_file1.close();
        }
        if (l == 2)
        {
            ofstream output_file2("Best_1divide5.csv");
            for (int i = 0; i <= 86400; i++)
                output_file2 << total_handoff[i] << endl;
            output_file2.close();
        }
        //write the file end
    }
    //best policy end**********************************************************************

    //threshold**********************************************************************
    for (int l = 0; l < 3; l++)
    {
        if (l == 0)
            lunda = 0.5;
        if (l == 1)
            lunda = 1.0 / 3.0;
        if (l == 2)
            lunda = 0.2;
        exponential_distribution<double> car_time_g(lunda);
        total_handoff.clear();
        total_handoff.resize(86401);

        //situation for loop
        for (int situation = 0; situation < 36; situation++)
        {
            if (situation == 0)
                cout << "situation : ";
            cout << situation << " ";

            //clear
            carD.clear();
            count_car = 0;
            sum_time = 0;
            //clear end

            //generate born time
            while (sum_time <= 86400)
            {
                get_time = car_time_g.operator()(rng);
                sum_time += get_time;
                temp.born_time = sum_time;
                carD.push_back(temp);
                count_car++;
            }
            //generate born time end

            //initialize car site
            const int hu = 100;
            if (situation < 9)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 100 + hu * (situation % 9);
                    carD[i].pos_y = 0;
                    carD[i].face = 4;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 18)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 0;
                    carD[i].pos_y = 100 + hu * (situation % 9);
                    carD[i].face = 1;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 27)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 100 + hu * (situation % 9);
                    carD[i].pos_y = 1000;
                    carD[i].face = 2;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 36)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 1000;
                    carD[i].pos_y = 100 + hu * (situation % 9);
                    carD[i].face = 3;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            //initialize car site end

            //count the handoff
            int count_handoff = 0;

            for (int ci = 0; ci < count_car; ci++)
            {
                for (int i = 0; i <= 86400; i++)
                {
                    //to decide turn or not
                    if (i != 0 && i % 10 == 0)
                    {
                        therand = rand() % 5;
                        if (therand == 1)
                            turn(carD[ci], 1);
                        if (therand == 2)
                            turn(carD[ci], 3);
                    }

                    //move
                    if (carD[ci].face == 1)
                        carD[ci].pos_x += 10;
                    if (carD[ci].face == 2)
                        carD[ci].pos_y -= 10;
                    if (carD[ci].face == 3)
                        carD[ci].pos_x -= 10;
                    if (carD[ci].face == 4)
                        carD[ci].pos_y += 10;

                    //choose the policy
                    policy_threshold(carD[ci], T);

                    //count the handoff
                    if (carD[ci].change == 1)
                    {
                        count_handoff++;
                        total_handoff[carD[ci].born_time]++;
                    }

                    //born time++
                    carD[ci].born_time++;

                    //to count N and P
                    N_counter++;
                    P_counter += get_Power(carD[ci], carD[ci].station);

                    //to check leave or not
                    if (carD[ci].born_time > 86400)
                        break;
                    if (carD[ci].pos_x <= 0 || carD[ci].pos_x >= 1000)
                        break;
                    if (carD[ci].pos_y <= 0 || carD[ci].pos_y >= 1000)
                        break;
                }
            }
            //cout << "count_handoff: " << count_handoff << endl
            //    << endl;
        }
        //per lunda end , start output the data
        cout << endl
             << endl;
        cout << "*****************************************" << endl;
        if (l == 0)
            cout << "Threshold Policy, lunda = 1/2" << endl;
        if (l == 1)
            cout << "Threshold Policy, lunda = 1/3" << endl;
        if (l == 2)
            cout << "Threshold Policy, lunda = 1/5" << endl;
        int total_handoff_c = 0;
        for (int i = 0; i <= 86400; i++)
            total_handoff_c += total_handoff[i];
        cout << "total_handoff :" << total_handoff_c << endl;
        //cout << "N_counter :" << N_counter << endl;
        //cout << "P_counter :" << P_counter << endl;
        cout << "AVG_Power :" << P_counter / double(N_counter) << endl;
        N_counter = 0;
        P_counter = 0;
        cout << "*****************************************" << endl;
        cout << endl;
        //output the data end

        //start to write the file
        if (l == 0)
        {
            ofstream output_file0("Threshold_1divide2.csv");
            for (int i = 0; i <= 86400; i++)
                output_file0 << total_handoff[i] << endl;
            output_file0.close();
        }
        if (l == 1)
        {
            ofstream output_file1("Threshold_1divide3.csv");
            for (int i = 0; i <= 86400; i++)
                output_file1 << total_handoff[i] << endl;
            output_file1.close();
        }
        if (l == 2)
        {
            ofstream output_file2("Threshold_1divide5.csv");
            for (int i = 0; i <= 86400; i++)
                output_file2 << total_handoff[i] << endl;
            output_file2.close();
        }
        //write the file end
    }
    //threshold end**********************************************************************

    //entropy**********************************************************************
    for (int l = 0; l < 3; l++)
    {
        if (l == 0)
            lunda = 0.5;
        if (l == 1)
            lunda = 1.0 / 3.0;
        if (l == 2)
            lunda = 0.2;
        exponential_distribution<double> car_time_g(lunda);
        total_handoff.clear();
        total_handoff.resize(86401);

        //situation for loop
        for (int situation = 0; situation < 36; situation++)
        {
            if (situation == 0)
                cout << "situation : ";
            cout << situation << " ";

            //clear
            carD.clear();
            count_car = 0;
            sum_time = 0;
            //clear end

            //generate born time
            while (sum_time <= 86400)
            {
                get_time = car_time_g.operator()(rng);
                sum_time += get_time;
                temp.born_time = sum_time;
                carD.push_back(temp);
                count_car++;
            }
            //generate born time end

            //initialize car site
            const int hu = 100;
            if (situation < 9)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 100 + hu * (situation % 9);
                    carD[i].pos_y = 0;
                    carD[i].face = 4;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //  cout << "count_car: " << count_car << endl;
            }
            else if (situation < 18)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 0;
                    carD[i].pos_y = 100 + hu * (situation % 9);
                    carD[i].face = 1;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                // cout << "count_car: " << count_car << endl;
            }
            else if (situation < 27)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 100 + hu * (situation % 9);
                    carD[i].pos_y = 1000;
                    carD[i].face = 2;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 36)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 1000;
                    carD[i].pos_y = 100 + hu * (situation % 9);
                    carD[i].face = 3;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            //initialize car site end

            //count the handoff
            int count_handoff = 0;

            for (int ci = 0; ci < count_car; ci++)
            {
                for (int i = 0; i <= 86400; i++)
                {
                    //to decide turn or not
                    if (i != 0 && i % 10 == 0)
                    {
                        therand = rand() % 5;
                        if (therand == 1)
                            turn(carD[ci], 1);
                        if (therand == 2)
                            turn(carD[ci], 3);
                    }

                    //move
                    if (carD[ci].face == 1)
                        carD[ci].pos_x += 10;
                    if (carD[ci].face == 2)
                        carD[ci].pos_y -= 10;
                    if (carD[ci].face == 3)
                        carD[ci].pos_x -= 10;
                    if (carD[ci].face == 4)
                        carD[ci].pos_y += 10;

                    //choose the policy
                    policy_entropy(carD[ci], E);

                    //count the handoff
                    if (carD[ci].change == 1)
                    {
                        count_handoff++;
                        total_handoff[carD[ci].born_time]++;
                    }

                    //born time++
                    carD[ci].born_time++;

                    //to count N and P
                    N_counter++;
                    P_counter += get_Power(carD[ci], carD[ci].station);

                    //to check leave or not
                    if (carD[ci].born_time > 86400)
                        break;
                    if (carD[ci].pos_x <= 0 || carD[ci].pos_x >= 1000)
                        break;
                    if (carD[ci].pos_y <= 0 || carD[ci].pos_y >= 1000)
                        break;
                }
            }
            //cout << "count_handoff: " << count_handoff << endl
            //    << endl;
        }
        //per lunda end , start output the data
        cout << endl
             << endl;
        cout << "*****************************************" << endl;
        if (l == 0)
            cout << "Entropy Policy, lunda = 1/2" << endl;
        if (l == 1)
            cout << "Entropy Policy, lunda = 1/3" << endl;
        if (l == 2)
            cout << "Entropy Policy, lunda = 1/5" << endl;
        int total_handoff_c = 0;
        for (int i = 0; i <= 86400; i++)
            total_handoff_c += total_handoff[i];
        cout << "total_handoff :" << total_handoff_c << endl;
        //cout << "N_counter :" << N_counter << endl;
        //cout << "P_counter :" << P_counter << endl;
        cout << "AVG_Power :" << P_counter / double(N_counter) << endl;
        N_counter = 0;
        P_counter = 0;
        cout << "*****************************************" << endl;
        cout << endl;
        //output the data end

        //start to write the file
        if (l == 0)
        {
            ofstream output_file0("Entropy_1divide2.csv");
            for (int i = 0; i <= 86400; i++)
                output_file0 << total_handoff[i] << endl;
            output_file0.close();
        }
        if (l == 1)
        {
            ofstream output_file1("Entropy_1divide3.csv");
            for (int i = 0; i <= 86400; i++)
                output_file1 << total_handoff[i] << endl;
            output_file1.close();
        }
        if (l == 2)
        {
            ofstream output_file2("Entropy_1divide5.csv");
            for (int i = 0; i <= 86400; i++)
                output_file2 << total_handoff[i] << endl;
            output_file2.close();
        }
        //write the file end
    }
    //entropy end**********************************************************************

    //my**********************************************************************
    for (int l = 0; l < 3; l++)
    {
        if (l == 0)
            lunda = 0.5;
        if (l == 1)
            lunda = 1.0 / 3.0;
        if (l == 2)
            lunda = 0.2;
        exponential_distribution<double> car_time_g(lunda);
        total_handoff.clear();
        total_handoff.resize(86401);

        //situation for loop
        for (int situation = 0; situation < 36; situation++)
        {
            if (situation == 0)
                cout << "situation : ";
            cout << situation << " ";

            //clear
            carD.clear();
            count_car = 0;
            sum_time = 0;
            //clear end

            //generate born time
            while (sum_time <= 86400)
            {
                get_time = car_time_g.operator()(rng);
                sum_time += get_time;
                temp.born_time = sum_time;
                carD.push_back(temp);
                count_car++;
            }
            //generate born time end

            //initialize car site
            const int hu = 100;
            if (situation < 9)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 100 + hu * (situation % 9);
                    carD[i].pos_y = 0;
                    carD[i].face = 4;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 18)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 0;
                    carD[i].pos_y = 100 + hu * (situation % 9);
                    carD[i].face = 1;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 27)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 100 + hu * (situation % 9);
                    carD[i].pos_y = 1000;
                    carD[i].face = 2;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            else if (situation < 36)
            {
                for (int i = 0; i < count_car; i++)
                {
                    carD[i].pos_x = 1000;
                    carD[i].pos_y = 100 + hu * (situation % 9);
                    carD[i].face = 3;
                    policy_best(carD[i]);
                    carD[i].change = 0;
                }
                //cout << "count_car: " << count_car << endl;
            }
            //initialize car site end

            //count the handoff
            int count_handoff = 0;

            for (int ci = 0; ci < count_car; ci++)
            {
                for (int i = 0; i <= 86400; i++)
                {
                    //to decide turn or not
                    if (i != 0 && i % 10 == 0)
                    {
                        therand = rand() % 5;
                        if (therand == 1)
                            turn(carD[ci], 1);
                        if (therand == 2)
                            turn(carD[ci], 3);
                    }

                    //move
                    if (carD[ci].face == 1)
                        carD[ci].pos_x += 10;
                    if (carD[ci].face == 2)
                        carD[ci].pos_y -= 10;
                    if (carD[ci].face == 3)
                        carD[ci].pos_x -= 10;
                    if (carD[ci].face == 4)
                        carD[ci].pos_y += 10;

                    //choose the policy
                    policy_my(carD[ci]);

                    //count the handoff
                    if (carD[ci].change == 1)
                    {
                        count_handoff++;
                        total_handoff[carD[ci].born_time]++;
                    }

                    //born time++
                    carD[ci].born_time++;

                    //to count N and P
                    N_counter++;
                    P_counter += get_Power(carD[ci], carD[ci].station);

                    //to check leave or not
                    if (carD[ci].born_time > 86400)
                        break;
                    if (carD[ci].pos_x <= 0 || carD[ci].pos_x >= 1000)
                        break;
                    if (carD[ci].pos_y <= 0 || carD[ci].pos_y >= 1000)
                        break;
                }
            }
            //cout << "count_handoff: " << count_handoff << endl
            //<< endl;
        }
        //per lunda end , start output the data
        cout << endl
             << endl;
        cout << "*****************************************" << endl;
        if (l == 0)
            cout << "My Policy, lunda = 1/2" << endl;
        if (l == 1)
            cout << "My Policy, lunda = 1/3" << endl;
        if (l == 2)
            cout << "My Policy, lunda = 1/5" << endl;
        int total_handoff_c = 0;
        for (int i = 0; i <= 86400; i++)
            total_handoff_c += total_handoff[i];
        cout << "total_handoff :" << total_handoff_c << endl;
        //cout << "N_counter :" << N_counter << endl;
        //cout << "P_counter :" << P_counter << endl;
        cout << "AVG_Power :" << P_counter / double(N_counter) << endl;
        N_counter = 0;
        P_counter = 0;
        cout << "*****************************************" << endl;
        cout << endl;
        //output the data end

        //start to write the file
        if (l == 0)
        {
            ofstream output_file0("My_1divide2.csv");
            for (int i = 0; i <= 86400; i++)
                output_file0 << total_handoff[i] << endl;
            output_file0.close();
        }
        if (l == 1)
        {
            ofstream output_file1("My_1divide3.csv");
            for (int i = 0; i <= 86400; i++)
                output_file1 << total_handoff[i] << endl;
            output_file1.close();
        }
        if (l == 2)
        {
            ofstream output_file2("My_1divide5.csv");
            for (int i = 0; i <= 86400; i++)
                output_file2 << total_handoff[i] << endl;
            output_file2.close();
        }
        //write the file end
    }
    //my end**********************************************************************
}

double distance(int choose, car thecar)
{
    double newd = 0;
    if (choose == 1)
        newd = sqrt(pow(thecar.pos_x - station_Ax, 2) + pow(thecar.pos_y - station_Ay, 2));
    else if (choose == 2)
        newd = sqrt(pow(thecar.pos_x - station_Bx, 2) + pow(thecar.pos_y - station_By, 2));
    else if (choose == 3)
        newd = sqrt(pow(thecar.pos_x - station_Cx, 2) + pow(thecar.pos_y - station_Cy, 2));
    else if (choose == 4)
        newd = sqrt(pow(thecar.pos_x - station_Dx, 2) + pow(thecar.pos_y - station_Dy, 2));
    else
        newd = -1;
    return newd;
}
void turn(car &thecar, int choose) //1:left, 2:straight, 3:right
{
    if (choose == 1)
        thecar.face--;
    if (choose == 3)
        thecar.face++;
    if (thecar.face == 5)
        thecar.face = 1;
    if (thecar.face == 0)
        thecar.face = 4;
}
void policy_best(car &thecar)
{
    double newd = distance(1, thecar);
    int news = 1;
    if (newd > distance(2, thecar))
    {
        newd = distance(2, thecar);
        news = 2;
    }
    if (newd > distance(3, thecar))
    {
        newd = distance(3, thecar);
        news = 3;
    }

    if (newd > distance(4, thecar))
    {
        newd = distance(4, thecar);
        news = 4;
    }
    if (thecar.station != news)
    {
        thecar.station = news;
        thecar.change = 1;
    }
    else
        thecar.change = 0;
}
void policy_threshold(car &thecar, double thres)
{
    double newd = distance(1, thecar);
    int news = 1;
    if (newd > distance(2, thecar))
    {
        newd = distance(2, thecar);
        news = 2;
    }
    if (newd > distance(3, thecar))
    {
        newd = distance(3, thecar);
        news = 3;
    }

    if (newd > distance(4, thecar))
    {
        newd = distance(4, thecar);
        news = 4;
    }
    if (thecar.station != news && get_Power(thecar, thecar.station) < thres)
    {
        thecar.station = news;
        thecar.change = 1;
    }
    else
        thecar.change = 0;
}
void policy_entropy(car &thecar, double entor)
{
    double newd = distance(1, thecar);
    int news = 1;
    if (newd > distance(2, thecar))
    {
        newd = distance(2, thecar);
        news = 2;
    }
    if (newd > distance(3, thecar))
    {
        newd = distance(3, thecar);
        news = 3;
    }

    if (newd > distance(4, thecar))
    {
        newd = distance(4, thecar);
        news = 4;
    }
    if (thecar.station != news && get_Power(thecar, news) > (get_Power(thecar, thecar.station) + entor))
    {
        thecar.station = news;
        thecar.change = 1;
    }
    else
        thecar.change = 0;
}
void policy_my(car &thecar)
{
    double newd = distance(1, thecar);
    int news = 1;
    if (newd > distance(2, thecar))
    {
        newd = distance(2, thecar);
        news = 2;
    }
    if (newd > distance(3, thecar))
    {
        newd = distance(3, thecar);
        news = 3;
    }

    if (newd > distance(4, thecar))
    {
        newd = distance(4, thecar);
        news = 4;
    }
    if (thecar.station != news)
    {
        if (get_Power(thecar, thecar.station) < 15 && get_Power(thecar, news) > (get_Power(thecar, thecar.station) + 13))
        {
            thecar.station = news;
            thecar.change = 1;
        }
        else
            thecar.change = 0;
    }
    else
        thecar.change = 0;
}
double get_Power(car &thecar, int choose)
{
    double dis = distance(choose, thecar);
    double output;
    output = 100 - 33 - 20 * log10(dis);
    if (output < 10)
        output = 0;
    return output;
}