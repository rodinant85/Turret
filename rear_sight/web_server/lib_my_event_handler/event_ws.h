//
// Created by biba_bo on 2020-08-18.
//

#ifndef IRON_TURTLE_EVENT_WS_H
#define IRON_TURTLE_EVENT_WS_H

/**These is a class EventWS, which used for contains an event state*/
class EventWS {
private:
    /**logical field with information about EVENT*/
    int _idEvent;

    /**
     * @brief field contains specific data for the current task
     */
    double _data;

    int _width, _height;

    int _x, _y;

    /**
     * @brief field contains specific data for the current task
     */
    double _dataSecond;

public:
    /**Constructor for creating a new EventWS object,
     * ~>public constructor*/
    EventWS(int);

    /**
     * @brief Constructor for creating a new EventWS object with data information,
     * @param idEvent - event code for the current event
     * @param data - specific data for current event
     */
    EventWS(int idEvent, double data);

    /**
     * @brief Constructor for creating a new EventWS object with data information,
     * @param idEvent - event code for the current event
     * @param dataFirst - specific data for current event
     * @param dataSecond - specific data for current event
     */
    EventWS(int idEvent, double dataFirst, double dataSecond);

    EventWS(int idEvent, double data, int width, int height);

    EventWS(int idEvent, int x_coord, int y_coord);

    /**function for getting an event information
     * ~>public function*/
    int getEventID();

    /**
     * @brief this function returns a data for current task
     * @return specific data for the current event
     */
    double getData();

    /**
     * @brief this function returns a data for current task
     * @return specific data for the current event
     */
    double getDataFirst();

    /**
     * @brief this function returns a data for current task
     * @return specific data for the current event
     */
    double getDataSecond();

    int getWidth();
    int getHeight();

    int getX();
    int getY();
};

#endif //IRON_TURTLE_EVENT_WS_H
