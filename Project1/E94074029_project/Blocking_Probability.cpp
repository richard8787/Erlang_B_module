#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <queue>
#include <random>
#include <vector>
using namespace std;
int main()
{
    int c = 4;
    int num_arrival = 0;
    int num_block = 0;
    int in_server = 0;
    int in_queue = 0;
    float sum_arrival_time;
    float get_arrival_time;
    float get_departure_time;
    ofstream output_file("Blocking_Probability_Table.csv");
    vector<float> BP;
    vector<float> ldm;
    queue<float> arrival_time;
    priority_queue<float, vector<float>, greater<float>> departure_time;
    random_device rd;
    mt19937 rng(rd());
    for (int table = 0; table < 2; table++)
    {
        if (table == 1)
            c = 4;

        for (int s = 1; s <= 10; s += c, c++)
            for (float lunda = 0.01; lunda <= 10; lunda *= 10)
                for (float mu = 0.01; mu <= 10.24; mu *= 4)
                {
                    //initialize the num_arrival, num_block
                    num_arrival = 0;
                    num_block = 0;

                    for (int simulation = 0; simulation < 10; simulation++)
                    {
                        //initialize the per simulation time
                        in_server = 0;
                        in_queue = 0;
                        sum_arrival_time = 0;
                        exponential_distribution<float> arrival_time_g(lunda);
                        exponential_distribution<float> departure_time_g(mu);

                        //generate arrival_time
                        while (sum_arrival_time <= 100000)
                        {
                            get_arrival_time = arrival_time_g.operator()(rng);
                            sum_arrival_time += get_arrival_time;
                            arrival_time.push(sum_arrival_time);
                            num_arrival++;
                        }
                        departure_time.push(150000);

                        if (table == 0) // queue = 0
                        {
                            //start to simulate
                            while (!arrival_time.empty())
                            {
                                if (arrival_time.front() < departure_time.top()) //term to arrival
                                {
                                    if (in_server < s)
                                    {
                                        get_departure_time = arrival_time.front() + departure_time_g.operator()(rng);
                                        departure_time.push(get_departure_time);
                                        arrival_time.pop();
                                        in_server++;
                                    }
                                    else //in_server is full
                                    {
                                        arrival_time.pop();
                                        num_block++;
                                    }
                                }
                                else //term to departure
                                {
                                    departure_time.pop();
                                    in_server--;
                                }
                            }
                        }
                        else // queue = S
                        {
                            while (!arrival_time.empty())
                            {
                                if (arrival_time.front() < departure_time.top()) //term to arrival
                                {
                                    if (in_server < s) // server not full
                                    {
                                        get_departure_time = arrival_time.front() + departure_time_g.operator()(rng);
                                        departure_time.push(get_departure_time);
                                        arrival_time.pop();
                                        in_server++;
                                    }
                                    else if (in_queue < s) // server is full, queue not full
                                    {
                                        arrival_time.pop();
                                        in_queue++;
                                    }
                                    else //all is full
                                    {
                                        arrival_time.pop();
                                        num_block++;
                                    }
                                }
                                else //term to departure
                                {
                                    if (in_server == s && in_queue != 0) // arrival some is in_queue
                                    {
                                        get_departure_time = departure_time.top() + departure_time_g.operator()(rng);
                                        departure_time.push(get_departure_time);
                                        departure_time.pop();
                                        in_queue--;
                                    }
                                    else // arrival only in_server
                                    {
                                        departure_time.pop();
                                        in_server--;
                                    }
                                }
                            }
                        }
                        while (!departure_time.empty())
                            departure_time.pop();
                    }
                    //put data into output vector
                    float p = float(num_block) / float(num_arrival);
                    BP.push_back(p * 100);
                    ldm.push_back(lunda / mu);
                }
    }

    //output
    int count_BP = 0;
    int count_ldm = 0;

    // output queue = 0
    output_file << "Queue = 0 " << endl;
    output_file << "(server #),"
                << "Blocking Probability and Erlang value" << endl;
    output_file << "1";
    for (; count_BP < 24; count_BP++)
        output_file << "," << BP[count_BP] << "%";
    output_file << endl;
    for (; count_ldm < 24; count_ldm++)
        output_file << "," << ldm[count_ldm];
    output_file << endl
                << endl;
    output_file << "5";
    for (; count_BP < 48; count_BP++)
        output_file << "," << BP[count_BP] << "%";
    output_file << endl;
    for (; count_ldm < 48; count_ldm++)
        output_file << "," << ldm[count_ldm];
    output_file << endl
                << endl;
    output_file << "10";
    for (; count_BP < 72; count_BP++)
        output_file << "," << BP[count_BP] << "%";
    output_file << endl;
    for (; count_ldm < 72; count_ldm++)
        output_file << "," << ldm[count_ldm];
    output_file << endl
                << endl;

    // output queue = S
    output_file << "Queue = S " << endl;
    output_file << "(server #),"
                << "Blocking Probability and Erlang value" << endl;
    output_file << "1";
    for (; count_BP < 96; count_BP++)
        output_file << "," << BP[count_BP] << "%";
    output_file << endl;
    for (; count_ldm < 96; count_ldm++)
        output_file << "," << ldm[count_ldm];
    output_file << endl
                << endl;
    output_file << "5";
    for (; count_BP < 120; count_BP++)
        output_file << "," << BP[count_BP] << "%";
    output_file << endl;
    for (; count_ldm < 120; count_ldm++)
        output_file << "," << ldm[count_ldm];
    output_file << endl
                << endl;
    output_file << "10";
    for (; count_BP < 144; count_BP++)
        output_file << "," << BP[count_BP] << "%";
    output_file << endl;
    for (; count_ldm < 144; count_ldm++)
        output_file << "," << ldm[count_ldm];

    //output end, close the file
    output_file.close();
}