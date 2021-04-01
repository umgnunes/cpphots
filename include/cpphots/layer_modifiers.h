/**
 * @file layer_modifiers.h
 * @brief Components that modify the behaviour of Layers
 */
#ifndef CPPHOTS_LAYER_MODIFIERS_H
#define CPPHOTS_LAYER_MODIFIERS_H

#include <vector>

#include "time_surface.h"


namespace cpphots {

/**
 * @brief Interface for a modifier that remaps events
 * 
 * A remapper usually changes the coordinates or the polarity of an event,
 * withoud modifying the event timestamp.
 */
struct EventRemapper {

    /**
     * @brief Remap event
     
     * @param ev event
     * @param k cluster id
     * @return remapped event
     */
    virtual event remapEvent(event ev, uint16_t k) = 0;

};

/**
 * @brief Changes the output of a Layer to ArrayHOTS
 * 
 * If added to a Layer, output events will be emitted as:
 * {t, k, y, 0}
 * where k is the clustering output.
 */
struct ArrayLayer : public EventRemapper {

    event remapEvent(event ev, uint16_t k) override;

};

/**
 * @brief Changes the output of a Layer to a single dimension
 * 
 * If added to a Layer, output events will be remapped to a single dimension,
 * as follows:
 * {t, w*h*k + w*y + x, 0, 0}
 * where k is the clustering output and w,h are the dimensions of the context.
 */
class SerializingLayer : public EventRemapper {

public:

    /**
     * @brief Construct a new SerializingLayer modifier
     * 
     * @param width horizontal size of the context
     * @param height vertical size of the context
     */
    SerializingLayer(uint16_t width, uint16_t height);

    event remapEvent(event ev, uint16_t k) override;

    /**
     * @brief Returns the size of the context
     * 
     * @return {width, height}
     */
    std::pair<uint16_t, uint16_t> getSize() const;

private:
    uint16_t w, h;

};


/**
 * @brief Subsample layer into super cells
 * 
 * If added to a layer, the output coordinates of the events will be mapped
 * using cells of a fixed size, thus reducing the output dimensionality.
 * Cells may be overlapping, causing the layer to emit more than one event.
 */
class SuperCell {

public:
    /**
     * @brief Construct a new SuperCell modifier
     * 
     * @param width width of the context
     * @param height height of the context
     * @param K size of the cells
     * @param overlap cells overlap
     */
    SuperCell(uint16_t width, uint16_t height, uint16_t K, uint16_t overlap = 0);

    /**
     * @brief Find coordinates of cells that are over certain coordinates
     * 
     * The output contains more than one set of coordinates only if overlap > 0.
     * 
     * @param ex x coordinate of the event
     * @param ey y coordinate of the event
     * @return vector of cell coordinates
     */
    std::vector<std::pair<uint16_t, uint16_t>> findCells(uint16_t ex, uint16_t ey) const;

    /**
     * @brief Returns the size of the context
     * 
     * @return {width, height}
     */
    std::pair<uint16_t, uint16_t> getSize() const;

    /**
     * @brief Get the number of the horizonat and vertical cells
     * 
     * @return {horizontal cells, vertical cells}
     */
    std::pair<uint16_t, uint16_t> getCellSizes() const;

protected:

    /**
     * @brief Width of the context
     */
    uint16_t width;

    /**
     * @brief Height of the context
     */
    uint16_t height;

    /**
     * @brief Size of the cells
     */
    uint16_t K;

    /**
     * @brief Overlap of the cells
     */
    uint16_t o;

    /**
     * @brief Number of cells (horizontal)
     */
    uint16_t wcell;

    /**
     * @brief Number of cells (vertical)
     */
    uint16_t hcell;

    /**
     * @brief Max horizontal processing size
     */
    uint16_t wmax;

    /**
     * @brief Max vertical processing size
     */
    uint16_t hmax;

    /**
     * @brief Get the center of a cell in event-space
     * 
     * @param cx cell x
     * @param cy cell y
     * @return event-space coordinates of the center
     */
    std::pair<uint16_t, uint16_t> getCellCenter(uint16_t cx, uint16_t cy) const;

    /**
     * @brief Check whether some events coordinates are in a certain cell or not
     * 
     * @param cx cell x
     * @param cy cell y
     * @param ex event x
     * @param ey event y
     * @return true if event is in cell
     * @return false otherwise
     */
    bool isInCell(uint16_t cx, uint16_t cy, uint16_t ex, uint16_t ey) const;

};


/**
 * @brief Average time surfaces over cells
 * 
 * If added to a layer, time surfaces will be averaged over supercells.
 */
class SuperCellAverage : public SuperCell {

public:
    /**
     * @brief Construct a new SuperCellAverage modifier
     * 
     * @param width width of the context
     * @param height height of the context
     * @param K size of the cells
     * @param overlap cells overlap
     */
    SuperCellAverage(uint16_t width, uint16_t height, uint16_t K, uint16_t overlap = 0);

    /**
     * @brief Average time surfaces over a cell
     * 
     * @param ts new time surface computed
     * @param cx x coordinate of the cell
     * @param cy y coordinate of the cell
     * @return averaged time surface
     */
    TimeSurfaceType averageTS(const TimeSurfaceType& ts, uint16_t cx, uint16_t cy);

private:

    struct CellMem {
        cpphots::TimeSurfaceType ts;
        unsigned int count;
    };

    std::vector<std::vector<CellMem>> cells;

};

}

#endif