#ifndef _VISUALIXERCLOUD_H
#define _VISUALIXERCLOUD_H

#include "PointCloud.hpp"
#include "Visualixer.hpp"


#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>

//*********** Point Cloud Visualixer ****************
class cloud_visualixer : public visualixer{
public:
	cloud_visualixer();
	~cloud_visualixer();

	void add_cloud(const PointCloud & cloud);
	void set_test_case();

protected:

	void onPrepareData();

	const PointCloud * _cloud;

};

#endif