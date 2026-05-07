#include <iostream>
#include <random>
#include <ctime>
#include <queue>
#include <vector>
using namespace std;
void toggle_coin(char &coin);
void remove_coin(char* coins,int index,int n);
void print_coins(char* coins,int n);
int calc_priority(char* coins,int index,int n);
void check (char* coins,int n);
struct CoinMove {
    int index;
    int priority;

    bool operator < (const CoinMove& other) const{
        return priority<other.priority;
    }
};
int main()
{   int n;
    mt19937 gen(static_cast<unsigned int>(time(0)));
    uniform_int_distribution<> dis(0, 1);

    cout << "Enter number of coins: ";
    cin>>n;
    while(n<=0)
    {
        cout<<"Number of coins must be greater than zero"<<endl;
        cout<<"Enter number of coins again: ";
        cin>>n;
    }
    cout<<"You entered number of coins: "<<n<<endl;

    char* coins= new char [n];
    for (int i=0;i<n;i++)
    {
        if(dis(gen) == 0)
            coins[i] ='H';
        else
            coins[i] = 'T';
        cout<<coins[i]<<" ";
    }

    cout<<endl<<"###############"<<endl;
    priority_queue<CoinMove> pq;
    int remove_index;
    for(int j =0;j<n;j++)
    {
        while(!pq.empty())
        {
                pq.pop();
        }
        for(int i =0;i<n;i++)
        {
            if(coins[i]=='H')
            {
                CoinMove coin = {i,calc_priority(coins,i,n)};
                pq.push(coin);
            }
        }
        if(!pq.empty())
        {
            CoinMove top_coin = pq.top();
            remove_index = top_coin.index;
            remove_coin(coins,remove_index,n);
            print_coins(coins,n);
        }
    }
    check(coins,n);
    return 0;
}

void remove_coin(char* coins,int index,int n)
{
    coins[index] = '-';

    if(index != 0)
        toggle_coin(coins[index-1]);
    if(index != n-1)
        toggle_coin(coins[index+1]);

}
void toggle_coin(char &coin){
    if(coin == 'H')
        coin = 'T';
    else if(coin == 'T')
        coin = 'H';
}
void print_coins(char* coins,int n)
{
    for (int i=0;i<n;i++)
    {
        cout<<coins[i]<<" ";
    }
    cout<<endl;
}
void check (char* coins,int n)
{
    for(int i=0;i<n;i++)
    {
        if(coins[i] !='-')
        {
            cout<<"Test failed.";
            return;
        }
    }
    cout<<"Test passed";
}
int calc_priority(char* coins,int index,int n)
{
    int priority = 0;
    if(index != 0)
    {
        if(coins[index-1]=='T')
            priority += 2;
        else if (coins[index-1] == '-')
            priority += 1;
    }
    if(index != n-1)
    {

        if(coins[index+1]=='T')
            priority+=2;
        else if (coins[index+1] == '-')
            priority+=1;
    }
    return priority;
}
