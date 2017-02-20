#ifndef _VISUALIXERSIMULATION_H
#define _VISUALIXERSIMULATION_H

#include "SimulationData.hpp"
#include "Visualixer.hpp"
#include "Hull.hpp"


#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>

// this needs to be able to view static images or "movies" from a simulation
// One of the options for a static image in 3D could be slices in x, y, z
// ... this could also be a dynamic image option
//*********** Simulation Data Visualixer ****************
class simulation_visualixer : public visualixer{
public:
	simulation_visualixer();
	simulation_visualixer(const SimulationData & simdat);
	~simulation_visualixer();

	// inspectors


	// mutators
	void bind_simulation(const SimulationData & simdat);
	void set_snapshot_increment(unsigned int inc);
	void set_colorby_field(std::string fieldname);
	void set_alpha_field(std::string fieldname);

	// action
	//void run();

protected:

private:

	void increment_time_step();

	void onKeyF();

	void onPrepareData();
	void onAlpha();
	//bool MainLoop();
	void PreLoopTasks();
	void LoopTasks();

	const SimulationData * _simdata;

	std::string _colorby_field, _alpha_field;
	unsigned int _freq_Hz, _cur_time_step, _increment_val; // frequency at which the simulation plays
	unsigned int _colorby_field_index;
	const Mesh * _mesh;
	std::vector<bool> m_subset;
	double m_xwidth, m_ywidth, m_zwidth;
	double m_xslice, m_yslice, m_zslice;

	double m_curTime, m_lastTime;
};

#endif
