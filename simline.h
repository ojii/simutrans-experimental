/*
 * part of the Simutrans project
 * @author hsiegeln
 * 01/12/2003
 */
#ifndef simline_h
#define simline_h

#include "convoihandle_t.h"
#include "linehandle_t.h"
#include "simtypes.h"

#include "tpl/minivec_tpl.h"
#include "tpl/vector_tpl.h"
#include "utils/plainstring.h"
#include "tpl/koordhashtable_tpl.h"

#define MAX_MONTHS				12 // Max history
#define MAX_NON_MONEY_TYPES		4 // number of non money types in line's financial statistic

#define LINE_CAPACITY			0 // the amount of ware that could be transported, theoretically
#define LINE_TRANSPORTED_GOODS	1 // the amount of ware that has been transported
#define LINE_AVERAGE_SPEED		2 // The average speed of all convoys in the line
#define LINE_COMFORT			3 // The average comfort rating of all vehicles on this line (weighted by numbers)
#define LINE_REVENUE			4 // the income this line generated
#define LINE_OPERATIONS         5 // the cost of operations this line generated
#define LINE_PROFIT             6 // total profit of line
#define LINE_CONVOIS			7 // number of convois for this line
#define LINE_DISTANCE		    8 // distance converd by all convois
#define LINE_REFUNDS			9 // Total refunds paid to passengers/goods owners desiring to use this line but kept waiting too long to do so.
#define MAX_LINE_COST			10 // Total number of cost items

class karte_t;
class loadsave_t;
class spieler_t;
class schedule_t;

class simline_t {

public:
	enum linetype { line = 0, truckline = 1, trainline = 2, shipline = 3, airline = 4, monorailline=5, tramline=6, maglevline=7, narrowgaugeline=8, MAX_LINE_TYPE};

	typedef koordhashtable_tpl<id_pair, average_tpl<uint16> > journey_times_map;

protected:
	schedule_t * fpl;
	spieler_t *sp;
	linetype type;

	bool withdraw;

private:
	static karte_t * welt;
	plainstring name;

	/**
	 * Handle for ourselves. Can be used like the 'this' pointer
	 * Initialized by constructors
	 * @author Hj. Malthaner
	 */
	linehandle_t self;

	/*
	 * the current state saved as color
	 * Meanings are BLACK (ok), WHITE (no convois), YELLOW (no vehicle moved), RED (last month income minus), BLUE (at least one convoi vehicle is obsolete)
	 */
	uint8 state_color;

	/*
	 * a list of all convoys assigned to this line
	 * @author hsiegeln
	 */
	vector_tpl<convoihandle_t> line_managed_convoys;

	/*
	 * @author hsiegeln
	 * a list of all catg_index, which can be transported by this line.
	 */
	minivec_tpl<uint8> goods_catg_index;

	/*
	 * struct holds new financial history for line
	 * @author hsiegeln
	 */
	sint64 financial_history[MAX_MONTHS][MAX_LINE_COST];

	/**
	 * creates empty schedule with type depending on line-type
	 */
	void create_schedule();

	void init_financial_history();

	/*
	 * whether the next convoy applied to this line should have its
	 * reverse_schedule flag set. Only applies to bidirectional schedules.
	 * @author yobbobandana
	 */
	bool start_reversed;

	uint16 livery_scheme_index;

	/**
	* The table of point-to-point average speeds.
	* @author jamespetts
	*/
	journey_times_map *average_journey_times;
	journey_times_map * average_journey_times_reverse_circular;

	bool is_alternating_circle_route;

public:
	simline_t(karte_t* welt, spieler_t *sp, linetype type);
	simline_t(karte_t* welt, spieler_t *sp, linetype type, loadsave_t *file);

	~simline_t();

	linehandle_t get_handle() const { return self; }

	/*
	 * add convoy to route
	 * @author hsiegeln
	 */
	void add_convoy(convoihandle_t cnv, bool from_loading = false);

	/*
	 * remove convoy from route
	 * @author hsiegeln
	 */
	void remove_convoy(convoihandle_t cnv);

	/*
	 * get convoy
	 * @author hsiegeln
	 */
	convoihandle_t get_convoy(int i) const { return line_managed_convoys[i]; }

	/*
	 * return number of manages convoys in this line
	 * @author hsiegeln
	 */
	uint32 count_convoys() const { return line_managed_convoys.get_count(); }

	/*
	 * returns the state of the line
	 * @author prissi
	 */
	uint8 get_state_color() const { return state_color; }

	/*
	 * return fahrplan of line
	 * @author hsiegeln
	 */
	schedule_t * get_schedule() const { return fpl; }

	void set_schedule(schedule_t* fpl);

	/*
	 * get name of line
	 * @author hsiegeln
	 */
	char const* get_name() const { return name; }
	void set_name(const char *str) { name = str; }

	/*
	 * load or save the line
	 */
	void rdwr(loadsave_t * file);

	/**
	 * method to load/save linehandles
	 */
	static void rdwr_linehandle_t(loadsave_t *file, linehandle_t &line);

	/*
	 * register line with stop
	 */
	void register_stops(schedule_t * fpl);

	void laden_abschliessen();

	/*
	 * unregister line from stop
	 */
	void unregister_stops(schedule_t * fpl);
	void unregister_stops();

	/*
	 * renew line registration for stops
	 */
	void renew_stops();

	// called after tiles are removed from stations to change the load of connected convois
	void check_freight();

	sint64* get_finance_history() { return *financial_history; }

	sint64 get_finance_history(int month, int cost_type) const { return financial_history[month][cost_type]; }

	void book(sint64 amount, int cost_type) 
	{
		if(cost_type != LINE_AVERAGE_SPEED && cost_type != LINE_COMFORT)
		{
			financial_history[0][cost_type] += amount; 
		}
		else
		{
			// Average types
			rolling_average[cost_type] += (uint32)amount;
			rolling_average_count[cost_type] ++;
			const sint64 tmp = (sint64)rolling_average[cost_type] / (sint64)rolling_average_count[cost_type];
			financial_history[0][cost_type] = tmp;
		}
	}

	static uint8 convoi_to_line_catgory(uint8 convoi_cost_type);

	void new_month();

	linetype get_linetype() { return type; }

	const minivec_tpl<uint8> &get_goods_catg_index() const { return goods_catg_index; }

	// recalculates the good transported by this line and (in case of changes) will start schedule recalculation
	void recalc_catg_index();

	int get_replacing_convoys_count() const;

	// @author: jamespetts
	uint32 rolling_average[MAX_LINE_COST];
	uint16 rolling_average_count[MAX_LINE_COST];

	//@author: jamespetts
	bool has_overcrowded() const;

	void set_withdraw( bool yes_no );

	bool get_withdraw() const { return withdraw; }

	spieler_t *get_besitzer() const {return sp;}

	void recalc_status();

	void set_livery_scheme_index (uint16 index) { livery_scheme_index = index; }
	uint16 get_livery_scheme_index() const { return livery_scheme_index; }
	void propogate_livery_scheme();

	inline journey_times_map * get_average_journey_times() { return average_journey_times; }
	inline journey_times_map * get_average_journey_times_reverse_circular() { return average_journey_times_reverse_circular; }

	void calc_is_alternating_circular_route();

	bool get_is_alternating_circle_route() const { return is_alternating_circle_route; }
};


#endif
