#include "rosout_publisher.h"
#include <PlotJuggler/any.hpp>
#include "../shape_shifter_factory.hpp"
#include "../qnodedialog.h"
#include <QMainWindow>
#include <QSettings>

RosoutPublisher::RosoutPublisher():
    enabled_(false )
{

}

RosoutPublisher::~RosoutPublisher()
{

}

void RosoutPublisher::setEnabled(bool to_enable)
{
    enabled_ = to_enable;

    if( enabled())
    {
        _minimum_time_usec = std::numeric_limits<int64_t>::max();
        _maximum_time_usec = std::numeric_limits<int64_t>::min();

        _log_window = new QMainWindow();

        auto logwidget = new rqt_console_plus::LogWidget(_tablemodel, _log_window);
        _log_window->setCentralWidget( logwidget );
        Qt::WindowFlags flags = _log_window->windowFlags();
        _log_window->setWindowFlags( flags | Qt::SubWindow );

        connect( this, SIGNAL(timeRangeChanged(TimePoint,TimePoint)),
                 logwidget, SLOT(on_timeRangeChanged(TimePoint,TimePoint)) );

        QSettings settings( "IcarusTechnology", "PlotJuggler");
        _log_window->restoreGeometry(settings.value("RosoutPublisher.geometry").toByteArray());

        _log_window->show();
    }
    else{
        if( _log_window ) {

            QSettings settings( "IcarusTechnology", "PlotJuggler");
            settings.setValue("RosoutPublisher.geometry", _log_window->saveGeometry());

            _log_window->setAttribute(Qt::WA_DeleteOnClose, true);
            _log_window->close();
            _tablemodel.removeRows(0, _tablemodel.rowCount() );
        }
    }
}


std::vector<const PlotDataAny *> RosoutPublisher::findRosoutTimeseries(PlotDataMap *datamap)
{
    std::vector<const PlotDataAny*> logs_timeseries;

    for(const auto& data_it:  datamap->user_defined )
    {
        const std::string&    topic_name = data_it.first;

        // check if I registered this message before
        nonstd::optional<RosIntrospection::ShapeShifter*> registered_shapeshifted_msg = ShapeShifterFactory::getInstance().getMessage( topic_name );
        if( ! registered_shapeshifted_msg )
        {
            continue; // will not be able to use this anyway, just skip
        }

        RosIntrospection::ShapeShifter* shapeshifted_msg = registered_shapeshifted_msg.value();
        if(shapeshifted_msg->getMD5Sum() != std::string(ros::message_traits::MD5Sum< rosgraph_msgs::Log >::value()) )
        {
            continue; // it is NOT a rosgraph_msgs::Log
        }

        logs_timeseries.push_back( data_it.second.get() );
    }

    return logs_timeseries;
}

void RosoutPublisher::syncWithTableModel(const std::vector<const PlotDataAny*>& logs_timeseries)
{
    const int64_t threshold_time = _maximum_time_usec;

    std::vector<rosgraph_msgs::LogConstPtr> logs;
    logs.reserve(100);

    // most of the time we expect logs_timeseries to have just 1 element
    for(const  PlotDataAny* type_erased_logs:  logs_timeseries )
    {
        const int first_index = type_erased_logs->getIndexFromX( threshold_time );

        if( first_index != -1)
        { 
            for( int i=first_index; i< type_erased_logs->size(); i++)
            {
                auto        any_msg   = type_erased_logs->at(i);
                nonstd::any any_value = any_msg.y;

                std::vector<uint8_t> raw_buffer =  nonstd::any_cast<std::vector<uint8_t>>( any_value );

                rosgraph_msgs::LogPtr p(boost::make_shared<rosgraph_msgs::Log>());
                ros::serialization::IStream stream(raw_buffer.data(), raw_buffer.size() );
                ros::serialization::deserialize(stream, *p);

                int64_t usec = p->header.stamp.toNSec() / 1000;
                _minimum_time_usec = std::min( _minimum_time_usec, usec);
                _maximum_time_usec = std::max( _maximum_time_usec, usec);

                if( usec >= threshold_time){
                    logs.push_back( p );
                }
            } 
        }
    }
    std::sort( logs.begin(), logs.end(), [](const rosgraph_msgs::LogConstPtr& a, const rosgraph_msgs::LogConstPtr& b)
    {
        return a->header.stamp < b->header.stamp;
    } );
    _tablemodel.push_back( logs );
}

void RosoutPublisher::updateState(PlotDataMap *datamap, double current_time)
{
    if(!enabled_) return;

    std::vector<const PlotDataAny*> logs_timeseries = findRosoutTimeseries(datamap);

    syncWithTableModel(logs_timeseries);

    using namespace std::chrono;
    TimePoint p_min  = TimePoint() + microseconds(_minimum_time_usec);
    TimePoint p_max  = TimePoint() + microseconds(_maximum_time_usec);
    TimePoint p_curr = TimePoint() + microseconds( (int64_t)(current_time*1000000));

    emit timeRangeChanged(p_min, p_curr);

}
