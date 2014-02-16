#include "p2d/app/way.h"

#include <geometry2d.h>

sad::p2d::app::Way::Way()
{
	m_constructed = false;
	m_closed = false;
	m_totaltime = 100.0;
}

sad::p2d::app::Way::~Way()
{

}


void sad::p2d::app::Way::step(
	sad::p2d::app::Way::WayLink * link, 
	double step,
	sad::p2d::Point & p
)
{
	assert( m_constructed );
	if (m_waypoints.size() == 1)
	{
		p = m_waypoints[0];
		link->CurrentTime += step;
		return;
	}

	double time = link->CurrentTime  + step;
	if (time > m_totaltime  && !m_closed)
	{
		p = m_waypoints[m_waypoints.size() - 1];
		link->CurrentTime += step;
		return;
	}

	while(time > m_totaltime)
		time -= m_totaltime;

	int index = -1;
	for(unsigned int i = 0; (i < m_times.size() - 1) && index == -1; i++)
	{
		if (time >= m_times[i] && time <= m_times[i+1])
		{
			index = i;
		}
	}
	double timespan = m_times[index+1] - m_times[index];
	double relativetime = time - m_times[index];
	sad::p2d::Point p1 = m_waypoints[index]; 
	sad::p2d::Point p2;
	if (index + 1 == m_waypoints.size())
	{
		p2 = m_waypoints[0];
	}
	else
	{
		p2 = m_waypoints[index+1];
	}
	p = p1 + (p2-p1) * (relativetime / timespan);	
	link->CurrentTime += step;
}


void sad::p2d::app::Way::setPoint(int i,  const sad::p2d::app::WayPoint & p)
{
	assert( !m_constructed );
	m_waypoints[i] = p;
}

void sad::p2d::app::Way::addPoint(const sad::p2d::app::WayPoint & p)
{
	assert( !m_constructed );
	m_waypoints << p;
}


void sad::p2d::app::Way::removePoint(int i)
{
	assert( !m_constructed );
	m_waypoints.removeAt(i);
}


void sad::p2d::app::Way::makeClosed()
{
	m_closed = true;
}

void sad::p2d::app::Way::makeOpen()
{
	m_closed = false;
}

void sad::p2d::app::Way::setTotalTime(double time)
{
	assert( !m_constructed );
	m_totaltime = time;
}

void sad::p2d::app::Way::startConstruction()
{
	m_constructed = false;
}


void sad::p2d::app::Way::construct()
{
	assert( m_waypoints.size() > 1  && sad::non_fuzzy_zero(m_totaltime) );
	m_constructed = true;
	double curtime = 0;
	double totaldistance = 0;
	for(unsigned int i = 0; i < m_waypoints.size() - 1; i++)
	{
		totaldistance += m_waypoints[i].distance(m_waypoints[i+1]);
	}
	if (m_closed)
	{
		totaldistance +=  m_waypoints[0].distance(m_waypoints[m_waypoints.size()-1]);
	}

	double avspeed = totaldistance / m_totaltime;
	m_times.clear();
	m_times << 0;
	for(unsigned int i = 1; i < m_waypoints.size(); i++)
	{
		double d = m_waypoints[i-1].distance(m_waypoints[i]);
		curtime += d / avspeed;
		m_times << curtime;
	}
	if (m_closed)
	{
		m_times << m_totaltime;
	}
}