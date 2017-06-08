#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cmath>
#include <string>
#include <cstdlib>


enum battery_state { Charging, Discharging };
typedef battery_state bs;


const char* const BAT_CAP{ "/sys/class/power_supply/BAT0/capacity" };
const char* const BAT_STAT{ "/sys/class/power_supply/BAT0/status" };
const int SLEEP_SECS{ 30 };
const int SLEEP_MICRO_SECS{ static_cast<int>(SLEEP_SECS*std::pow(10, 6)) };


double get_battery_capacity();
bs get_battery_status();


int main()
{	
	bool up_alert_made{ 0 };
	bool down_alert_made{ 0 };
	int sleep_time{ 0 };

	while (1)
	{	
		double now = get_battery_capacity();
		bs charge = get_battery_status();

		// Too much charge.
		if (now > 80 && charge != Discharging && ! up_alert_made)
		{
			std::cout << "Above 80 and charging, BAD!" << std::endl;

			std::string title = "\'Battery above 80%. Stop charging!\'";
			std::string message = "\'To keep your battery in a perfect condition you need to stop charging now.\'";
			std::string cmd = "notify-send " + title + " " + message;

			if (! system(cmd.c_str()))
				up_alert_made = true;
			else
			{
				std::cerr << "Error sending an alert! " << std::endl;
				exit(0);
			}			

		}
		// Too less charge.
		else if (now < 30 && charge == Discharging && ! down_alert_made)
		{
			std::cout << "Below 30 and discharging, BAD!" << std::endl;

			std::string title = "\'Battery below 30%. Charge Now.\'";
			std::string message = "\'To keep your battery in a perfect condition you need to connect the charger now.\'";
			std::string cmd = "notify-send " + title + " " + message;

			if (! system(cmd.c_str()))
				down_alert_made = true;
			else
			{
				std::cerr << "Error sending an alert! " << std::endl;
				exit(0);
			}
		}
		// If the charger is disconnected after alert.
		else if (now > 80 && charge == Discharging && up_alert_made)
		{
			std::cout << "Above 80 and discharging, GOOD!" << std::endl;			
			up_alert_made = false;
		}
		// If the charger is connected after alert.
		else if (now < 30 && charge != Discharging && down_alert_made)
		{
			std::cout << "Below 30 and charging, GOOD!" << std::endl;			
			down_alert_made = false;
		}
		else
		{
			// no issues.
		}
		sleep_time = (down_alert_made || up_alert_made) ? (5*std::pow(10, 6)) : SLEEP_MICRO_SECS;
		// Sleep for a while.
		usleep(sleep_time);

	}
	std::cout << get_battery_capacity() << std::endl;
	return 0;
}

/**
 * Gets charging percentage in numbers.
 * @return {double};
 */
double get_battery_capacity()
{	
	double present_battery_capacity;

	std::fstream bat_file;
	bat_file.open(BAT_CAP, std::ios_base::in);

	bat_file >> present_battery_capacity;

	return present_battery_capacity;
};

/**
 * Tells if batter is 'Charging' or 'Discharging'.
 * @return {bs};
 */
bs get_battery_status()
{	
	std::string present_battery_status;
	bs bs1;

	std::fstream bat_file;
	bat_file.open(BAT_STAT, std::ios_base::in);

	bat_file >> present_battery_status;

	if (present_battery_status == "Discharging")
		bs1 = Discharging;		
	else
		bs1 = Charging;
	return bs1;
};